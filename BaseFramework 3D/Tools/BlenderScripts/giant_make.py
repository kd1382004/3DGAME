# Giantblend.blend：ボスの単発攻撃アニメを足して、保存・書き出しする
#   DoubleSlam … 両腕を振り上げて前に叩きつける
#   Stomp      … 右足を上げて踏みつけ(周りに衝撃)
#   Spin       … 腕を広げて一回転なぎ払い
#   Roar       … 胸を張って吠える(行動の合間・休憩用)
import bpy, sys, importlib, math, json, os
from mathutils import Vector, Quaternion, Matrix
S = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts"
if S not in sys.path: sys.path.append(S)
import anim_lib as L
importlib.reload(L)
ARM = L.ARM
PB = ARM.pose.bones
P = "mixamorig"
log = {}

if bpy.context.object and bpy.context.object.mode != 'OBJECT':
    bpy.ops.object.mode_set(mode='OBJECT')

def basis(pose, n):
    v = pose[n]
    return Matrix.LocRotScale(v["loc"], v["rot"], v["scale"])

def pose_mat(pose, n):
    pb = PB[n]
    if pb.parent is None:
        return pb.bone.matrix_local @ basis(pose, n)
    p = pb.parent
    return pose_mat(pose, p.name) @ p.bone.matrix_local.inverted() @ pb.bone.matrix_local @ basis(pose, n)

def frame_mat(pose, n):
    pb = PB[n]
    if pb.parent is None:
        return pb.bone.matrix_local.copy()
    p = pb.parent
    return pose_mat(pose, p.name) @ p.bone.matrix_local.inverted() @ pb.bone.matrix_local

def head(pose, n):
    return pose_mat(pose, n).translation.copy()

def move(pose, n, d):
    M = frame_mat(pose, n).to_3x3()
    pose[n]["loc"] = pose[n]["loc"] + M.inverted() @ Vector(d)

def rotate(pose, n, axis, deg):
    M = frame_mat(pose, n).to_3x3() @ pose[n]["rot"].to_matrix()
    a = (M.inverted() @ Vector(axis)).normalized()
    pose[n]["rot"] = (pose[n]["rot"] @ Quaternion(a, math.radians(deg))).normalized()

def rotate_between(pose, n, v_from, v_to):
    v_from = Vector(v_from).normalized(); v_to = Vector(v_to).normalized()
    ax = v_from.cross(v_to)
    if ax.length < 1e-6: return
    ang = math.degrees(v_from.angle(v_to))
    rotate(pose, n, ax.normalized(), ang)

def ik2(pose, upper, lower, end, target, pole):
    """2本の骨(upper, lower)で end の根元を target に届かせる。pole は肘・膝を向ける点"""
    Sp = head(pose, upper); E = head(pose, lower); H = head(pose, end)
    a = (E - Sp).length; b = (H - E).length
    T = Vector(target)
    d = T - Sp
    dist = min(max(d.length, abs(a - b) + 1e-3), a + b - 1e-3)
    dn = d.normalized()
    # 肘の位置(余弦定理)
    cosA = (a * a + dist * dist - b * b) / (2 * a * dist)
    cosA = max(-1.0, min(1.0, cosA))
    pv = Vector(pole) - Sp
    pv = (pv - dn * pv.dot(dn))
    if pv.length < 1e-6: pv = Vector((0, 0, 1)) - dn * dn.z
    pv.normalize()
    Ed = Sp + dn * (a * cosA) + pv * (a * math.sqrt(max(0.0, 1 - cosA * cosA)))
    rotate_between(pose, upper, E - Sp, Ed - Sp)
    E2 = head(pose, lower); H2 = head(pose, end)
    rotate_between(pose, lower, H2 - E2, (Sp + dn * dist) - E2)

def copy_pose(p):
    return {k: {"loc": v["loc"].copy(), "rot": v["rot"].copy(), "scale": v["scale"].copy()} for k, v in p.items()}

def smooth(t):
    t = max(0.0, min(1.0, t)); return t * t * (3 - 2 * t)

def keyed(keys, f):
    for i in range(len(keys) - 1):
        f0, v0 = keys[i]; f1, v1 = keys[i + 1]
        if f0 <= f <= f1:
            u = smooth((f - f0) / max(f1 - f0, 1e-6))
            if isinstance(v0, (tuple, list)):
                return tuple(a + (b - a) * u for a, b in zip(v0, v1))
            return v0 + (v1 - v0) * u
    return keys[-1][1]

def lerpv(a, b, u):
    return tuple(x + (y - x) * u for x, y in zip(a, b))

ORIG = os.path.join(os.path.dirname(bpy.data.filepath), "Giantblend_orig.blend")
if not os.path.exists(ORIG):
    bpy.ops.wm.save_as_mainfile(filepath=ORIG, copy=True)   # 作り変える前のコピー

IDLE = L.fc_map(bpy.data.actions["idle"])
P0 = L.sample(IDLE, 0.0)
B = {k: P + k for k in ("Hips", "Spine", "Spine1", "Spine2", "Neck", "Head",
                        "LeftArm", "LeftForeArm", "LeftHand", "RightArm", "RightForeArm", "RightHand",
                        "LeftUpLeg", "LeftLeg", "LeftFoot", "RightUpLeg", "RightLeg", "RightFoot")}
H0 = {k: head(P0, B[k]) for k in ("Hips", "LeftHand", "RightHand", "LeftFoot", "RightFoot", "LeftArm", "RightArm", "Head")}
log["P0"] = {k: [round(x, 2) for x in v] for k, v in H0.items()}

def T(v): return tuple(v)

def arms(p, lt, rt):
    """左右の手を目標へ(肘は外・後ろへ)"""
    lsh = head(p, B["LeftArm"]); rsh = head(p, B["RightArm"])
    if lt is not None:
        ik2(p, B["LeftArm"], B["LeftForeArm"], B["LeftHand"], lt, lsh + Vector((-1.5, -1.5, -0.5)))
    if rt is not None:
        ik2(p, B["RightArm"], B["RightForeArm"], B["RightHand"], rt, rsh + Vector((1.5, -1.5, -0.5)))

def legs(p, lf, rf):
    """足を目標へ(膝は前へ)"""
    if lf is not None:
        ik2(p, B["LeftUpLeg"], B["LeftLeg"], B["LeftFoot"], lf, head(p, B["LeftLeg"]) + Vector((0, 3, 0)))
    if rf is not None:
        ik2(p, B["RightUpLeg"], B["RightLeg"], B["RightFoot"], rf, head(p, B["RightLeg"]) + Vector((0, 3, 0)))

made = {}

# ---------- 1. DoubleSlam(64f) ----------
def double_slam():
    lh, rh = T(H0["LeftHand"]), T(H0["RightHand"])
    lk = [(0, lh), (20, (-1.3, 0.2, 7.3)), (27, (-1.4, -0.1, 7.6)), (33, (-0.9, 3.6, 0.6)), (44, (-0.9, 3.5, 0.6)), (64, lh)]
    rk = [(0, rh), (20, (1.1, 0.2, 7.3)), (27, (1.2, -0.1, 7.6)), (33, (0.7, 3.6, 0.6)), (44, (0.7, 3.5, 0.6)), (64, rh)]
    bend = [(0, 0.0), (20, -14.0), (27, -18.0), (33, 38.0), (44, 36.0), (64, 0.0)]   # 前屈(度)
    drop = [(0, 0.0), (20, 0.15), (27, 0.2), (33, -0.9), (44, -0.85), (64, 0.0)]
    fr = []
    for f in range(65):
        p = copy_pose(P0)
        move(p, B["Hips"], (0, keyed([(0, 0.0), (33, 0.5), (64, 0.0)], f), keyed(drop, f)))
        rotate(p, B["Spine"], (-1, 0, 0), keyed(bend, f) * 0.4)
        rotate(p, B["Spine1"], (-1, 0, 0), keyed(bend, f) * 0.3)
        rotate(p, B["Spine2"], (-1, 0, 0), keyed(bend, f) * 0.3)
        rotate(p, B["Neck"], (-1, 0, 0), -keyed(bend, f) * 0.3)
        legs(p, T(H0["LeftFoot"]), T(H0["RightFoot"]))
        arms(p, keyed(lk, f), keyed(rk, f))
        fr.append(p)
    return fr
made["DoubleSlam"] = double_slam()

# ---------- 2. Stomp(52f) 右足 ----------
def stomp():
    rf0 = T(H0["RightFoot"]); lf0 = T(H0["LeftFoot"])
    fk = [(0, rf0), (18, (rf0[0] + 0.1, rf0[1] + 0.9, 2.4)), (23, (rf0[0] + 0.1, rf0[1] + 1.0, 2.6)),
          (27, (rf0[0] + 0.1, rf0[1] + 1.3, rf0[2])), (52, (rf0[0] + 0.1, rf0[1] + 1.3, rf0[2]))]
    fk2 = [(36, (rf0[0] + 0.1, rf0[1] + 1.3, rf0[2])), (52, rf0)]
    hx = [(0, 0.0), (18, -0.55), (23, -0.6), (27, 0.0), (52, 0.0)]       # 左足に体重
    hz = [(0, 0.0), (18, 0.1), (23, 0.15), (27, -0.6), (36, -0.55), (52, 0.0)]
    lean = [(0, 0.0), (18, -8.0), (23, -10.0), (27, 14.0), (36, 12.0), (52, 0.0)]
    lh, rh = T(H0["LeftHand"]), T(H0["RightHand"])
    lk = [(0, lh), (18, (-3.0, 0.8, 4.6)), (23, (-3.1, 0.6, 4.9)), (27, (-2.6, 1.6, 2.4)), (36, (-2.6, 1.6, 2.4)), (52, lh)]
    rk = [(0, rh), (18, (2.9, 0.6, 4.4)), (23, (3.0, 0.4, 4.7)), (27, (2.5, 1.8, 2.3)), (36, (2.5, 1.8, 2.3)), (52, rh)]
    fr = []
    for f in range(53):
        p = copy_pose(P0)
        move(p, B["Hips"], (keyed(hx, f), 0, keyed(hz, f)))
        rotate(p, B["Spine"], (-1, 0, 0), keyed(lean, f))
        rotate(p, B["Spine"], (0, 1, 0), -keyed(hx, f) * 10)
        rf = keyed(fk, f) if f <= 36 else keyed(fk2, f)
        legs(p, lf0, rf)
        arms(p, keyed(lk, f), keyed(rk, f))
        fr.append(p)
    return fr
made["Stomp"] = stomp()

# ---------- 3. Spin(64f) 一回転 ----------
def spin():
    lh, rh = T(H0["LeftHand"]), T(H0["RightHand"])
    out_l, out_r = (-4.3, 0.6, 3.9), (4.1, 0.6, 3.9)
    lk = [(0, lh), (16, out_l), (44, out_l), (64, lh)]
    rk = [(0, rh), (16, out_r), (44, out_r), (64, rh)]
    ang = [(0, 0.0), (16, 45.0), (22, 45.0), (40, -360.0), (46, -365.0), (64, -360.0)]
    hz = [(0, 0.0), (16, -0.3), (22, -0.35), (40, -0.3), (64, 0.0)]
    fr = []
    for f in range(65):
        p = copy_pose(P0)
        move(p, B["Hips"], (0, 0, keyed(hz, f)))
        legs(p, T(H0["LeftFoot"]), T(H0["RightFoot"]))
        arms(p, keyed(lk, f), keyed(rk, f))
        # 回転は最後に腰ごと(足も一緒にその場で回る)
        a = keyed(ang, f)
        hip = head(p, B["Hips"])
        rotate(p, B["Hips"], (0, 0, 1), a)
        # 腰の根元を中心に回すので位置はそのまま
        fr.append(p)
    return fr
made["Spin"] = spin()

# ---------- 4. Roar(72f) 吠える ----------
def roar():
    lh, rh = T(H0["LeftHand"]), T(H0["RightHand"])
    lk = [(0, lh), (14, (-2.2, 1.2, 1.6)), (24, (-3.6, 0.4, 5.6)), (56, (-3.7, 0.3, 5.8)), (72, lh)]
    rk = [(0, rh), (14, (2.0, 1.2, 1.6)), (24, (3.4, 0.4, 5.6)), (56, (3.5, 0.3, 5.8)), (72, rh)]
    bend = [(0, 0.0), (14, 18.0), (24, -20.0), (56, -22.0), (72, 0.0)]
    fr = []
    for f in range(73):
        p = copy_pose(P0)
        move(p, B["Hips"], (0, 0, keyed([(0, 0.0), (14, -0.4), (24, 0.1), (56, 0.1), (72, 0.0)], f)))
        rotate(p, B["Spine1"], (-1, 0, 0), keyed(bend, f) * 0.5)
        rotate(p, B["Spine2"], (-1, 0, 0), keyed(bend, f) * 0.5)
        rotate(p, B["Neck"], (-1, 0, 0), keyed(bend, f) * 0.6)
        legs(p, T(H0["LeftFoot"]), T(H0["RightFoot"]))
        arms(p, keyed(lk, f), keyed(rk, f))
        fr.append(p)
    return fr
made["Roar"] = roar()

# ---------- 書き込み・NLA・確認 ----------
ad = ARM.animation_data
for name, frames in made.items():
    act = L.write_action(name, frames)
    tr = ad.nla_tracks.get(name) or ad.nla_tracks.new()
    tr.name = name
    for s in list(tr.strips): tr.strips.remove(s)
    st = tr.strips.new(name, 0, act)
    st.mute = False
    tr.mute = True
    log[name] = {}
    for fidx in range(0, len(frames), 4):
        pp = frames[fidx]
        log[name][fidx] = {k: [round(x, 2) for x in head(pp, B[k])] for k in ("Hips", "LeftHand", "RightHand", "LeftFoot", "RightFoot", "Head")}

# 元のアクションに戻す
ad.action = bpy.data.actions["idle"]
try: ad.action_slot = bpy.data.actions["idle"].slots[0]
except Exception: pass

bpy.ops.wm.save_mainfile()
os.makedirs(S + r"\check", exist_ok=True)
bpy.ops.export_scene.gltf(filepath=S + r"\check\giant_check.gltf", export_format='GLTF_SEPARATE')
json.dump(log, open(S + r"\giant_make_out.json", "w", encoding="utf-8"), ensure_ascii=False)
print("GIANT MAKE DONE")
