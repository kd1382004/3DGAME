# golem_clean.blend：単発攻撃アニメを足し、歩きを分かりやすく作り直して、保存・書き出しする
#   SmashQuick … 速い叩きつけ(smash を縮めたもの)
#   SwipeR / SwipeL … 右腕 / 左腕の横なぎ
#   Stomp … 右足の踏みつけ(周りに衝撃)
#   walk … 歩幅・足の上げ・腰の上下と左右・腕の振りを大きくしたもの(上書き)
import bpy, sys, importlib, math, json, os
from mathutils import Vector, Quaternion, Matrix
S = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts"
if S not in sys.path: sys.path.append(S)
import anim_lib as L
importlib.reload(L)
ARM = L.ARM
PB = ARM.pose.bones
GAME_GLTF = r"C:\GitHub\3DGAME\BaseFramework 3D\Asset\Models\Character\Enemy\Golem\golem_clean.gltf"
log = {}

if bpy.context.object and bpy.context.object.mode != 'OBJECT':
    bpy.ops.object.mode_set(mode='OBJECT')

# ---------- 骨の行列(ポーズ辞書から。コンストレイントは考えない) ----------
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
    """basis を掛ける前の、この骨の座標系(アーマチュア空間)"""
    pb = PB[n]
    if pb.parent is None:
        return pb.bone.matrix_local.copy()
    p = pb.parent
    return pose_mat(pose, p.name) @ p.bone.matrix_local.inverted() @ pb.bone.matrix_local

def move(pose, n, d):
    """アーマチュア空間で d だけ動かす"""
    M = frame_mat(pose, n).to_3x3()
    pose[n]["loc"] = pose[n]["loc"] + M.inverted() @ Vector(d)

def move_to(pose, n, target):
    cur = pose_mat(pose, n).translation
    move(pose, n, Vector(target) - cur)

def rotate(pose, n, axis, deg):
    """アーマチュア空間の軸 axis まわりに deg 度まわす"""
    M = (frame_mat(pose, n).to_3x3() @ pose[n]["rot"].to_matrix())
    a = (M.inverted() @ Vector(axis)).normalized()
    pose[n]["rot"] = pose[n]["rot"] @ Quaternion(a, math.radians(deg))

def head(pose, n):
    return pose_mat(pose, n).translation.copy()

def copy_pose(p):
    return {k: {"loc": v["loc"].copy(), "rot": v["rot"].copy(), "scale": v["scale"].copy()} for k, v in p.items()}

def smooth(t):
    t = max(0.0, min(1.0, t))
    return t * t * (3 - 2 * t)

def keyed(keys, f):
    """keys: [(frame, value)] を smoothstep でつなぐ(値は float か tuple)"""
    for i in range(len(keys) - 1):
        f0, v0 = keys[i]; f1, v1 = keys[i + 1]
        if f0 <= f <= f1:
            u = smooth((f - f0) / max(f1 - f0, 1e-6))
            if isinstance(v0, (tuple, list)):
                return tuple(a + (b - a) * u for a, b in zip(v0, v1))
            return v0 + (v1 - v0) * u
    return keys[-1][1]

SMASH = L.fc_map(bpy.data.actions["smash"])

# 元の歩きは golem_clean_orig.blend(作り直す前のコピー)から読む(何度流しても同じ結果になるように)
ORIG = os.path.join(os.path.dirname(bpy.data.filepath), "golem_clean_orig.blend")
for a in [a for a in bpy.data.actions if a.name.startswith("walk_src")]:
    bpy.data.actions.remove(a)
with bpy.data.libraries.load(ORIG, link=False) as (src_data, dst_data):
    dst_data.actions = ["walk"]
WALK_ACT = dst_data.actions[0]
WALK_ACT.name = "walk_src"
WALK = L.fc_map(WALK_ACT)
P0 = L.sample(SMASH, 0.0)          # 構え(smash の始めと終わりと同じ姿勢)

H0 = {n: head(P0, n) for n in ("IKhand.R", "IKhand.L", "heel.R", "heel.L", "hips")}
log["P0"] = {k: [round(x, 3) for x in v] for k, v in H0.items()}

made = {}

# ---------- 1. SmashQuick：smash の振りかぶりを縮める ----------
def warp(m, a, b, n, power=1.0):
    return [L.sample(m, a + (b - a) * ((i / max(n - 1, 1)) ** power)) for i in range(n)]
made["SmashQuick"] = (warp(SMASH, 0, 55, 30, 0.8)[:-1] + warp(SMASH, 55, 68, 13)[:-1]
                      + warp(SMASH, 68, 90, 10)[:-1] + warp(SMASH, 90, 120, 28))

# ---------- 2. 横なぎ(右 / 左) ----------
def swipe(side):
    s = 1 if side == "R" else -1           # 右腕は +X 側
    hand, other = "IKhand." + side, "IKhand." + ("L" if side == "R" else "R")
    h0, o0 = H0[hand], H0[other]
    hk = [(0, tuple(h0)),
          (18, (2.9 * s, -1.3, 3.4)),          # 外・後ろへ振りかぶる
          (25, (1.6 * s, 2.4, 2.6)),           # 前を横切る
          (31, (-0.9 * s, 2.6, 2.3)),          # 反対側まで振り抜く
          (40, (-1.1 * s, 2.3, 2.3)),          # 止め
          (66, tuple(h0))]
    ok = [(0, tuple(o0)), (18, (o0.x + 0.2 * s, o0.y + 0.6, o0.z - 0.1)), (31, (o0.x + 0.3 * s, o0.y - 0.7, o0.z)), (66, tuple(o0))]
    tw = [(0, 0.0), (18, -22.0 * s), (25, 0.0), (31, 28.0 * s), (40, 30.0 * s), (66, 0.0)]     # 腰のひねり(度)
    dp = [(0, 0.0), (18, 0.05), (28, -0.22), (40, -0.2), (66, 0.0)]                           # 腰の沈み
    frames = []
    for f in range(67):
        p = copy_pose(P0)
        move(p, "hips", (0, 0, keyed(dp, f)))
        rotate(p, "hips", (0, 0, 1), keyed(tw, f))
        rotate(p, "spine1", (0, 0, 1), keyed(tw, f) * 0.5)
        move_to(p, hand, keyed(hk, f))
        move_to(p, other, keyed(ok, f))
        frames.append(p)
    return frames
made["SwipeR"] = swipe("R")
made["SwipeL"] = swipe("L")

# ---------- 3. 踏みつけ(右足) ----------
def stomp():
    f0 = H0["heel.R"]
    fk = [(0, tuple(f0)), (20, (f0.x + 0.15, f0.y + 0.5, f0.z + 1.1)), (30, (f0.x + 0.15, f0.y + 0.6, f0.z + 1.2)),
          (35, (f0.x + 0.1, f0.y + 0.6, f0.z)), (48, (f0.x + 0.1, f0.y + 0.6, f0.z)), (58, (f0.x + 0.05, f0.y + 0.3, f0.z + 0.25)),
          (70, tuple(f0))]
    hp = [(0, (0, 0, 0)), (20, (-0.3, -0.1, 0.15)), (30, (-0.3, -0.15, 0.2)), (35, (0.0, 0.15, -0.4)), (48, (0.0, 0.15, -0.35)), (70, (0, 0, 0))]
    lean = [(0, 0.0), (20, -10.0), (30, -12.0), (35, 14.0), (48, 12.0), (70, 0.0)]       # 前後の傾き(度)
    arm = [(0, (0, 0, 0)), (20, (0.4, -0.3, 0.9)), (30, (0.5, -0.3, 1.1)), (35, (0.5, 0.6, -0.6)), (48, (0.5, 0.5, -0.5)), (70, (0, 0, 0))]
    frames = []
    for f in range(71):
        p = copy_pose(P0)
        move(p, "hips", keyed(hp, f))
        rotate(p, "spine1", (1, 0, 0), keyed(lean, f))
        a = keyed(arm, f)
        move_to(p, "IKhand.R", (H0["IKhand.R"].x + a[0], H0["IKhand.R"].y + a[1], H0["IKhand.R"].z + a[2]))
        move_to(p, "IKhand.L", (H0["IKhand.L"].x - a[0], H0["IKhand.L"].y + a[1], H0["IKhand.L"].z + a[2]))
        move_to(p, "heel.R", keyed(fk, f))
        frames.append(p)
    return frames
made["Stomp"] = stomp()

# ---------- 4. 歩き：分かりやすく(上書き) ----------
def walk_new():
    n = int(WALK_ACT.frame_range[1])      # 100
    src = [L.sample(WALK, float(f)) for f in range(n + 1)]
    feet = {k: [head(p, k) for p in src] for k in ("heel.L", "heel.R")}
    toes = {k: [head(p, k.replace("heel", "foot")) for p in src] for k in feet}     # 足先の高さ(かかとの回転で上がる分)
    tz = {k: min(v.z for v in toes[k]) for k in toes}
    hands = {k: [head(p, k) for p in src] for k in ("IKhand.L", "IKhand.R")}
    cy = {k: (max(v.y for v in feet[k]) + min(v.y for v in feet[k])) / 2 for k in feet}
    gz = {k: min(v.z for v in feet[k]) for k in feet}
    hy = {k: (max(v.y for v in hands[k]) + min(v.y for v in hands[k])) / 2 for k in hands}
    frames = []
    for f in range(n + 1):
        t = f / n
        p = copy_pose(src[f])
        for k in feet:
            v = feet[k][f]
            ny = cy[k] + (v.y - cy[k]) * 1.6                 # 歩幅 1.6倍
            nz = gz[k] + (v.z - gz[k]) * 1.6 + (toes[k][f].z - tz[k]) * 1.6   # 足を高く上げる
            move_to(p, k, (v.x, ny, nz))
        # 腰：着地(t=0, 0.5)で沈み、すれ違い(t=0.25, 0.75)で上がる。立っている足の側へ寄る
        bob = -0.07 - 0.07 * math.cos(4 * math.pi * t)
        sway = 0.12 * math.sin(2 * math.pi * t)
        move(p, "hips", (sway, 0, bob))
        rotate(p, "hips", (0, 0, 1), -7.0 * math.cos(2 * math.pi * t))      # 前に出た足の側の腰が前へ
        rotate(p, "hips", (0, 1, 0), 4.0 * math.sin(2 * math.pi * t))       # 立っている足の側へ少し傾く
        rotate(p, "spine1", (0, 0, 1), 9.0 * math.cos(2 * math.pi * t))     # 肩は逆にひねる
        rotate(p, "spine1", (1, 0, 0), 6.0)                                 # 少し前かがみ(重い感じ)
        for k in hands:
            v = hands[k][f]
            ny = hy[k] + (v.y - hy[k]) * 3.0                 # 腕を大きく振る
            nz = v.z + 0.25 * abs(ny - hy[k])                # 前後に振ったとき少し上がる
            move_to(p, k, (v.x, ny, nz))
        frames.append(p)
    return frames
made["walk"] = walk_new()

# ---------- 書き込み ----------
for name, frames in made.items():
    L.write_action(name, frames)
bpy.data.actions.remove(WALK_ACT)       # 元の歩きの読み込み用は消す(書き出さない)

# 手・足・腰の動きを記録(タイミングの確認用)
track = {}
for name in made:
    act = bpy.data.actions[name]
    tr = []
    for f in range(0, int(act.frame_range[1]) + 1):
        pos = L.bone_positions(act, f, ["hand.R", "hand.L", "foot.R", "foot.L", "hips", "head"])
        tr.append({k: [round(x, 3) for x in v] for k, v in pos.items()})
    track[name] = tr
log["track"] = track
log["lengths"] = {k: len(v) for k, v in made.items()}

ad = ARM.animation_data
ad.action = bpy.data.actions["walk"]
try: ad.action_slot = ad.action.slots[0]
except Exception: pass
bpy.context.scene.frame_set(0)
bpy.ops.wm.save_mainfile()
os.makedirs(S + r"\check", exist_ok=True)
bpy.ops.export_scene.gltf(filepath=S + r"\check\golem_check.gltf", export_format='GLTF_SEPARATE')
json.dump(log, open(S + r"\golem_make_out.json", "w", encoding="utf-8"), ensure_ascii=False)
print("GOLEM MAKE DONE", log["lengths"])
