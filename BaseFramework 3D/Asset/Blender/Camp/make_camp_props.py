# キャンプ場(準備画面の背景)の小物を作るスクリプト
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
LOG = os.path.join(OUT_ROOT, "make_camp_props_error.log")

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
# 小物
# ------------------------------------------------------------
def make_campfire(b):
    """たき火 : 石の囲い + 組んだ薪 + 燃えかす。半径0.9くらい"""
    rock = get_mat("Rock", "Rock.png", rough=0.95)
    bark = get_mat("Bark", "Bark.png", rough=0.95)
    end = get_mat("LogEnd", "LogEnd.png", rough=0.9)
    coal = get_mat("Charcoal", "Charcoal.png", rough=1.0)

    # 燃えかす(真ん中の黒い円)
    b.cyl(0.62, 0.06, coal, loc=(0, 0, 0.03), seg=20, uv_scale=(1.0, 1.0))

    # 石の囲い
    n = 11
    for i in range(n):
        a = 2 * math.pi * i / n
        r = 0.78
        b.rock(0.2, rock, (r * math.cos(a), r * math.sin(a), 0.05),
               scale=(1.1, 0.9, 0.8), seed=i)

    # 組んだ薪(真ん中で寄りかかる)
    for i in range(5):
        a = 2 * math.pi * i / 5 + 0.3
        foot = Vector((0.45 * math.cos(a), 0.45 * math.sin(a), 0.0))
        top = Vector((0.0, 0.0, 0.75))
        mid = (foot + top) / 2
        d = top - foot
        length = d.length
        tilt = math.degrees(math.acos(d.normalized().z))
        yaw = math.degrees(math.atan2(d.y, d.x))
        b.cyl(0.07, length, bark, loc=mid, rot=(0, tilt, yaw), seg=8, cap_mat=end, cap_uv="fit")

    # 横に寝かせた薪
    b.cyl(0.08, 0.9, bark, loc=(0.1, -0.15, 0.1), rot=(0, 90, 25), seg=8, cap_mat=end, cap_uv="fit")


def make_tent(b):
    """テント(三角の形)。奥行き4.2・幅3.6・高さ2.8。入り口は -Y 側"""
    canvas = get_mat("Canvas", "Canvas.png", rough=0.95)
    wood = get_mat("WoodPlank", "WoodPlank.png", rough=0.85)

    W, L, H, T = 3.6, 4.2, 2.8, 0.05
    half = W / 2
    side = math.hypot(half, H)
    ang = math.degrees(math.atan2(H, half))

    # 屋根の布(左右)。下を少し広げる
    for s in (-1, 1):
        b.box((side + 0.25, L, T), canvas, loc=(s * (half / 2 + 0.05), 0, H / 2 - 0.05),
              rot=(0, s * ang, 0), uv_scale=(0.35, 0.35))

    # 奥の三角の壁
    b.prism([(-half, 0), (half, 0), (0, H)], L / 2 - 0.02, T, canvas)

    # 手前の入り口の布(左右に開いている)
    b.prism([(-half, 0), (-0.55, 0), (0, H)], -L / 2 + 0.02, T, canvas)
    b.prism([(0.55, 0), (half, 0), (0, H)], -L / 2 + 0.02, T, canvas)

    # 棟(てっぺんの棒)と、前後の柱
    b.cyl(0.06, L + 0.5, wood, loc=(0, 0, H + 0.02), rot=(90, 0, 0), seg=8)
    for y in (-L / 2 - 0.1, L / 2 + 0.1):
        b.cyl(0.06, H + 0.2, wood, loc=(0, y, (H + 0.2) / 2), seg=8)

    # 地面のくい
    for s in (-1, 1):
        for y in (-L / 2 + 0.3, 0, L / 2 - 0.3):
            b.box((0.08, 0.08, 0.3), wood, loc=(s * (half + 0.35), y, 0.12), rot=(0, s * 20, 0))


def make_weapon_rack(b):
    """武器立て。幅2.4・高さ1.9。正面は -Y。剣とダガーは別のモデルを立てかける"""
    wood = get_mat("WoodPlank", "WoodPlank.png", rough=0.85)
    dark = get_mat("WoodDark", None, color=(0.16, 0.09, 0.05), rough=0.9)

    W, H = 2.4, 1.9
    # 左右のA字の脚
    for s in (-1, 1):
        x = s * W / 2
        for t in (-1, 1):
            b.box((0.12, 0.12, H + 0.1), wood, loc=(x, t * 0.25, H / 2), rot=(t * -8, 0, 0))
        b.box((0.1, 0.7, 0.1), wood, loc=(x, 0, 0.35))
    # 横の棒(上・真ん中)と、武器を受ける下の台
    b.cyl(0.06, W + 0.25, wood, loc=(0, 0, H - 0.1), rot=(0, 90, 0), seg=10)
    b.box((W, 0.14, 0.12), wood, loc=(0, -0.12, 1.05))
    b.box((W, 0.5, 0.1), wood, loc=(0, -0.05, 0.25))
    # 武器を差し込むくぼみ(暗い四角)
    for i in range(5):
        x = -W / 2 + 0.3 + i * (W - 0.6) / 4
        b.box((0.12, 0.16, 0.02), dark, loc=(x, -0.12, 1.12))


def make_market_stall(b):
    """露店。台の幅3.6・高さ1.1、屋根の高さ3.0。正面(お客さん側)は -Y"""
    wood = get_mat("WoodPlank", "WoodPlank.png", rough=0.85)
    cloth = get_mat("StripeCloth", "StripeCloth.png", rough=0.9)

    W, D, H = 3.6, 1.3, 1.1
    # 台(天板・前板・横板)
    b.box((W + 0.2, D + 0.2, 0.1), wood, loc=(0, 0, H))
    b.box((W, 0.08, H), wood, loc=(0, -D / 2, H / 2))
    for s in (-1, 1):
        b.box((0.08, D, H), wood, loc=(s * W / 2, 0, H / 2))
    # 柱(前は低く、後ろは高く)
    front_h, back_h = 2.6, 3.0
    for s in (-1, 1):
        b.box((0.12, 0.12, front_h), wood, loc=(s * (W / 2 + 0.05), -D / 2 - 0.05, front_h / 2))
        b.box((0.12, 0.12, back_h), wood, loc=(s * (W / 2 + 0.05), D / 2 + 0.05, back_h / 2))
    # 屋根の布(後ろから前へ下がる。前に少しはみ出す)
    depth = D + 0.9
    drop = back_h - front_h
    ang = math.degrees(math.atan2(drop, D + 0.1))
    roof_len = math.hypot(depth, drop)
    roof_z = (front_h + back_h) / 2 - 0.02
    b.box((W + 0.5, roof_len, 0.04), cloth,
          loc=(0, -0.3, roof_z), rot=(ang, 0, 0), uv_scale=(0.25, 0.25))
    # 屋根の前のふち(三角のギザギザ)。屋根の前の端にぶら下げる
    a = math.radians(ang)
    edge_y = -0.3 - roof_len / 2 * math.cos(a) + 0.03
    edge_z = roof_z - roof_len / 2 * math.sin(a)
    n = 8
    for i in range(n):
        x0 = -(W + 0.5) / 2 + i * (W + 0.5) / n
        x1 = x0 + (W + 0.5) / n
        b.prism([(x0, edge_z + 0.02), (x1, edge_z + 0.02), ((x0 + x1) / 2, edge_z - 0.3)], edge_y, 0.03, cloth)


def make_barrel(b):
    """樽。高さ1.4・半径0.55。真ん中がふくらむ"""
    wood = get_mat("WoodPlank", "WoodPlank.png", rough=0.85)
    end = get_mat("LogEnd", "LogEnd.png", rough=0.9)
    iron = get_mat("Iron", "Iron.png", rough=0.5, metal=0.8)

    H, R = 1.4, 0.55
    prof = []
    for k in range(9):
        z = H * k / 8
        r = R * (0.84 + 0.16 * math.sin(math.pi * z / H))
        prof.append((r, z))
    b.lathe(prof, wood, seg=20, uv_scale=(0.9, 0.7), cap_mat=end)
    # たが(鉄の輪)
    for z in (0.15, 0.45, H - 0.45, H - 0.15):
        r = R * (0.84 + 0.16 * math.sin(math.pi * z / H)) + 0.012
        b.cyl(r, 0.07, iron, loc=(0, 0, z), seg=20, uv_scale=(1.0, 1.0))


def make_crate(b):
    """木箱。1.0の立方体に、ふちの補強"""
    wood = get_mat("WoodPlank", "WoodPlank.png", rough=0.85)
    dark = get_mat("WoodDark", None, color=(0.16, 0.09, 0.05), rough=0.9)
    S, F = 1.0, 0.09
    b.box((S, S, S), wood, loc=(0, 0, S / 2), uv_scale=(1.0, 1.0))
    h = S / 2
    # 12本のふち
    for x in (-h, h):
        for y in (-h, h):
            b.box((F, F, S + 0.02), dark, loc=(x, y, h))
    for x in (-h, h):
        for z in (0, S):
            b.box((F, S + 0.02, F), dark, loc=(x, 0, z))
    for y in (-h, h):
        for z in (0, S):
            b.box((S + 0.02, F, F), dark, loc=(0, y, z))
    # ななめの補強(正面と背面)
    for y in (-h - 0.01, h + 0.01):
        b.box((S * 1.3, 0.04, F), dark, loc=(0, y, h), rot=(0, 45, 0))


def make_sack(b):
    """袋。高さ0.95くらい。上をしばってある"""
    burlap = get_mat("Burlap", "Burlap.png", rough=1.0)
    prof = [(0.38, 0.0), (0.46, 0.12), (0.48, 0.35), (0.42, 0.6),
            (0.26, 0.74), (0.1, 0.8), (0.09, 0.84)]
    b.lathe(prof, burlap, seg=18, uv_scale=(0.9, 1.0))
    # しばったひも
    b.cyl(0.11, 0.05, get_mat("Rope", None, color=(0.45, 0.32, 0.18), rough=1.0), loc=(0, 0, 0.82), seg=12)
    # 上の口(広がった布)
    b.cyl(0.08, 0.14, burlap, loc=(0, 0, 0.92), seg=12, radius2=0.18, uv_scale=(0.9, 1.0))


def make_log_seat(b):
    """丸太の椅子。長さ2.6・半径0.3。X方向に寝かせる"""
    bark = get_mat("Bark", "Bark.png", rough=0.95)
    end = get_mat("LogEnd", "LogEnd.png", rough=0.9)
    b.cyl(0.3, 2.6, bark, loc=(0, 0, 0.3), rot=(0, 90, 0), seg=14, cap_mat=end, cap_uv="fit",
          uv_scale=(0.8, 0.5))
    # 転がらないように下に小さな支え
    for x in (-0.9, 0.9):
        b.box((0.15, 0.5, 0.12), get_mat("WoodDark", None, color=(0.16, 0.09, 0.05), rough=0.9),
              loc=(x, 0, 0.05))


def make_stump(b):
    """切り株。半径0.55・高さ0.7。根が4本"""
    bark = get_mat("Bark", "Bark.png", rough=0.95)
    end = get_mat("LogEnd", "LogEnd.png", rough=0.9)
    b.cyl(0.55, 0.7, bark, loc=(0, 0, 0.35), seg=16, radius2=0.5, cap_mat=end, cap_uv="fit",
          uv_scale=(0.8, 0.8))
    for i in range(4):
        a = i * 90 + 20
        r = math.radians(a)
        b.box((0.55, 0.22, 0.25), bark, loc=(0.55 * math.cos(r), 0.55 * math.sin(r), 0.1),
              rot=(0, 25, a))


def make_journal(b):
    """日誌と地図(切り株や丸太の上に置く)。本は開いた状態"""
    leather = get_mat("Leather", "Leather.png", rough=0.7)
    page = get_mat("BookPage", "BookPage.png", rough=0.95)
    parch = get_mat("Parchment", "Parchment.png", rough=0.95)

    # 地図(下に広げて敷く)
    b.box((0.9, 0.7, 0.008), parch, loc=(0.1, 0.05, 0.004), rot=(0, 0, 12), uv_scale=(1.0 / 0.9, 1.0 / 0.7), uv_offset=(0.5, 0.5))
    # 本の表紙(左右)
    for s in (-1, 1):
        b.box((0.32, 0.44, 0.02), leather, loc=(s * 0.16, -0.12, 0.03), rot=(0, s * -6, -8))
    # ページ(表紙の上に少し小さく)。UVは開いたページ1枚が収まるように
    b.box((0.6, 0.4, 0.04), page, loc=(0, -0.12, 0.06), rot=(0, 0, -8), uv_scale=(1.0 / 0.6, 1.0 / 0.4), uv_offset=(0.5, 0.5))
    # 羽ペン
    b.cyl(0.008, 0.35, get_mat("Quill", None, color=(0.85, 0.82, 0.75), rough=0.8),
          loc=(0.45, -0.25, 0.03), rot=(0, 90, 30), seg=6)


PROPS = [
    ("Campfire", make_campfire),
    ("Tent", make_tent),
    ("WeaponRack", make_weapon_rack),
    ("MarketStall", make_market_stall),
    ("Barrel", make_barrel),
    ("Crate", make_crate),
    ("Sack", make_sack),
    ("LogSeat", make_log_seat),
    ("Stump", make_stump),
    ("Journal", make_journal),
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
    _done = run()
    print("CAMP PROPS DONE:", _done)
    if os.path.exists(LOG):
        os.remove(LOG)
except Exception:
    os.makedirs(OUT_ROOT, exist_ok=True)
    with open(LOG, "w", encoding="utf-8") as f:
        f.write(traceback.format_exc())
    traceback.print_exc()
finally:
    cleanup()
