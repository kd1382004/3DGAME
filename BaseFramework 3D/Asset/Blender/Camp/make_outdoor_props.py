# キャンプ場(準備画面の背景・屋外版)の地形まわりのモデルを作るスクリプト
# 針葉樹・茂み・崖2種・岩2種・ダンジョンの入り口・地面
#
# 使い方: Blenderのスクリプトエディタで開いて実行(またはPythonコンソールで exec(open(パス).read()))
#
# ・開いているファイルの中身は消さない。小物ごとに一時的なシーンを作って、
#   そのシーンだけを .blend と .gltf で保存し、最後に一時的なものを全部消す。
# ・保存先 : Asset/Blender/Camp/<名前>/<名前>.blend と .gltf(.bin と画像も同じ場所に出る)
# ・テクスチャ : Asset/Textures/Camp/*.png を読み込む
# ・大きさの目安 : プレイヤーの身長が約3.0。原点は足元の真ん中、正面は -Y 方向
# ・失敗したら Asset/Blender/Camp/make_camp_props_error.log に内容を書く

import bpy, bmesh, math, os, random, traceback
from mathutils import Vector, Matrix

ROOT = r"C:\GitHub\3DGAME\BaseFramework 3D\Asset"
OUT_ROOT = os.path.join(ROOT, "Blender", "Camp")
TEX_DIR = os.path.join(ROOT, "Textures", "Camp")
LOG = os.path.join(OUT_ROOT, "make_outdoor_props_error.log")

# 作ったデータ(最後に消す)
_created = {"objects": [], "meshes": [], "materials": [], "images": [], "scenes": []}


# ------------------------------------------------------------
# マテリアル
# ------------------------------------------------------------
_mat_cache = {}

def get_mat(name, tex=None, color=(0.8, 0.8, 0.8), rough=0.85, metal=0.0):
    """テクスチャ(またはただの色)のマテリアル。同じ名前は使い回す"""
    if name in _mat_cache:
        return _mat_cache[name]

    mat = bpy.data.materials.new("Camp_" + name)
    _created["materials"].append(mat)
    mat.use_nodes = True
    nodes = mat.node_tree.nodes
    links = mat.node_tree.links

    bsdf = next((n for n in nodes if n.type == "BSDF_PRINCIPLED"), None)
    if bsdf is None:
        bsdf = nodes.new("ShaderNodeBsdfPrincipled")
        out = next((n for n in nodes if n.type == "OUTPUT_MATERIAL"), None) or nodes.new("ShaderNodeOutputMaterial")
        links.new(bsdf.outputs[0], out.inputs[0])

    bsdf.inputs["Roughness"].default_value = rough
    bsdf.inputs["Metallic"].default_value = metal

    if tex:
        img = bpy.data.images.load(os.path.join(TEX_DIR, tex), check_existing=True)
        _created["images"].append(img)
        node = nodes.new("ShaderNodeTexImage")
        node.image = img
        node.location = (-400, 200)
        links.new(node.outputs["Color"], bsdf.inputs["Base Color"])

        # ノーマルマップ(<名前>_Normal.png があればつなぐ。OpenGL形式)
        nrm_path = os.path.join(TEX_DIR, tex.replace(".png", "_Normal.png"))
        if os.path.exists(nrm_path):
            nimg = bpy.data.images.load(nrm_path, check_existing=True)
            nimg.colorspace_settings.name = "Non-Color"
            _created["images"].append(nimg)
            ntex = nodes.new("ShaderNodeTexImage")
            ntex.image = nimg
            ntex.location = (-700, -200)
            nmap = nodes.new("ShaderNodeNormalMap")
            nmap.location = (-400, -200)
            links.new(ntex.outputs["Color"], nmap.inputs["Color"])
            links.new(nmap.outputs["Normal"], bsdf.inputs["Normal"])
    else:
        bsdf.inputs["Base Color"].default_value = (color[0], color[1], color[2], 1.0)

    _mat_cache[name] = mat
    return mat


# ------------------------------------------------------------
# 形を足していく道具(1つのメッシュに、部品をマテリアル付きで足す)
# ------------------------------------------------------------
class Builder:
    def __init__(self):
        self.bm = bmesh.new()
        self.uv = self.bm.loops.layers.uv.verify()
        self.mats = []

    def _mat_index(self, mat):
        if mat not in self.mats:
            self.mats.append(mat)
        return self.mats.index(mat)

    def _finish(self, faces, mat, matrix, uv_mode, uv_scale, uv_offset=(0.0, 0.0)):
        """新しく足した面に、UV・マテリアル・位置を設定する"""
        idx = self._mat_index(mat)
        verts = set()
        for f in faces:
            f.material_index = idx
            for l in f.loops:
                co = l.vert.co
                n = f.normal
                if uv_mode == "cyl" and abs(n.z) < 0.7:
                    a = math.atan2(co.y, co.x)
                    l[self.uv].uv = (a / (2 * math.pi) * uv_scale[0], co.z * uv_scale[1])
                elif uv_mode == "cap":
                    # 丸い面(切り口)は、面の中に画像1枚が収まるように
                    l[self.uv].uv = (co.x * uv_scale[0] + 0.5, co.y * uv_scale[1] + 0.5)
                else:
                    # 箱型の投影(面の向きで軸を選ぶ)
                    ax, ay, az = abs(n.x), abs(n.y), abs(n.z)
                    if az >= ax and az >= ay:
                        u, v = co.x, co.y
                    elif ax >= ay:
                        u, v = co.y, co.z
                    else:
                        u, v = co.x, co.z
                    l[self.uv].uv = (u * uv_scale[0] + uv_offset[0], v * uv_scale[1] + uv_offset[1])
                verts.add(l.vert)
        bmesh.ops.transform(self.bm, matrix=matrix, verts=list(verts))

    def _new_faces(self, before):
        self.bm.normal_update()
        return [f for f in self.bm.faces if f not in before]

    def box(self, size, mat, loc=(0, 0, 0), rot=(0, 0, 0), uv_scale=(0.5, 0.5), uv_offset=(0.0, 0.0)):
        """size=(x,y,z) の箱。loc は箱の真ん中"""
        before = set(self.bm.faces)
        ret = bmesh.ops.create_cube(self.bm, size=1.0)
        bmesh.ops.scale(self.bm, vec=Vector(size), verts=ret["verts"])
        faces = self._new_faces(before)
        m = Matrix.Translation(loc) @ euler(rot)
        self._finish(faces, mat, m, "box", uv_scale, uv_offset)

    def cyl(self, radius, depth, mat, loc=(0, 0, 0), rot=(0, 0, 0), seg=16, radius2=None,
            cap_mat=None, uv_scale=(1.0, 0.5), cap_uv=None):
        """Z軸向きの円柱(radius2 で上の半径を変えると円すい台)。loc は真ん中"""
        r2 = radius if radius2 is None else radius2
        before = set(self.bm.faces)
        bmesh.ops.create_cone(self.bm, cap_ends=True, cap_tris=False, segments=seg,
                              radius1=radius, radius2=r2, depth=depth)
        faces = self._new_faces(before)
        side = [f for f in faces if abs(f.normal.z) < 0.7]
        caps = [f for f in faces if abs(f.normal.z) >= 0.7]
        m = Matrix.Translation(loc) @ euler(rot)
        # UVを先に決めてから動かす
        idx_side = self._mat_index(mat)
        for f in side:
            f.material_index = idx_side
            for l in f.loops:
                co = l.vert.co
                a = math.atan2(co.y, co.x)
                l[self.uv].uv = (a / (2 * math.pi) * uv_scale[0] * max(radius, r2) * 2 * math.pi, co.z * uv_scale[1])
        cm = cap_mat or mat
        idx_cap = self._mat_index(cm)
        rr = max(radius, r2)
        for f in caps:
            f.material_index = idx_cap
            for l in f.loops:
                co = l.vert.co
                if cap_uv == "fit":
                    l[self.uv].uv = (co.x / (2 * rr) + 0.5, co.y / (2 * rr) + 0.5)
                else:
                    l[self.uv].uv = (co.x * uv_scale[0], co.y * uv_scale[0])
        verts = set(v for f in faces for v in f.verts)
        bmesh.ops.transform(self.bm, matrix=m, verts=list(verts))

    def rock(self, radius, mat, loc, scale=(1, 1, 1), seed=0):
        """でこぼこした石"""
        rnd = random.Random(seed)
        before = set(self.bm.faces)
        ret = bmesh.ops.create_icosphere(self.bm, subdivisions=2, radius=radius)
        for v in ret["verts"]:
            v.co *= 0.8 + 0.35 * rnd.random()
            v.co.x *= scale[0]; v.co.y *= scale[1]; v.co.z *= scale[2]
            if v.co.z < -radius * 0.25 * scale[2]:
                v.co.z = -radius * 0.25 * scale[2]   # 下は平らにして地面に置けるように
        faces = self._new_faces(before)
        self._finish(faces, mat, Matrix.Translation(loc) @ euler((0, 0, rnd.random() * 6.28)), "box", (1.2, 1.2))

    def prism(self, pts_xz, y, thick, mat, uv_scale=(0.4, 0.4)):
        """XZ面の多角形を、Y方向に厚み thick だけ伸ばした板(テントの前後の三角など)"""
        before = set(self.bm.faces)
        front = [self.bm.verts.new((x, y - thick / 2, z)) for x, z in pts_xz]
        back = [self.bm.verts.new((x, y + thick / 2, z)) for x, z in pts_xz]
        self.bm.faces.new(list(reversed(front)))
        self.bm.faces.new(back)
        n = len(pts_xz)
        for i in range(n):
            j = (i + 1) % n
            self.bm.faces.new((front[i], front[j], back[j], back[i]))
        bmesh.ops.recalc_face_normals(self.bm, faces=self._new_faces(before))
        faces = self._new_faces(before)
        self._finish(faces, mat, Matrix.Identity(4), "box", uv_scale)

    def lathe(self, profile, mat, seg=20, uv_scale=(0.5, 0.6), cap_mat=None):
        """(半径, 高さ) の並びを回して作る形(樽など)。上下にふたを付ける"""
        before = set(self.bm.faces)
        rings = []
        for r, z in profile:
            ring = []
            for i in range(seg):
                a = 2 * math.pi * i / seg
                ring.append(self.bm.verts.new((r * math.cos(a), r * math.sin(a), z)))
            rings.append(ring)
        for k in range(len(rings) - 1):
            for i in range(seg):
                j = (i + 1) % seg
                self.bm.faces.new((rings[k][i], rings[k][j], rings[k + 1][j], rings[k + 1][i]))
        side = self._new_faces(before)
        idx = self._mat_index(mat)
        maxr = max(r for r, z in profile)
        for f in side:
            f.material_index = idx
            for l in f.loops:
                co = l.vert.co
                a = math.atan2(co.y, co.x)
                l[self.uv].uv = (a / (2 * math.pi) * 2 * math.pi * maxr * uv_scale[0], co.z * uv_scale[1])
        before2 = set(self.bm.faces)
        self.bm.faces.new(list(reversed(rings[0])))
        self.bm.faces.new(rings[-1])
        caps = self._new_faces(before2)
        cidx = self._mat_index(cap_mat or mat)
        for f in caps:
            f.material_index = cidx
            for l in f.loops:
                co = l.vert.co
                l[self.uv].uv = (co.x / (2 * maxr) + 0.5, co.y / (2 * maxr) + 0.5)
        bmesh.ops.recalc_face_normals(self.bm, faces=side + caps)

    def build(self, name, scene, smooth_angle=None):
        me = bpy.data.meshes.new(name)
        _created["meshes"].append(me)
        bmesh.ops.recalc_face_normals(self.bm, faces=self.bm.faces)
        self.bm.to_mesh(me)
        self.bm.free()
        for m in self.mats:
            me.materials.append(m)
        obj = bpy.data.objects.new(name, me)
        _created["objects"].append(obj)
        scene.collection.objects.link(obj)
        if smooth_angle is not None:
            for p in me.polygons:
                p.use_smooth = True
            try:
                me.set_sharp_from_angle(angle=math.radians(smooth_angle))
            except Exception:
                pass
        return obj


def euler(rot_deg):
    rx, ry, rz = (math.radians(a) for a in rot_deg)
    return (Matrix.Rotation(rz, 4, "Z") @ Matrix.Rotation(ry, 4, "Y") @ Matrix.Rotation(rx, 4, "X"))



# ------------------------------------------------------------
# 形の道具(屋外用に追加)
# ------------------------------------------------------------
def _noise(p, seed):
    """なめらかなでこぼこ(いくつかのsin波を重ねる)"""
    x, y, z = p
    s = seed * 1.37
    return (math.sin(x * 0.9 + s) * math.cos(z * 0.7 + s * 0.5) * 0.5
            + math.sin(y * 1.7 + z * 1.3 + s * 2.1) * 0.3
            + math.sin(x * 3.1 - z * 2.3 + s * 3.7) * 0.2)


def lumpy_box(b, size, mat, loc=(0, 0, 0), cuts=6, amp=0.4, seed=0, uv_scale=(0.2, 0.2), keep_bottom=True):
    """細かく分けた箱を、でこぼこに崩した形(崖や岩の塊)。loc は底の真ん中"""
    bm = b.bm
    before = set(bm.faces)
    ret = bmesh.ops.create_cube(bm, size=1.0)
    verts = ret["verts"]
    edges = list(set(e for v in verts for e in v.link_edges))
    bmesh.ops.subdivide_edges(bm, edges=edges, cuts=cuts, use_grid_fill=True)
    new_faces = [f for f in bm.faces if f not in before]
    vs = list(set(v for f in new_faces for v in f.verts))
    bmesh.ops.scale(bm, vec=Vector(size), verts=vs)
    bmesh.ops.translate(bm, vec=Vector((0, 0, size[2] / 2)), verts=vs)
    for v in vs:
        c = v.co.copy()
        d = Vector((c.x / max(size[0], 1e-3), c.y / max(size[1], 1e-3), (c.z - size[2] / 2) / max(size[2], 1e-3)))
        if d.length < 1e-4:
            continue
        n = _noise((c.x * 0.6, c.y * 0.6, c.z * 0.6), seed)
        off = d.normalized() * n * amp
        if keep_bottom and c.z < 0.05:
            off.z = 0.0
        v.co = c + off
    bm.normal_update()
    faces = [f for f in bm.faces if f not in before]
    b._finish(faces, mat, Matrix.Translation(loc), "box", uv_scale)


def star_cone(b, mat, r, h, z0, seg=14, jag=0.3, droop=0.35, seed=0):
    """針葉樹の葉の一段。ふちがギザギザで、先が少し垂れた円すい"""
    import random
    rnd = random.Random(seed)
    bm = b.bm
    before = set(bm.faces)
    ring = []
    for i in range(seg * 2):
        a = math.pi * i / seg + rnd.uniform(-0.05, 0.05)
        outer = (i % 2 == 0)
        rr = r * (1.0 if outer else 1.0 - jag) * rnd.uniform(0.92, 1.05)
        zz = z0 - (droop if outer else 0.0) * rnd.uniform(0.7, 1.1)
        ring.append(bm.verts.new((rr * math.cos(a), rr * math.sin(a), zz)))
    top = bm.verts.new((0, 0, z0 + h))
    under = bm.verts.new((0, 0, z0 + h * 0.2))
    n = len(ring)
    for i in range(n):
        j = (i + 1) % n
        bm.faces.new((ring[i], ring[j], top))
        bm.faces.new((ring[j], ring[i], under))
    faces = [f for f in bm.faces if f not in before]
    bmesh.ops.recalc_face_normals(bm, faces=faces)
    idx = b._mat_index(mat)
    for f in faces:
        f.material_index = idx
        for l in f.loops:
            co = l.vert.co
            a = math.atan2(co.y, co.x)
            l[b.uv].uv = (a / (2 * math.pi) * r * 2.0, (co.z - z0) * 0.6)


def blob(b, mat, radius, loc, scale=(1, 1, 1), seed=0, amp=0.3, sub=2, flat_bottom=True, uv_scale=(0.5, 0.5)):
    """丸いかたまり(岩・茂み)"""
    bm = b.bm
    before = set(bm.faces)
    ret = bmesh.ops.create_icosphere(bm, subdivisions=sub, radius=radius)
    for v in ret["verts"]:
        c = v.co.copy()
        n = _noise((c.x * 2.0, c.y * 2.0, c.z * 2.0), seed)
        c = c * (1.0 + n * amp)
        c = Vector((c.x * scale[0], c.y * scale[1], c.z * scale[2]))
        if flat_bottom and c.z < -radius * 0.2 * scale[2]:
            c.z = -radius * 0.2 * scale[2]
        v.co = c
    bm.normal_update()
    faces = [f for f in bm.faces if f not in before]
    lift = radius * 0.2 * scale[2] if flat_bottom else 0.0
    b._finish(faces, mat, Matrix.Translation((loc[0], loc[1], loc[2] + lift)), "box", uv_scale)


# ------------------------------------------------------------
# モデル
# ------------------------------------------------------------
def make_pine_tree(b):
    """針葉樹。高さ8くらい。幹 + ギザギザの葉が4段"""
    bark = get_mat("Bark", "Bark.png", rough=0.95)
    needle = get_mat("PineNeedle", "PineNeedle.png", rough=0.9)
    b.lathe([(0.32, 0.0), (0.26, 1.0), (0.2, 3.5), (0.12, 6.5), (0.05, 7.6)], bark, seg=10, uv_scale=(0.8, 0.5))
    for i, (r, h, z0) in enumerate([(2.3, 2.6, 1.4), (1.9, 2.3, 2.9), (1.45, 2.0, 4.3), (0.95, 1.9, 5.6)]):
        star_cone(b, needle, r, h, z0, seg=14, seed=i)


def make_bush(b):
    """茂み。幅1.6・高さ1.0くらい"""
    needle = get_mat("PineNeedle", "PineNeedle.png", rough=0.9)
    for i, (x, y, r) in enumerate([(0, 0, 0.6), (0.55, 0.15, 0.45), (-0.5, -0.1, 0.5), (0.1, -0.45, 0.42), (-0.15, 0.45, 0.4)]):
        blob(b, needle, r, (x, y, 0), scale=(1.0, 1.0, 0.85), seed=i + 10, amp=0.35, sub=2, uv_scale=(1.2, 1.2))


def make_boulder_a(b):
    """大きな岩(横長)。幅3・高さ1.8くらい"""
    rock = get_mat("CliffRock", "CliffRock.png", rough=0.95)
    blob(b, rock, 1.2, (0, 0, 0), scale=(1.4, 1.0, 0.95), seed=21, amp=0.3, sub=3, uv_scale=(0.35, 0.35))
    blob(b, rock, 0.6, (1.3, 0.4, 0), scale=(1.0, 1.0, 0.9), seed=22, amp=0.3, sub=2, uv_scale=(0.35, 0.35))


def make_boulder_b(b):
    """大きな岩(縦長)。幅2・高さ2.6くらい"""
    rock = get_mat("CliffRock", "CliffRock.png", rough=0.95)
    blob(b, rock, 1.0, (0, 0, 0), scale=(1.0, 0.9, 1.5), seed=31, amp=0.32, sub=3, uv_scale=(0.35, 0.35))
    blob(b, rock, 0.5, (-0.9, -0.5, 0), scale=(1.0, 1.0, 0.8), seed=32, amp=0.3, sub=2, uv_scale=(0.35, 0.35))


def make_cliff_a(b):
    """崖のかたまり(横に並べて使う)。幅10・高さ14・奥行き5。正面は -Y"""
    rock = get_mat("CliffRock", "CliffRock.png", rough=0.95)
    lumpy_box(b, (10, 5, 14), rock, cuts=8, amp=1.0, seed=41, uv_scale=(0.12, 0.12))
    lumpy_box(b, (6, 3, 5), rock, loc=(-1.5, -2.6, 0), cuts=4, amp=0.7, seed=42, uv_scale=(0.12, 0.12))


def make_cliff_b(b):
    """崖のかたまり(もう一つの形)。幅8・高さ16・奥行き5"""
    rock = get_mat("CliffRock", "CliffRock.png", rough=0.95)
    lumpy_box(b, (8, 5, 16), rock, cuts=8, amp=1.0, seed=51, uv_scale=(0.12, 0.12))
    lumpy_box(b, (4, 3, 7), rock, loc=(2.0, -2.4, 0), cuts=4, amp=0.6, seed=52, uv_scale=(0.12, 0.12))


def get_dark_mat(name, tex, dark, rough=0.9):
    """テクスチャに暗さ(0〜1)を掛けたマテリアル。glTF では baseColorFactor として書き出される"""
    key = "%s_%.3f" % (name, dark)
    if key in _mat_cache:
        return _mat_cache[key]

    mat = get_mat(name, tex, rough=rough).copy()
    mat.name = "Camp_%s_Dark%02d" % (name, int(round(dark * 100)))
    _created["materials"].append(mat)
    nodes = mat.node_tree.nodes
    links = mat.node_tree.links

    bsdf = next(n for n in nodes if n.type == "BSDF_PRINCIPLED")
    texnode = next(n for n in nodes if n.type == "TEX_IMAGE" and n.image and not n.image.name.endswith("_Normal.png")
                   and n.image.colorspace_settings.name != "Non-Color")

    # 画像の色 × 暗さ(Multiply)を Base Color につなぐ
    mix = nodes.new("ShaderNodeMix")
    mix.data_type = "RGBA"
    mix.blend_type = "MULTIPLY"
    mix.location = (-150, 250)
    mix.inputs[0].default_value = 1.0
    links.new(texnode.outputs["Color"], mix.inputs[6])
    mix.inputs[7].default_value = (dark, dark, dark, 1.0)
    links.new(mix.outputs[2], bsdf.inputs["Base Color"])

    _mat_cache[key] = mat
    return mat


def make_dungeon_gate(b):
    """ダンジョンの入り口。石の門(幅6.8・高さ5.9)と、奥へ続く通路(奥行き28)。
    原点は入り口の足元の真ん中。正面は -Y(通路は +Y へ続く)。
    ・通路は奥ほど暗くなる(2ごとに区切って、石の色に暗さを掛ける)
    ・入ってから12は平ら(地面の板が通路の中に出ないように)、そこから先は階段で下っていく
    ・天井も一緒に下がるので、手前からは突き当たりが見えない。突き当たりは真っ黒"""
    stone = get_mat("GateStone", "GateStone.png", rough=0.9)
    W, H = 5.0, 5.0                 # 通路の内側の幅・高さ
    T = 1.0                         # 壁の厚み
    SEG = 2.0                       # 区切りの長さ
    N = 14                          # 区切りの数(奥行き 28)
    FLAT = 6                        # 平らな区切りの数(12)
    DROP = 0.8                      # 下りの区切り1つで下がる高さ(段2つ分)
    Y0 = 0.8                        # 通路の始まり(柱の奥)

    # 門の柱とまぐさ(前と同じ)
    for s in (-1, 1):
        b.box((0.9, 1.6, 5.6), stone, loc=(s * (W / 2 + 0.45), 0, 2.8), uv_scale=(0.5, 0.5))
        b.box((1.2, 1.9, 0.4), stone, loc=(s * (W / 2 + 0.45), 0, 0.2), uv_scale=(0.5, 0.5))     # 柱の台
    b.box((W + 1.8, 1.6, 0.9), stone, loc=(0, 0, 5.45), uv_scale=(0.5, 0.5))
    b.box((1.2, 1.7, 1.1), stone, loc=(0, -0.05, 5.5), uv_scale=(0.5, 0.5))                       # 要石

    # 入口の手前(柱の間)の床と天井のすき間をふさぐ
    b.box((W, Y0 + 0.8, 0.2), stone, loc=(0, (Y0 - 0.8) / 2, 0.03 - 0.1), uv_scale=(0.25, 0.25))

    # 今の区切りの始まりの床の高さ
    # 地面の板(上の面が高さ0)と同じ高さだとちらつくので、少しだけ上げる
    floor_z = 0.03
    for i in range(N):
        y0 = Y0 + i * SEG
        cy = y0 + SEG / 2

        # 奥ほど暗く(入口 1.0 → 区切り9 で 0)。少しだけ残して真っ黒の手前で止める
        dark = max(0.0, 1.0 - i / 9.0) ** 1.6
        mat = stone if i == 0 else get_dark_mat("GateStone", "GateStone.png", round(dark, 3))

        ceil_z = floor_z + H         # この区切りの天井の高さ(床と一緒に下がる)

        # 床
        if i < FLAT:
            b.box((W, SEG, 0.2), mat, loc=(0, cy, floor_z - 0.1), uv_scale=(0.25, 0.25))
            low = floor_z
        else:
            # 段を2つ(1段 0.4 下がる)。段の前の面が見えるように厚めの箱にする
            for k in range(2):
                top = floor_z - DROP / 2 * (k + 1)
                b.box((W, SEG / 2, 1.0), mat, loc=(0, y0 + SEG / 4 + k * SEG / 2, top - 0.5), uv_scale=(0.25, 0.25))
            low = floor_z - DROP

        # 左右の壁(床の一番低い所から天井の上まで)
        wall_h = (ceil_z + T) - (low - 1.0)
        for s in (-1, 1):
            b.box((T, SEG, wall_h), mat, loc=(s * (W / 2 + T / 2), cy, (low - 1.0) + wall_h / 2), uv_scale=(0.25, 0.25))

        # 天井
        b.box((W + 2 * T, SEG, T), mat, loc=(0, cy, ceil_z + T / 2), uv_scale=(0.25, 0.25))

        if i >= FLAT:
            floor_z -= DROP

    # 突き当たり(真っ黒)
    black = get_mat("GateVoid", None, color=(0.0, 0.0, 0.0), rough=1.0)
    end_y = Y0 + N * SEG
    b.box((W + 2 * T, T, H + 3.0), black, loc=(0, end_y + T / 2, floor_z - 1.5 + (H + 3.0) / 2), uv_scale=(0.25, 0.25))


def make_ground(b):
    """地面(土)。44×36の平らな板。原点は真ん中"""
    dirt = get_mat("Dirt", "Dirt.png", rough=1.0)
    b.box((44, 36, 0.2), dirt, loc=(0, 0, -0.1), uv_scale=(0.2, 0.2))


PROPS = [
    ("PineTree", make_pine_tree),
    ("Bush", make_bush),
    ("BoulderA", make_boulder_a),
    ("BoulderB", make_boulder_b),
    ("CliffA", make_cliff_a),
    ("CliffB", make_cliff_b),
    ("DungeonGate", make_dungeon_gate),
    ("GroundDirt", make_ground),
]

# ------------------------------------------------------------
# 保存
# ------------------------------------------------------------
def save_prop(name, scene):
    out_dir = os.path.join(OUT_ROOT, name)
    os.makedirs(out_dir, exist_ok=True)

    # .blend(このシーンと、使っているものだけを書き出す)
    bpy.data.libraries.write(os.path.join(out_dir, name + ".blend"), {scene},
                             path_remap="RELATIVE_ALL", fake_user=True)

    # .gltf(このシーンだけ)
    win = bpy.context.window
    old = win.scene
    win.scene = scene
    try:
        bpy.ops.export_scene.gltf(filepath=os.path.join(out_dir, name + ".gltf"),
                                  export_format="GLTF_SEPARATE",
                                  use_active_scene=True,
                                  export_apply=True)
    finally:
        win.scene = old


def cleanup():
    for sc in _created["scenes"]:
        if sc.name in bpy.data.scenes:
            bpy.data.scenes.remove(sc)
    for o in _created["objects"]:
        if o.name in bpy.data.objects:
            bpy.data.objects.remove(o, do_unlink=True)
    for m in _created["meshes"]:
        if m.name in bpy.data.meshes:
            bpy.data.meshes.remove(m)
    for m in _created["materials"]:
        if m.name in bpy.data.materials:
            bpy.data.materials.remove(m)
    for im in set(_created["images"]):
        if im.name in bpy.data.images and im.users == 0:
            bpy.data.images.remove(im)


def run(only=None):
    os.makedirs(OUT_ROOT, exist_ok=True)
    done = []
    for name, func in PROPS:
        if only and name not in only:
            continue
        sc = bpy.data.scenes.new("Camp_" + name)
        _created["scenes"].append(sc)
        b = Builder()
        func(b)
        b.build(name, sc, smooth_angle=35)
        save_prop(name, sc)
        done.append(name)
    return done


try:
    # 一部だけ作り直すときは、実行する前に ONLY = ["DungeonGate"] のように決めておく
    _done = run(globals().get("ONLY"))
    print("OUTDOOR PROPS DONE:", _done)
    if os.path.exists(LOG):
        os.remove(LOG)
except Exception:
    os.makedirs(OUT_ROOT, exist_ok=True)
    with open(LOG, "w", encoding="utf-8") as f:
        f.write(traceback.format_exc())
    traceback.print_exc()
finally:
    cleanup()
