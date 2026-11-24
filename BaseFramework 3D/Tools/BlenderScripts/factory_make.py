# 工場マップ用の敵(ロボット)を3体作る:作り・骨・アニメ・保存・書き出し
#   ScrapBot  … ゴブリン枠(小さくてすばしっこい)
#   PressBot  … ゴーレム枠(重くて腕が太い)
#   Titan     … ジャイアント枠(ボス)
# 今開いているファイルは汚さない(一時シーンで作って、別の .blend に書き出してから消す)
import bpy, bmesh, math, json, os
from mathutils import Vector, Quaternion, Matrix, Euler

ROOT = r"C:\GitHub\3DGAME\BaseFramework 3D"
TEX = ROOT + r"\Asset\Models\Terrains\Map\Factory"
BLEND_DIR = ROOT + r"\Asset\Blender\Enemy\Factory"
MODEL_DIR = ROOT + r"\Asset\Models\Character\Enemy\Factory"
OUT = ROOT + r"\Tools\BlenderScripts\factory_make_out.json"
FPS = 30
log = {}

if bpy.context.object and bpy.context.object.mode != 'OBJECT':
    bpy.ops.object.mode_set(mode='OBJECT')

# 前に途中で止まったときの残りを片付ける
_w = bpy.context.window
if _w.scene.name.startswith("Tmp_"):
    _w.scene = next(sc for sc in bpy.data.scenes if not sc.name.startswith("Tmp_"))
_names = ("ScrapBot", "PressBot", "Titan")
for o in [o for o in bpy.data.objects if o.name.startswith(_names)]: bpy.data.objects.remove(o)
for sc in [sc for sc in bpy.data.scenes if sc.name.startswith("Tmp_")]: bpy.data.scenes.remove(sc)
for m in [m for m in bpy.data.materials if m.name.startswith(_names)]: bpy.data.materials.remove(m)
for a in [a for a in bpy.data.armatures if a.name.startswith(_names)]: bpy.data.armatures.remove(a)
for me in [me for me in bpy.data.meshes if me.name.startswith(_names)]: bpy.data.meshes.remove(me)
for a in [a for a in bpy.data.actions if a.name.startswith(tuple(n + "_" for n in _names))]: bpy.data.actions.remove(a)

# ======================= マテリアル =======================
def img(path):
    return bpy.data.images.load(path, check_existing=True)

NTEX = BLEND_DIR + r"\Textures"   # ノーマルマップ(工場マップのものを名前を分けてコピー + 色だけの部品用の metal_n.png)

def make_mat(name, color=(0.5, 0.5, 0.5), tex=None, metal=0.6, rough=0.45, emit=None, normal=None, nstrength=1.0):
    m = bpy.data.materials.get(name)
    if m: return m
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    nt = m.node_tree
    bsdf = next(n for n in nt.nodes if n.type == 'BSDF_PRINCIPLED')
    bsdf.inputs["Base Color"].default_value = (*color, 1.0)
    bsdf.inputs["Metallic"].default_value = metal
    bsdf.inputs["Roughness"].default_value = rough
    if tex:
        t = nt.nodes.new("ShaderNodeTexImage")
        t.image = img(tex)
        nt.links.new(t.outputs["Color"], bsdf.inputs["Base Color"])
    if normal:
        nt_img = nt.nodes.new("ShaderNodeTexImage")
        nt_img.image = img(normal)
        nt_img.image.colorspace_settings.name = 'Non-Color'
        nm = nt.nodes.new("ShaderNodeNormalMap")
        nm.inputs["Strength"].default_value = nstrength
        nt.links.new(nt_img.outputs["Color"], nm.inputs["Color"])
        nt.links.new(nm.outputs["Normal"], bsdf.inputs["Normal"])
    if emit:
        bsdf.inputs["Emission Color"].default_value = (*emit, 1.0)
        bsdf.inputs["Emission Strength"].default_value = 4.0
    return m

def materials(prefix):
    return [
        make_mat(prefix + "_Plate", tex=TEX + r"\Wall\stone_c.png", normal=NTEX + r"\stone_c_n.png"),                    # 0 鉄板(リベット)
        make_mat(prefix + "_Tread", tex=TEX + r"\Floor\stone_2_c.png", metal=0.7, normal=NTEX + r"\stone_2_c_n.png"),     # 1 縞鋼板
        make_mat(prefix + "_Hazard", color=(0.95, 0.62, 0.05), metal=0.3, rough=0.5, normal=NTEX + r"\metal_n.png", nstrength=0.6),  # 2 黄色
        make_mat(prefix + "_Dark", color=(0.07, 0.07, 0.08), metal=0.8, rough=0.35, normal=NTEX + r"\metal_n.png", nstrength=0.8),   # 3 関節
        make_mat(prefix + "_Glow", color=(0.25, 0.95, 1.0), metal=0.0, rough=0.3, emit=(0.25, 0.95, 1.0)),                # 4 目(凹凸なし)
        make_mat(prefix + "_Rust", tex=TEX + r"\Stairs\stone_4_c.png", metal=0.5, normal=NTEX + r"\stone_4_c_n.png"),     # 5 波板
    ]
PLATE, TREAD, HAZ, DARK, GLOW, RUST = range(6)

# ======================= 形(固い部品を骨に付ける) =======================
class Builder:
    def __init__(self):
        self.bm = bmesh.new()
        self.uv = self.bm.loops.layers.uv.new("UVMap")
        self.dl = self.bm.verts.layers.deform.verify()
        self.groups = []

    def gi(self, bone):
        if bone not in self.groups: self.groups.append(bone)
        return self.groups.index(bone)

    def _add(self, geom_verts, bone, mat):
        g = self.gi(bone)
        faces = set()
        for v in geom_verts:
            v[self.dl][g] = 1.0
            for f in v.link_faces: faces.add(f)
        for f in faces: f.material_index = mat

    def box(self, bone, center, size, mat, rot=(0, 0, 0), bevel=0.0):
        M = Matrix.LocRotScale(Vector(center), Euler([math.radians(a) for a in rot]).to_quaternion(), Vector(size))
        r = bmesh.ops.create_cube(self.bm, size=1.0, matrix=M, calc_uvs=True)
        self._add(r["verts"], bone, mat)

    def seg(self, bone, a, b, w, d, mat, pad=0.0):
        """a→b に沿った箱(太さ w × 奥行き d)"""
        a = Vector(a); b = Vector(b)
        axis = (b - a)
        L = axis.length + pad * 2
        q = axis.normalized().to_track_quat('Z', 'Y')
        M = Matrix.LocRotScale((a + b) / 2, q, Vector((w, d, L)))
        r = bmesh.ops.create_cube(self.bm, size=1.0, matrix=M, calc_uvs=True)
        self._add(r["verts"], bone, mat)

    def cyl(self, bone, center, radius, depth, mat, axis='X', seg=10):
        rot = {'X': Euler((0, math.radians(90), 0)), 'Y': Euler((math.radians(90), 0, 0)), 'Z': Euler((0, 0, 0))}[axis]
        M = Matrix.LocRotScale(Vector(center), rot.to_quaternion(), Vector((1, 1, 1)))
        r = bmesh.ops.create_cone(self.bm, cap_ends=True, segments=seg, radius1=radius, radius2=radius, depth=depth, matrix=M, calc_uvs=True)
        self._add(r["verts"], bone, mat)

    def sphere(self, bone, center, radius, mat):
        M = Matrix.Translation(Vector(center))
        r = bmesh.ops.create_uvsphere(self.bm, u_segments=10, v_segments=6, radius=radius, matrix=M, calc_uvs=True)
        self._add(r["verts"], bone, mat)

    def finish(self, name, mats, arm, scale=1.0):
        me = bpy.data.meshes.new(name + "_Mesh")
        bmesh.ops.scale(self.bm, vec=Vector((scale, scale, scale)), verts=self.bm.verts)
        self.bm.normal_update()
        self.bm.to_mesh(me)
        self.bm.free()
        for m in mats: me.materials.append(m)
        ob = bpy.data.objects.new(name + "_Mesh", me)
        for g in self.groups: ob.vertex_groups.new(name=g)
        ob.parent = arm
        mod = ob.modifiers.new("Armature", 'ARMATURE')
        mod.object = arm
        for p in me.polygons: p.use_smooth = False
        return ob

# ======================= 骨 =======================
BONES = [  # 名前, 親, 根元の関節, 先の関節
    ("root", None, "ground", "groundF"),
    ("hips", "root", "hips", "spine"),
    ("spine", "hips", "spine", "chest"),
    ("chest", "spine", "chest", "neck"),
    ("head", "chest", "neck", "headtop"),
    ("upperarm.R", "chest", "shoulder.R", "elbow.R"), ("forearm.R", "upperarm.R", "elbow.R", "wrist.R"), ("hand.R", "forearm.R", "wrist.R", "tip.R"),
    ("upperarm.L", "chest", "shoulder.L", "elbow.L"), ("forearm.L", "upperarm.L", "elbow.L", "wrist.L"), ("hand.L", "forearm.L", "wrist.L", "tip.L"),
    ("thigh.R", "hips", "hip.R", "knee.R"), ("shin.R", "thigh.R", "knee.R", "ankle.R"), ("foot.R", "shin.R", "ankle.R", "toe.R"),
    ("thigh.L", "hips", "hip.L", "knee.L"), ("shin.L", "thigh.L", "knee.L", "ankle.L"), ("foot.L", "shin.L", "ankle.L", "toe.L"),
]

def mirror_joints(J):
    out = dict(J)
    for k, v in J.items():
        if k.endswith(".R"):
            out[k[:-2] + ".L"] = (-v[0], v[1], v[2])
    return out

def make_armature(name, J):
    ad = bpy.data.armatures.new(name + "_Rig")
    arm = bpy.data.objects.new(name + "_Armature", ad)
    bpy.context.scene.collection.objects.link(arm)
    bpy.context.view_layer.objects.active = arm
    for o in bpy.context.view_layer.objects: o.select_set(False)
    arm.select_set(True)
    bpy.ops.object.mode_set(mode='EDIT')
    for n, p, a, b in BONES:
        eb = ad.edit_bones.new(n)
        eb.head = Vector(J[a]); eb.tail = Vector(J[b])
        eb.roll = 0.0
        if p: eb.parent = ad.edit_bones[p]
        eb.use_connect = False
    bpy.ops.object.mode_set(mode='OBJECT')
    for pb in arm.pose.bones: pb.rotation_mode = 'QUATERNION'
    return arm

# ======================= ポーズとアニメ =======================
def mirror_name(n):
    return n[:-2] + ".L" if n.endswith(".R") else (n[:-2] + ".R" if n.endswith(".L") else n)

def P(base=None, **kw):
    """ポーズ:{骨: (rx, ry, rz)} と 'off':(x,y,z)(腰の移動)、'rootoff'、'rootrot'
       名前の末尾 _R は右、_L は左、_S は左右対称(左は Y と Z を反転)"""
    p = {k: tuple(v) for k, v in (base or {}).items()}
    for k, v in kw.items():
        if k.endswith("_S"):
            n = k[:-2].replace("_", ".") if False else k[:-2]
            p[n + ".R"] = tuple(v)
            p[n + ".L"] = (v[0], -v[1], -v[2])
        elif k.endswith("_R") or k.endswith("_L"):
            p[k[:-2] + "." + k[-1]] = tuple(v)
        else:
            p[k] = tuple(v)
    return p

def mirror_pose(p):
    out = {}
    for k, v in p.items():
        if k in ("off", "rootoff"):
            out[k] = (-v[0], v[1], v[2])
        elif k == "rootrot":
            out[k] = (v[0], -v[1], -v[2])
        else:
            out[mirror_name(k)] = (v[0], -v[1], -v[2])
    return out

def smooth(t):
    t = max(0.0, min(1.0, t)); return t * t * (3 - 2 * t)

def interp(keys, f):
    """keys: [(frame, pose)] → その frame のポーズ"""
    for i in range(len(keys) - 1):
        f0, p0 = keys[i]; f1, p1 = keys[i + 1]
        if f0 <= f <= f1:
            u = smooth((f - f0) / max(f1 - f0, 1e-6))
            out = {}
            for k in set(p0) | set(p1):
                a = p0.get(k, (0, 0, 0)); b = p1.get(k, (0, 0, 0))
                out[k] = tuple(x + (y - x) * u for x, y in zip(a, b))
            return out
    return keys[-1][1]

def world_q(r):
    rx, ry, rz = [math.radians(a) for a in r]
    return (Quaternion((0, 0, 1), rz) @ Quaternion((0, 1, 0), ry) @ Quaternion((1, 0, 0), rx))

def write_action(arm, prefix, name, keys, length, H):
    """keys を毎フレーム焼いて Action にする(回転は骨の休みの向きに対する世界軸の回転)
       Action の名前は「ロボット名_アニメ名」(ジャイアントのアクションと名前がぶつからないように)。
       書き出すアニメの名前は NLA トラックの名前(アニメ名)になる"""
    old = bpy.data.actions.get(prefix + "_" + name)
    if old: bpy.data.actions.remove(old)
    act = bpy.data.actions.new(prefix + "_" + name)
    act.use_fake_user = True
    slot = act.slots.new(id_type='OBJECT', name=arm.name)
    ad = arm.animation_data or arm.animation_data_create()
    ad.action = act; ad.action_slot = slot
    n = length + 1
    frames = [interp(keys, f) for f in range(n)]
    for pb in arm.pose.bones:
        b = pb.name
        R = pb.bone.matrix_local.to_3x3()
        Rq = R.to_quaternion()
        qs, ls = [], []
        for fr in frames:
            q = Rq.inverted() @ world_q(fr.get(b, (0, 0, 0))) @ Rq
            if qs and qs[-1].dot(q) < 0: q.negate()
            qs.append(q)
            off = (0, 0, 0)
            if b == "hips": off = fr.get("off", (0, 0, 0))
            if b == "root": off = fr.get("rootoff", (0, 0, 0))
            ls.append(R.inverted() @ (Vector(off) * H))
        if b == "root":
            qs = []
            for fr in frames:
                q = Rq.inverted() @ world_q(fr.get("rootrot", (0, 0, 0))) @ Rq
                if qs and qs[-1].dot(q) < 0: q.negate()
                qs.append(q)
        def put(prop, idx, vals):
            fc = act.fcurve_ensure_for_datablock(arm, 'pose.bones["%s"].%s' % (b, prop), index=idx, group_name=b)
            fc.keyframe_points.add(n)
            co = []
            for f, v in enumerate(vals): co += [float(f), float(v)]
            fc.keyframe_points.foreach_set("co", co)
            for kp in fc.keyframe_points: kp.interpolation = 'LINEAR'
            fc.update()
        for i in range(4): put("rotation_quaternion", i, [q[i] for q in qs])
        for i in range(3): put("location", i, [l[i] for l in ls])
    act.frame_range = (0, length); act.use_frame_range = True
    tr = ad.nla_tracks.new(); tr.name = name
    tr.strips.new(prefix + "_" + name, 0, act)
    ad.action = None
    return act

# ---------- 基本の姿勢(少し前かがみ・肘と膝を軽く曲げる) ----------
def stand(hunch=0.0, arm_out=10.0):
    return P(off=(0, 0, -0.015), spine=(-hunch * 0.5, 0, 0), chest=(-hunch * 0.5, 0, 0), head=(hunch * 0.7, 0, 0),
             upperarm_S=(5, -arm_out, 0), forearm_S=(18, 0, 0), hand_S=(0, 0, 0),
             thigh_S=(10, 0, 0), shin_S=(-20, 0, 0), foot_S=(10, 0, 0))

def idle(B, length=60, amp=1.0):
    k = []
    for i, t in enumerate((0, length // 2, length)):
        s = 1 if i == 1 else 0
        k.append((t, P(B, off=(0, 0, -0.015 - 0.012 * s * amp), chest=(B["chest"][0] + 2.5 * s * amp, 0, 0),
                       head=(B["head"][0] - 3 * s * amp, 4 * s * amp, 0), upperarm_S=(B["upperarm.R"][0] + 3 * s, B["upperarm.R"][1] - 2 * s, 0))))
    return k, length

def walk(B, length=36, stride=26, arm=22, bob=0.02):
    h = length // 2
    A = P(B, off=(0, 0, -0.01), hips=(0, 0, 5), chest=(B["chest"][0], 0, -6),
          thigh_R=(10 + stride, 0, 0), shin_R=(-8, 0, 0), foot_R=(-10, 0, 0),
          thigh_L=(10 - stride, 0, 0), shin_L=(-25, 0, 0), foot_L=(25, 0, 0),
          upperarm_R=(5 - arm, -10, 0), upperarm_L=(5 + arm, 10, 0))
    Pm = P(B, off=(0, 0, -0.01 - bob), thigh_R=(10, 0, 0), shin_R=(-15, 0, 0), thigh_L=(30, 0, 0), shin_L=(-70, 0, 0), foot_L=(20, 0, 0))
    Bp = mirror_pose(A)
    Pm2 = mirror_pose(Pm)
    return [(0, A), (h // 2, Pm), (h, Bp), (h + h // 2, Pm2), (length, A)], length

def run(B, length=22, stride=45, arm=45, lean=14):
    h = length // 2
    A = P(B, off=(0, 0.0, -0.03), spine=(-lean * 0.5, 0, 0), chest=(-lean * 0.5, 0, -8), head=(lean * 0.6, 0, 0),
          thigh_R=(10 + stride, 0, 0), shin_R=(-15, 0, 0), foot_R=(-5, 0, 0),
          thigh_L=(-stride * 0.6, 0, 0), shin_L=(-60, 0, 0), foot_L=(30, 0, 0),
          upperarm_R=(-arm, -12, 0), forearm_R=(75, 0, 0), upperarm_L=(arm, 12, 0), forearm_L=(75, 0, 0))
    Up = P(A, off=(0, 0, 0.03), thigh_R=(20, 0, 0), shin_R=(-30, 0, 0), thigh_L=(60, 0, 0), shin_L=(-110, 0, 0))
    return [(0, A), (h // 2, Up), (h, mirror_pose(A)), (h + h // 2, mirror_pose(Up)), (length, A)], length

def die(B, length=50):
    k = [(0, B),
         (8, P(B, chest=(20, 0, 10), head=(25, 0, 0), upperarm_S=(-30, -40, 0), off=(0, -0.03, 0))),
         (18, P(B, off=(0, 0, -0.25), thigh_S=(60, 0, 0), shin_S=(-100, 0, 0), foot_S=(40, 0, 0), chest=(15, 0, 15), upperarm_S=(-20, -50, 0))),
         (34, P(B, rootrot=(86, 0, 8), rootoff=(0, -0.05, 0.02), off=(0, 0, -0.02), chest=(5, 0, 10), head=(-10, 0, 25),
                upperarm_R=(-10, -80, 0), upperarm_L=(20, 70, 0), forearm_S=(10, 0, 0), thigh_S=(15, 0, 0), shin_S=(-10, 0, 0), foot_S=(5, 0, 0))),
         (38, P(B, rootrot=(80, 0, 8), rootoff=(0, -0.05, 0.03), off=(0, 0, -0.02), chest=(5, 0, 10), head=(-10, 0, 25),
                upperarm_R=(-10, -85, 0), upperarm_L=(20, 75, 0), forearm_S=(10, 0, 0), thigh_S=(18, 0, 0), shin_S=(-12, 0, 0))),
         (length, P(B, rootrot=(88, 0, 8), rootoff=(0, -0.05, 0.02), off=(0, 0, -0.02), chest=(5, 0, 10), head=(-10, 0, 30),
                    upperarm_R=(-10, -85, 0), upperarm_L=(20, 75, 0), forearm_S=(10, 0, 0), thigh_S=(15, 0, 0), shin_S=(-10, 0, 0)))]
    return k, length

def die_robot(B, t=1.0):
    """ロボットの停止:ショックでビクッ → 首と腕がガクガク痙攣 → 電源が落ちて首と腕がだらんと垂れる
       → 膝から崩れて膝立ち → 前に倒れて一度跳ねる → 最後にピクッと動いて止まる
       関節が急にパキッと動くように、1〜2フレームで切り替わるキーを多めに入れている"""
    F = lambda f: int(round(f * t))
    limp = dict(head=(40, 0, 18), chest=(-12, 0, 0), spine=(-6, 0, 0),
                upperarm_R=(-4, -6, 0), upperarm_L=(-4, 6, 0), forearm_S=(6, 0, 0), hand_S=(-10, 0, 0))
    kneel = dict(off=(0, 0.0, -0.3), thigh_S=(75, 0, 0), shin_S=(-125, 0, 0), foot_S=(55, 0, 0))
    lie = dict(rootrot=(-84, 0, 6), rootoff=(0, 0.03, 0.11),   # 倒れた体が床に埋まらない高さ
               off=(0, 0, -0.05),
               thigh_S=(15, 0, 0), shin_S=(-25, 0, 0), foot_S=(-10, 0, 0),
               upperarm_R=(-10, -35, 0), upperarm_L=(30, 40, 0), forearm_S=(20, 0, 0), chest=(-6, 0, 0))
    k = [(0, B),
         # ショック(のけぞってビクッ)
         (F(3), P(B, chest=(10, 0, 0), head=(-15, 0, 22), off=(0, -0.01, 0.01), upperarm_R=(10, -40, 0), upperarm_L=(10, 40, 0), forearm_S=(40, 0, 0))),
         (F(6), P(B, chest=(10, 0, 0), head=(-15, 0, 22), off=(0, -0.01, 0.01), upperarm_R=(10, -40, 0), upperarm_L=(10, 40, 0), forearm_S=(40, 0, 0))),
         # 痙攣(首と腕が1フレームでパキッと動く)
         (F(7), P(B, chest=(6, 0, -6), head=(5, 0, -28), upperarm_R=(45, -20, 0), upperarm_L=(0, 30, 0), forearm_R=(80, 0, 0))),
         (F(10), P(B, chest=(6, 0, -6), head=(5, 0, -28), upperarm_R=(45, -20, 0), upperarm_L=(0, 30, 0), forearm_R=(80, 0, 0))),
         (F(11), P(B, chest=(4, 0, 8), head=(-8, 0, 30), upperarm_R=(10, -25, 0), upperarm_L=(40, 20, 0), forearm_L=(85, 0, 0))),
         (F(13), P(B, chest=(4, 0, 8), head=(-8, 0, 30), upperarm_R=(10, -25, 0), upperarm_L=(40, 20, 0), forearm_L=(85, 0, 0))),
         (F(14), P(B, chest=(2, 0, -3), head=(0, 0, -12), upperarm_R=(25, -15, 0), forearm_R=(50, 0, 0))),
         # 電源が落ちる(首と腕がだらん、少し沈む)
         (F(22), P(B, **limp)),
         (F(26), P(B, **limp, off=(0, 0, -0.04))),
         # 膝からガクッと崩れる
         (F(30), P(B, **{**limp, **kneel})),
         (F(32), P(B, **{**limp, **kneel, "off": (0, 0, -0.27)})),
         (F(36), P(B, **{**limp, **kneel})),
         # 膝立ちのまま腕がピクッ
         (F(40), P(B, **{**limp, **kneel})),
         (F(41), P(B, **{**limp, **kneel, "upperarm_R": (30, -15, 0), "forearm_R": (60, 0, 0)})),
         (F(43), P(B, **{**limp, **kneel})),
         # 前に倒れる → 一度跳ねる → 止まる
         (F(54), P(B, **lie)),
         (F(57), P(B, **{**lie, "rootrot": (-78, 0, 6)})),
         (F(61), P(B, **{**lie, "rootrot": (-87, 0, 6), "head": (10, 0, 35)})),
         # 最後のピクッ
         (F(70), P(B, **{**lie, "rootrot": (-87, 0, 6), "head": (10, 0, 35)})),
         (F(71), P(B, **{**lie, "rootrot": (-87, 0, 6), "head": (10, 0, 35), "upperarm_L": (45, 40, 0), "hand_L": (25, 0, 0)})),
         (F(73), P(B, **{**lie, "rootrot": (-87, 0, 6), "head": (10, 0, 35)})),
         (F(80), P(B, **{**lie, "rootrot": (-87, 0, 6), "head": (12, 0, 36)}))]
    return k, F(80)

# ---------- 攻撃(右手版。左は mirror) ----------
def swipe_R(B, t=1.0, wind=22, hold=6, strike=7, rec=24):
    """右腕の横なぎ。t は速さの倍率(大きいほど遅い)"""
    f1 = int(wind * t); f2 = f1 + int(hold * t); f3 = f2 + int(strike * t); f4 = f3 + int(6 * t); f5 = f4 + int(rec * t)
    W = P(B, off=(0.02, -0.02, -0.04), chest=(B["chest"][0], 0, -35), spine=(B["spine"][0], 0, -10), head=(B["head"][0], 0, 25),
          upperarm_R=(0, -95, -55), forearm_R=(25, 0, 0), hand_R=(0, 0, -20), upperarm_L=(30, 20, 30), forearm_L=(60, 0, 0),
          thigh_R=(5, 0, 0), thigh_L=(20, 0, 0), shin_L=(-35, 0, 0))
    W2 = P(W, upperarm_R=(0, -100, -62), chest=(B["chest"][0], 0, -40))
    S = P(B, off=(-0.02, 0.05, -0.07), chest=(B["chest"][0] - 10, 0, 40), spine=(B["spine"][0], 0, 15), head=(B["head"][0], 0, -15),
          upperarm_R=(0, -95, 125), forearm_R=(10, 0, 0), hand_R=(0, 0, 15), upperarm_L=(-20, 20, -10), forearm_L=(50, 0, 0),
          thigh_R=(30, 0, 0), shin_R=(-30, 0, 0), thigh_L=(-5, 0, 0))
    S2 = P(S, upperarm_R=(-5, -90, 135))
    keys = [(0, B), (f1, W), (f2, W2), (f3, S), (f4, S2), (f5, B)]
    return keys, f5, (f2 + int(strike * t * 0.35), f3 + int(3 * t))

def slam(B, t=1.0, wind=24, hold=8, strike=6, rec=30, crouch=0.12):
    f1 = int(wind * t); f2 = f1 + int(hold * t); f3 = f2 + int(strike * t); f4 = f3 + int(10 * t); f5 = f4 + int(rec * t)
    W = P(B, off=(0, -0.03, 0.02), spine=(8, 0, 0), chest=(12, 0, 0), head=(-10, 0, 0),
          upperarm_S=(175, -12, 0), forearm_S=(35, 0, 0), hand_S=(20, 0, 0), thigh_S=(5, 0, 0), shin_S=(-10, 0, 0))
    W2 = P(W, spine=(10, 0, 0), chest=(15, 0, 0), upperarm_S=(182, -14, 0))
    S = P(B, off=(0, 0.06, -crouch), spine=(-25, 0, 0), chest=(-25, 0, 0), head=(30, 0, 0),
          upperarm_S=(55, -10, 0), forearm_S=(5, 0, 0), hand_S=(-10, 0, 0), thigh_S=(45, 0, 0), shin_S=(-70, 0, 0), foot_S=(25, 0, 0))
    keys = [(0, B), (f1, W), (f2, W2), (f3, S), (f4, S), (f5, B)]
    return keys, f5, (f2 + int(strike * t * 0.5), f3 + int(3 * t))

def punch_R(B, t=1.0, wind=14, hold=5, strike=5, rec=20):
    f1 = int(wind * t); f2 = f1 + int(hold * t); f3 = f2 + int(strike * t); f4 = f3 + int(6 * t); f5 = f4 + int(rec * t)
    W = P(B, off=(0.02, -0.05, -0.04), chest=(B["chest"][0], 0, -30), head=(B["head"][0], 0, 20),
          upperarm_R=(-40, -25, 0), forearm_R=(120, 0, 0), upperarm_L=(50, 15, 0), forearm_L=(90, 0, 0),
          thigh_L=(30, 0, 0), shin_L=(-40, 0, 0), thigh_R=(-5, 0, 0))
    S = P(B, off=(-0.01, 0.08, -0.06), chest=(B["chest"][0] - 8, 0, 30), head=(B["head"][0], 0, -20),
          upperarm_R=(88, -5, 15), forearm_R=(5, 0, 0), upperarm_L=(-10, 20, 0), forearm_L=(80, 0, 0),
          thigh_R=(40, 0, 0), shin_R=(-35, 0, 0), thigh_L=(-15, 0, 0), shin_L=(-10, 0, 0))
    keys = [(0, B), (f1, W), (f2, W), (f3, S), (f4, S), (f5, B)]
    return keys, f5, (f2 + int(strike * t * 0.4), f3 + int(3 * t))

def stomp_R(B, t=1.0, wind=20, hold=6, strike=4, rec=24):
    f1 = int(wind * t); f2 = f1 + int(hold * t); f3 = f2 + int(strike * t); f4 = f3 + int(10 * t); f5 = f4 + int(rec * t)
    W = P(B, off=(-0.06, 0, 0.02), spine=(5, -8, 0), chest=(5, -5, 0), upperarm_S=(20, -70, 0), forearm_S=(40, 0, 0),
          thigh_R=(95, 0, 0), shin_R=(-100, 0, 0), foot_R=(10, 0, 0), thigh_L=(8, 0, 0), shin_L=(-12, 0, 0))
    W2 = P(W, thigh_R=(102, 0, 0), upperarm_S=(25, -75, 0))
    S = P(B, off=(0, 0.04, -0.09), spine=(-15, 0, 0), chest=(-12, 0, 0), upperarm_S=(10, -45, 0), forearm_S=(30, 0, 0),
          thigh_R=(30, 0, 0), shin_R=(-30, 0, 0), foot_R=(0, 0, 0), thigh_L=(25, 0, 0), shin_L=(-45, 0, 0), foot_L=(20, 0, 0))
    keys = [(0, B), (f1, W), (f2, W2), (f3, S), (f4, S), (f5, B)]
    return keys, f5, (f2 + int(strike * t * 0.5), f3 + int(4 * t))

def spin(B, t=1.0, wind=18, hold=6, turn=16, rec=22):
    f1 = int(wind * t); f2 = f1 + int(hold * t); f3 = f2 + int(turn * t); f4 = f3 + int(5 * t); f5 = f4 + int(rec * t)
    O = P(B, off=(0, 0, -0.05), upperarm_S=(0, -88, 0), forearm_S=(5, 0, 0), hand_S=(0, 0, 0), thigh_S=(20, 0, 0), shin_S=(-35, 0, 0), foot_S=(15, 0, 0))
    W = P(O, rootrot=(0, 0, 50), chest=(B["chest"][0], 0, 15))
    W2 = P(O, rootrot=(0, 0, 55), chest=(B["chest"][0], 0, 18))
    S = P(O, rootrot=(0, 0, -360), chest=(B["chest"][0], 0, -10))
    S2 = P(O, rootrot=(0, 0, -365), chest=(B["chest"][0], 0, -5))
    E = P(B, rootrot=(0, 0, -360))
    keys = [(0, B), (f1, W), (f2, W2), (f3, S), (f4, S2), (f5, E)]
    return keys, f5, (f2 + int(turn * t * 0.15), f3)

def leap(B, t=1.0, crouch=14, air=16, land=4, rec=26, height=0.55):
    f1 = int(crouch * t); f2 = f1 + int(air * 0.5 * t); f3 = f1 + int(air * t); f4 = f3 + int(land * t); f5 = f4 + int(8 * t); f6 = f5 + int(rec * t)
    C = P(B, off=(0, -0.03, -0.18), spine=(-20, 0, 0), chest=(-15, 0, 0), head=(25, 0, 0), upperarm_S=(-55, -15, 0), forearm_S=(30, 0, 0),
          thigh_S=(70, 0, 0), shin_S=(-110, 0, 0), foot_S=(40, 0, 0))
    A = P(B, rootoff=(0, 0, height), spine=(10, 0, 0), chest=(12, 0, 0), head=(-10, 0, 0), upperarm_S=(170, -15, 0), forearm_S=(30, 0, 0),
          thigh_S=(50, 0, 0), shin_S=(-90, 0, 0), foot_S=(10, 0, 0))
    D = P(B, rootoff=(0, 0, height * 0.15), off=(0, 0.04, 0.0), spine=(-10, 0, 0), chest=(-10, 0, 0), upperarm_S=(140, -12, 0), forearm_S=(20, 0, 0),
          thigh_S=(30, 0, 0), shin_S=(-40, 0, 0), foot_S=(10, 0, 0))
    S = P(B, off=(0, 0.06, -0.2), spine=(-30, 0, 0), chest=(-25, 0, 0), head=(30, 0, 0), upperarm_S=(55, -10, 0), forearm_S=(5, 0, 0),
          thigh_S=(75, 0, 0), shin_S=(-110, 0, 0), foot_S=(35, 0, 0))
    keys = [(0, B), (f1, C), (f2, A), (f3, D), (f4, S), (f5, S), (f6, B)]
    return keys, f6, (f3, f4 + int(3 * t))

def roar(B, length=70):
    C = P(B, off=(0, 0, -0.06), spine=(-15, 0, 0), chest=(-15, 0, 0), head=(20, 0, 0), upperarm_S=(25, -15, 0), forearm_S=(70, 0, 0), thigh_S=(25, 0, 0), shin_S=(-40, 0, 0), foot_S=(15, 0, 0))
    R = P(B, off=(0, -0.02, 0.01), spine=(10, 0, 0), chest=(15, 0, 0), head=(-25, 0, 0), upperarm_S=(40, -70, 0), forearm_S=(30, 0, 0), hand_S=(-20, 0, 0))
    R2 = P(R, head=(-28, 0, 4), chest=(16, 0, 2))
    R3 = P(R, head=(-26, 0, -4), chest=(15, 0, -2))
    return [(0, B), (14, C), (24, R), (40, R2), (54, R3), (length, B)], length, None

def mirrored(fn):
    def g(*a, **kw):
        keys, L, hit = fn(*a, **kw)
        return [(f, mirror_pose(p)) for f, p in keys], L, hit
    return g
swipe_L = mirrored(swipe_R); punch_L = mirrored(punch_R)

# ======================= ロボットの形 =======================
def connect(bd, J, waist, pelvis, hip_r, neck_r, sh_r, elbow_r, knee_r, ankle_r):
    """胴・首・関節のすき間を埋める部品(腰と胸をつなぐ胴、骨盤、首、肩・肘・股・膝・足首の玉)
       waist  ... (幅, 奥行き) 腰から胸までの胴
       pelvis ... (幅, 奥行き, 高さ) 両足の付け根を覆う骨盤"""
    s = lambda k: Vector(J[k])
    hips, spine, chest = s("hips"), s("spine"), s("chest")
    # 腰 → 胸の胴(骨ごとに2つ。少し伸ばして上下の部品に食い込ませる)
    bd.seg("hips", hips, spine, waist[0], waist[1], DARK, pad=(spine - hips).length * 0.25)
    bd.seg("spine", spine, chest, waist[0] * 0.9, waist[1] * 0.9, DARK, pad=(chest - spine).length * 0.35)
    # 骨盤:両足の付け根まで覆う
    hz = s("hip.R").z
    bd.box("hips", (0, hips.y, hz), (pelvis[0], pelvis[1], pelvis[2]), TREAD)
    # 首:頭の骨に付けて、下は胸の中まで入れる
    n = s("neck"); top = s("headtop")
    bd.seg("head", n - (top - n) * 0.45, n + (top - n) * 0.35, neck_r * 2, neck_r * 2, DARK)
    for side in ("R", "L"):
        bd.sphere("upperarm." + side, s("shoulder." + side), sh_r, DARK)
        bd.sphere("forearm." + side, s("elbow." + side), elbow_r, DARK)
        bd.sphere("hand." + side, s("wrist." + side), elbow_r * 0.85, DARK)
        bd.sphere("thigh." + side, s("hip." + side), hip_r, DARK)
        bd.sphere("shin." + side, s("knee." + side), knee_r, DARK)
        bd.sphere("foot." + side, s("ankle." + side), ankle_r, DARK)

def J_scrap():
    J = {"ground": (0, 0, 0), "groundF": (0, 0.15, 0),
         "hips": (0, 0, 0.58), "spine": (0, 0.01, 0.72), "chest": (0, 0.03, 0.86), "neck": (0, 0.08, 1.02), "headtop": (0, 0.1, 1.22),
         "shoulder.R": (0.2, 0.04, 0.94), "elbow.R": (0.27, 0.03, 0.7), "wrist.R": (0.3, 0.05, 0.46), "tip.R": (0.31, 0.07, 0.33),
         "hip.R": (0.1, 0, 0.56), "knee.R": (0.12, 0.05, 0.31), "ankle.R": (0.12, -0.02, 0.07), "toe.R": (0.12, 0.13, 0.02)}
    return mirror_joints(J)

def body_scrap(bd, J):
    s = lambda k: Vector(J[k])
    # 胴:細いフレームと背中の黄色いタンク
    bd.seg("hips", s("hips") + Vector((0, 0, -0.04)), s("spine"), 0.26, 0.17, DARK)
    bd.seg("spine", s("spine"), s("chest"), 0.12, 0.1, DARK)
    bd.box("chest", (0, 0.04, 0.93), (0.34, 0.22, 0.18), PLATE)
    bd.cyl("chest", (0, -0.1, 0.92), 0.06, 0.2, HAZ, axis='Z')
    # 頭:横長のバイザーと一つ目
    bd.box("head", (0, 0.1, 1.1), (0.2, 0.18, 0.13), PLATE)
    bd.box("head", (0, 0.19, 1.1), (0.16, 0.02, 0.04), DARK)
    bd.sphere("head", (0.0, 0.2, 1.1), 0.028, GLOW)
    bd.seg("head", (0.07, 0.07, 1.16), (0.11, 0.03, 1.27), 0.015, 0.015, DARK)  # アンテナ
    for side in ("R", "L"):
        sx = 1 if side == "R" else -1
        bd.sphere("upperarm." + side, s("shoulder." + side), 0.055, HAZ)
        bd.seg("upperarm." + side, s("shoulder." + side), s("elbow." + side), 0.06, 0.06, PLATE)
        bd.sphere("forearm." + side, s("elbow." + side), 0.04, DARK)
        bd.seg("forearm." + side, s("elbow." + side), s("wrist." + side), 0.08, 0.07, PLATE)
        # 爪3本
        w = s("wrist." + side)
        for i, dx in enumerate((-0.03, 0.0, 0.03)):
            bd.seg("hand." + side, w + Vector((dx * sx, 0.0, 0)), w + Vector((dx * sx * 1.5, 0.05, -0.17)), 0.018, 0.018, HAZ if i == 1 else DARK)
        bd.seg("thigh." + side, s("hip." + side), s("knee." + side), 0.07, 0.07, PLATE)
        bd.sphere("shin." + side, s("knee." + side), 0.04, DARK)
        bd.seg("shin." + side, s("knee." + side), s("ankle." + side), 0.06, 0.06, DARK)
        a = s("ankle." + side)
        bd.box("foot." + side, (a.x, a.y + 0.04, 0.03), (0.08, 0.17, 0.06), TREAD)
    connect(bd, J, waist=(0.17, 0.12), pelvis=(0.32, 0.14, 0.1), hip_r=0.05, neck_r=0.035,
            sh_r=0.05, elbow_r=0.04, knee_r=0.045, ankle_r=0.035)

def J_press():
    J = {"ground": (0, 0, 0), "groundF": (0, 0.5, 0),
         "hips": (0, 0, 1.55), "spine": (0, 0.02, 2.2), "chest": (0, 0.1, 2.9), "neck": (0, 0.35, 3.75), "headtop": (0, 0.4, 4.4),
         "shoulder.R": (1.35, 0.15, 3.45), "elbow.R": (1.75, 0.15, 2.45), "wrist.R": (1.9, 0.3, 1.45), "tip.R": (1.95, 0.4, 0.85),
         "hip.R": (0.6, 0, 1.45), "knee.R": (0.7, 0.18, 0.85), "ankle.R": (0.7, 0, 0.25), "toe.R": (0.7, 0.55, 0.05)}
    return mirror_joints(J)

def body_press(bd, J):
    s = lambda k: Vector(J[k])
    bd.box("hips", (0, 0, 1.6), (1.2, 0.9, 0.5), TREAD)
    bd.seg("spine", s("spine"), s("chest"), 0.7, 0.6, DARK)
    # 胸:大きな箱と黄色い縞の帯
    bd.box("chest", (0, 0.15, 3.15), (2.1, 1.4, 1.3), PLATE)
    bd.box("chest", (0, 0.86, 2.75), (1.6, 0.05, 0.22), HAZ)
    bd.box("chest", (0, -0.65, 3.3), (1.2, 0.3, 0.9), RUST)     # 背中のエンジン
    for x in (-0.35, 0.35): bd.cyl("chest", (x, -0.65, 4.0), 0.12, 0.6, DARK, axis='Z')   # 煙突
    # 頭:胸に埋まった小さな頭と横長の目
    bd.box("head", (0, 0.55, 3.95), (0.75, 0.6, 0.5), PLATE)
    bd.box("head", (0, 0.86, 3.98), (0.55, 0.04, 0.1), GLOW)
    for side in ("R", "L"):
        sx = 1 if side == "R" else -1
        bd.box("upperarm." + side, s("shoulder." + side) + Vector((0.12 * sx, 0, 0.15)), (0.75, 0.8, 0.55), HAZ)   # 肩当て
        bd.seg("upperarm." + side, s("shoulder." + side), s("elbow." + side), 0.45, 0.45, DARK)
        bd.cyl("forearm." + side, s("elbow." + side), 0.28, 0.5, DARK, axis='X')
        bd.seg("forearm." + side, s("elbow." + side), s("wrist." + side), 0.7, 0.7, PLATE)
        # 拳:大きなプレス金具
        w = s("wrist." + side); t = s("tip." + side)
        bd.seg("hand." + side, w, t, 0.85, 0.8, TREAD, pad=0.05)
        bd.box("hand." + side, (t.x, t.y, t.z - 0.05), (0.95, 0.9, 0.12), HAZ)
        bd.seg("thigh." + side, s("hip." + side), s("knee." + side), 0.5, 0.5, DARK)
        bd.seg("shin." + side, s("knee." + side), s("ankle." + side), 0.65, 0.6, PLATE)
        a = s("ankle." + side)
        bd.box("foot." + side, (a.x, a.y + 0.15, 0.13), (0.7, 0.95, 0.26), TREAD)
    connect(bd, J, waist=(0.95, 0.75), pelvis=(1.75, 0.85, 0.55), hip_r=0.36, neck_r=0.22,
            sh_r=0.38, elbow_r=0.32, knee_r=0.36, ankle_r=0.3)

def J_titan():
    J = {"ground": (0, 0, 0), "groundF": (0, 0.6, 0),
         "hips": (0, 0, 2.8), "spine": (0, 0.0, 3.55), "chest": (0, 0.05, 4.35), "neck": (0, 0.12, 5.15), "headtop": (0, 0.15, 5.95),
         "shoulder.R": (1.3, 0.0, 4.85), "elbow.R": (1.65, 0.0, 3.7), "wrist.R": (1.8, 0.1, 2.6), "tip.R": (1.85, 0.15, 2.0),
         "hip.R": (0.6, 0, 2.7), "knee.R": (0.7, 0.12, 1.45), "ankle.R": (0.7, 0, 0.28), "toe.R": (0.7, 0.65, 0.05)}
    return mirror_joints(J)

def body_titan(bd, J):
    s = lambda k: Vector(J[k])
    bd.box("hips", (0, 0, 2.75), (1.5, 0.9, 0.6), DARK)
    bd.box("hips", (0, 0.47, 2.7), (1.0, 0.06, 0.4), HAZ)
    bd.seg("spine", s("spine"), s("chest"), 0.8, 0.6, DARK)
    bd.box("chest", (0, 0.05, 4.55), (2.2, 1.2, 1.3), PLATE)
    bd.box("chest", (0, 0.66, 4.6), (0.9, 0.06, 0.6), DARK)
    bd.cyl("chest", (0, 0.7, 4.6), 0.22, 0.08, GLOW, axis='Y', seg=12)    # 胸の炉心
    bd.box("chest", (0, -0.7, 4.7), (1.4, 0.4, 1.1), RUST)
    for x in (-0.45, 0.0, 0.45): bd.cyl("chest", (x, -0.75, 5.45), 0.1, 0.7, DARK, axis='Z')
    # 頭:角ばった兜と二つ目、頭頂の黄色い角
    bd.box("head", (0, 0.15, 5.55), (0.75, 0.7, 0.65), PLATE)
    bd.box("head", (0, 0.5, 5.5), (0.6, 0.05, 0.14), DARK)
    for x in (-0.15, 0.15): bd.box("head", (x, 0.53, 5.52), (0.14, 0.04, 0.08), GLOW)
    bd.seg("head", (0, 0.2, 5.85), (0, 0.45, 6.25), 0.12, 0.12, HAZ)
    for side in ("R", "L"):
        sx = 1 if side == "R" else -1
        sh = s("shoulder." + side)
        bd.box("upperarm." + side, sh + Vector((0.15 * sx, 0, 0.2)), (0.9, 0.9, 0.5), HAZ)
        bd.box("upperarm." + side, sh + Vector((0.2 * sx, 0, 0.48)), (0.7, 0.7, 0.08), DARK)
        bd.seg("upperarm." + side, sh, s("elbow." + side), 0.5, 0.5, PLATE)
        bd.cyl("forearm." + side, s("elbow." + side), 0.3, 0.55, DARK, axis='X')
        bd.seg("forearm." + side, s("elbow." + side), s("wrist." + side), 0.62, 0.62, PLATE)
        bd.box("forearm." + side, (s("elbow." + side) + s("wrist." + side)) / 2 + Vector((0.31 * sx, 0, 0)), (0.06, 0.4, 0.6), HAZ)
        bd.seg("hand." + side, s("wrist." + side), s("tip." + side), 0.7, 0.65, TREAD, pad=0.05)
        bd.seg("thigh." + side, s("hip." + side), s("knee." + side), 0.6, 0.6, PLATE)
        bd.cyl("shin." + side, s("knee." + side), 0.33, 0.6, DARK, axis='X')
        bd.seg("shin." + side, s("knee." + side), s("ankle." + side), 0.7, 0.7, PLATE)
        bd.box("shin." + side, (s("knee." + side) + s("ankle." + side)) / 2 + Vector((0, 0.36, 0)), (0.45, 0.06, 0.7), HAZ)
        a = s("ankle." + side)
        bd.box("foot." + side, (a.x, a.y + 0.2, 0.15), (0.85, 1.25, 0.3), TREAD)
    connect(bd, J, waist=(1.0, 0.7), pelvis=(1.85, 0.85, 0.6), hip_r=0.4, neck_r=0.25,
            sh_r=0.38, elbow_r=0.32, knee_r=0.36, ankle_r=0.32)

# ======================= 1体分 =======================
def make_robot(name, Jfn, bodyfn, H, anims, scale=1.0):
    """scale ... 全体の大きさの倍率(骨・形・アニメの移動量をまとめて大きくする)"""
    sc = bpy.data.scenes.new("Tmp_" + name)
    sc.render.fps = FPS
    win = bpy.context.window
    prev_scene = win.scene
    win.scene = sc
    J = Jfn()
    Js = {k: tuple(x * scale for x in v) for k, v in J.items()}
    arm = make_armature(name, Js)
    H = H * scale
    mats = materials(name)
    bd = Builder()
    bodyfn(bd, J)
    ob = bd.finish(name, mats, arm, scale)
    sc.collection.objects.link(ob)
    info = {"height": H, "actions": {}}
    for an, (keys, L, hit) in anims(arm).items():
        write_action(arm, name, an, keys, L, H)
        info["actions"][an] = {"frames": L, "sec": round(L / FPS, 3),
                               "hit": None if not hit else [round(hit[0] / L, 3), round(hit[1] / L, 3)]}
    arm.animation_data.action = None
    sc.frame_end = 60
    # 書き出し(NLA のトラックを 1 本ずつアニメにする)
    os.makedirs(MODEL_DIR + "\\" + name, exist_ok=True)
    os.makedirs(BLEND_DIR + "\\" + name, exist_ok=True)
    # glTF の書き出しは、書き出すアニメと同じ名前のアクションを消してしまうことがある
    # (開いているファイルのジャイアントの idle などが消えた)ので、書き出す間だけ他のアクションの名前を変えておく
    keep = [a for a in bpy.data.actions if not a.name.startswith(name + "_")]
    for a in keep: a.name = "__keep__" + a.name
    bpy.ops.export_scene.gltf(filepath=MODEL_DIR + "\\" + name + "\\" + name + ".gltf", export_format='GLTF_SEPARATE',
                              export_animation_mode='NLA_TRACKS', export_apply=False, use_active_scene=True)
    for a in keep: a.name = a.name[len("__keep__"):]
    # .blend に保存(このシーンと中身だけ)
    blocks = {sc, arm, ob, arm.data, ob.data} | set(mats) | {a for a in bpy.data.actions if a.name.startswith(name + "_")}
    bpy.data.libraries.write(BLEND_DIR + "\\" + name + "\\" + name + ".blend", blocks, fake_user=True)
    # 一時シーンを消す
    win.scene = prev_scene
    acts = [a for a in bpy.data.actions if a.name.startswith(name + "_")]
    bpy.data.objects.remove(ob); bpy.data.objects.remove(arm)
    for a in acts: bpy.data.actions.remove(a)
    bpy.data.scenes.remove(sc)
    return info

def anims_scrap(arm):
    B = stand(hunch=22, arm_out=12)
    return {"Idle": (*idle(B, 48, 1.4), None), "Walk": (*walk(B, 26, 30, 25, 0.03), None), "Run": (*run(B, 16, 50, 50, 22), None),
            "Die": (*die_robot(B, 0.85), None),
            "SlashR": swipe_R(B, 0.8), "SlashL": swipe_L(B, 0.8), "Slam": slam(B, 0.8, crouch=0.15),
            "Thrust": punch_R(B, 0.8), "Leap": leap(B, 0.9, height=0.7)}

def anims_press(arm):
    B = stand(hunch=6, arm_out=14)
    return {"idle": (*idle(B, 70, 0.8), None), "walk": (*walk(B, 44, 20, 14, 0.015), None), "run": (*run(B, 30, 32, 30, 10), None),
            "death": (*die_robot(B, 1.2), None),
            "smash": slam(B, 1.4, crouch=0.08), "SmashQuick": slam(B, 1.0, crouch=0.08),
            "SwipeR": swipe_R(B, 1.3), "SwipeL": swipe_L(B, 1.3), "Stomp": stomp_R(B, 1.3)}

def anims_titan(arm):
    B = stand(hunch=4, arm_out=12)
    return {"idle": (*idle(B, 90, 0.8), None), "Walk": (*walk(B, 48, 22, 18, 0.015), None), "Run": (*run(B, 34, 38, 34, 12), None),
            "Death": (*die_robot(B, 1.4), None),
            "LeftPunch": punch_L(B, 1.3), "RightPunch": punch_R(B, 1.3), "DoubleSlam": slam(B, 1.4, crouch=0.1),
            "Stomp": stomp_R(B, 1.4), "Spin": spin(B, 1.3), "JumpSlam": leap(B, 1.6, height=0.9), "Roar": roar(B, 70)}

# 大きさ:プレイヤー(Rogue)は約3.0。ゴブリンは約3.5、ゴーレムは約4.7
SCALE = {"ScrapBot": 2.9,   # 1.2 → 約3.5(ゴブリンと同じくらい)
         "PressBot": 1.25,  # 4.3 → 約5.4(ゴーレムより少し大きく)
         "Titan": 1.35}     # 6.2 → 約8.4(プレイヤーの3倍近いボス)
for name, jf, bf, H, an in (("ScrapBot", J_scrap, body_scrap, 1.25, anims_scrap),
                            ("PressBot", J_press, body_press, 4.4, anims_press),
                            ("Titan", J_titan, body_titan, 6.2, anims_titan)):
    log[name] = make_robot(name, jf, bf, H, an, SCALE[name])

json.dump(log, open(OUT, "w", encoding="utf-8"), ensure_ascii=False, indent=1)
print("FACTORY MAKE DONE")
