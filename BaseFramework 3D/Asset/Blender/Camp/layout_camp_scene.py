# 準備画面の背景(キャンプ場)の配置ラフを組んで、カメラごとにレンダリングするスクリプト
#
# ・洞窟版(Cave)と屋外版(Outdoor)の2つを作る。シーン「CampLayout_Cave」「CampLayout_Outdoor」(開いているシーンはそのまま)
# ・配置は Asset/Blender/Camp/Layout/CampLayout_<版>.blend に保存(カメラの位置もここで直せる)
# ・レンダリング画像は Asset/Blender/Camp/Layout/Render/<版>_<カメラ>.png
# ・座標はBlender(Z上)。入り口は +Y の奥、手前が -Y。1マス = ゲームと同じ大きさ(プレイヤー身長 約3)

import bpy, bmesh, os, math, traceback
from mathutils import Vector, Matrix

ASSET = r"C:\GitHub\3DGAME\BaseFramework 3D\Asset"
MODELS = os.path.join(ASSET, "Models")
CAMP = os.path.join(ASSET, "Blender", "Camp")
OUT = os.path.join(CAMP, "Layout")
RENDER = os.path.join(OUT, "Render")
LOG = os.path.join(OUT, "layout_error.log")

SCENE_NAME = "CampLayout"
TEX = os.path.join(ASSET, "Textures", "Camp")


# ------------------------------------------------------------
# 便利な関数
# ------------------------------------------------------------
def new_scene(name):
    old = bpy.data.scenes.get(name)
    if old:
        for o in list(old.objects):
            bpy.data.objects.remove(o, do_unlink=True)
        bpy.data.scenes.remove(old)
    sc = bpy.data.scenes.new(name)
    bpy.context.window.scene = sc
    return sc


def game_model_path(path):
    """Blender側のパスを、ゲームが読み込むパス(Asset/Models/...)に直す"""
    p = os.path.normpath(path)
    camp = os.path.normpath(CAMP)
    if p.startswith(camp):
        name = os.path.splitext(os.path.basename(p))[0]
        return "Asset/Models/Terrains/Camp/%s/%s.gltf" % (name, name)
    rel = os.path.relpath(p, os.path.normpath(ASSET)).replace("\\", "/")
    return "Asset/" + rel


def world_bbox(objs):
    mn = Vector((1e9, 1e9, 1e9)); mx = Vector((-1e9, -1e9, -1e9))
    for o in objs:
        if o.type != "MESH":
            continue
        for c in o.bound_box:
            w = o.matrix_world @ Vector(c)
            mn = Vector((min(mn[i], w[i]) for i in range(3)))
            mx = Vector((max(mx[i], w[i]) for i in range(3)))
    return mn, mx


def import_gltf(path, loc=(0, 0, 0), rot_z=0.0, height=None, upright=False, name=None, scale=None, by_origin=False, kind="Prop"):
    """glTFを読み込んで空の親にまとめ、足元を loc に置く。height を指定すると高さを合わせる"""
    loc = tuple(loc) + (0.0,) * (3 - len(loc))
    before = set(bpy.data.objects)
    bpy.ops.import_scene.gltf(filepath=path)
    new = [o for o in bpy.data.objects if o not in before]
    root = bpy.data.objects.new(name or os.path.splitext(os.path.basename(path))[0], None)
    bpy.context.scene.collection.objects.link(root)
    root["kind"] = kind
    root["model"] = game_model_path(path)
    for o in new:
        if o.parent is None:
            o.parent = root
    bpy.context.view_layer.update()

    if upright:
        # いちばん長い向きを縦にする(武器を立てかける用)
        mn, mx = world_bbox(new)
        d = mx - mn
        if d.x >= d.y and d.x >= d.z:
            root.rotation_euler = (0, math.radians(90), 0)
        elif d.y >= d.x and d.y >= d.z:
            root.rotation_euler = (math.radians(90), 0, 0)
        bpy.context.view_layer.update()

    if height:
        mn, mx = world_bbox(new)
        h = mx.z - mn.z
        if h > 1e-4:
            s = height / h
            root.scale = (s, s, s)
            bpy.context.view_layer.update()

    if scale:
        sv = scale if isinstance(scale, (tuple, list)) else (scale, scale, scale)
        root.scale = (root.scale[0] * sv[0], root.scale[1] * sv[1], root.scale[2] * sv[2])

    rot = root.rotation_euler.copy()
    root.rotation_euler = (rot.x, rot.y, rot.z + math.radians(rot_z))
    bpy.context.view_layer.update()

    if by_origin:
        # モデルの原点をそのまま loc に置く(入り口や崖など、原点を決めて作ったモデル用)
        root.location = Vector(loc)
        bpy.context.view_layer.update()
        return root, new

    # 足元(バウンディングボックスの下の真ん中)を loc に合わせる
    mn, mx = world_bbox(new)
    c = Vector(((mn.x + mx.x) / 2, (mn.y + mx.y) / 2, mn.z))
    root.location += Vector(loc) - c
    bpy.context.view_layer.update()
    return root, new


def tex_mat(name, color_path, normal_path=None, tile=1.0, rough=0.9):
    mat = bpy.data.materials.new(name)
    mat.use_nodes = True
    nt = mat.node_tree
    bsdf = next(n for n in nt.nodes if n.type == "BSDF_PRINCIPLED")
    bsdf.inputs["Roughness"].default_value = rough
    coord = nt.nodes.new("ShaderNodeTexCoord")
    mapping = nt.nodes.new("ShaderNodeMapping")
    mapping.inputs["Scale"].default_value = (tile, tile, tile)
    nt.links.new(coord.outputs["UV"], mapping.inputs["Vector"])
    img = nt.nodes.new("ShaderNodeTexImage")
    img.image = bpy.data.images.load(color_path, check_existing=True)
    nt.links.new(mapping.outputs["Vector"], img.inputs["Vector"])
    nt.links.new(img.outputs["Color"], bsdf.inputs["Base Color"])
    if normal_path and os.path.exists(normal_path):
        nimg = nt.nodes.new("ShaderNodeTexImage")
        nimg.image = bpy.data.images.load(normal_path, check_existing=True)
        nimg.image.colorspace_settings.name = "Non-Color"
        nt.links.new(mapping.outputs["Vector"], nimg.inputs["Vector"])
        nm = nt.nodes.new("ShaderNodeNormalMap")
        nt.links.new(nimg.outputs["Color"], nm.inputs["Color"])
        nt.links.new(nm.outputs["Normal"], bsdf.inputs["Normal"])
    return mat


def emit_mat(name, color, strength):
    mat = bpy.data.materials.new(name)
    mat.use_nodes = True
    bsdf = next(n for n in mat.node_tree.nodes if n.type == "BSDF_PRINCIPLED")
    bsdf.inputs["Base Color"].default_value = (*color, 1)
    bsdf.inputs["Emission Color"].default_value = (*color, 1)
    bsdf.inputs["Emission Strength"].default_value = strength
    return mat


def box(name, size, loc, mat, uv_tile=None, rot=(0, 0, 0)):
    """size=(x,y,z) の箱。loc は真ん中。UVは世界の大きさに合わせた箱投影(uv_tile マスで画像1枚)"""
    tile = uv_tile or 4.0
    me = bpy.data.meshes.new(name)
    bm = bmesh.new()
    bmesh.ops.create_cube(bm, size=1.0)
    bmesh.ops.scale(bm, vec=Vector(size), verts=bm.verts)
    rx, ry, rz = (math.radians(a) for a in rot)
    m = Matrix.Translation(loc) @ Matrix.Rotation(rz, 4, "Z") @ Matrix.Rotation(ry, 4, "Y") @ Matrix.Rotation(rx, 4, "X")
    bmesh.ops.transform(bm, matrix=m, verts=bm.verts)
    bm.normal_update()
    uv = bm.loops.layers.uv.verify()
    for f in bm.faces:
        n = f.normal
        for l in f.loops:
            co = l.vert.co
            if abs(n.z) >= abs(n.x) and abs(n.z) >= abs(n.y):
                l[uv].uv = (co.x / tile, co.y / tile)
            elif abs(n.x) >= abs(n.y):
                l[uv].uv = (co.y / tile, co.z / tile)
            else:
                l[uv].uv = (co.x / tile, co.z / tile)
    bm.to_mesh(me)
    bm.free()
    me.materials.append(mat)
    o = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(o)
    return o


def color_mat(name, color, rough=0.9):
    mat = bpy.data.materials.new(name)
    mat.use_nodes = True
    bsdf = next(n for n in mat.node_tree.nodes if n.type == "BSDF_PRINCIPLED")
    bsdf.inputs["Base Color"].default_value = (*color, 1)
    bsdf.inputs["Roughness"].default_value = rough
    return mat


def cone(name, r1, r2, depth, loc, mat, seg=10):
    me = bpy.data.meshes.new(name)
    bm = bmesh.new()
    bmesh.ops.create_cone(bm, cap_ends=True, cap_tris=False, segments=seg, radius1=r1, radius2=r2, depth=depth)
    bmesh.ops.translate(bm, vec=Vector((loc[0], loc[1], loc[2] + depth / 2)), verts=bm.verts)
    bm.to_mesh(me); bm.free()
    me.materials.append(mat)
    o = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(o)
    return o


def tree(name, loc, h=7.0):
    """簡単な針葉樹(ラフ用)"""
    trunk = color_mat("Layout_Trunk", (0.12, 0.07, 0.04))
    leaf = color_mat("Layout_Leaf", (0.03, 0.09, 0.05))
    cone(name + "_Trunk", 0.25, 0.2, h * 0.35, loc, trunk, 8)
    for i, (r, z, d) in enumerate([(1.9, 0.25, 0.4), (1.5, 0.45, 0.35), (1.0, 0.65, 0.35)]):
        cone("%s_Leaf%d" % (name, i), r * h / 7, 0.0, d * h, (loc[0], loc[1], loc[2] + z * h), leaf, 10)


def big_rock(name, loc, scale, mat, seed=0):
    import random
    rnd = random.Random(seed)
    me = bpy.data.meshes.new(name)
    bm = bmesh.new()
    bmesh.ops.create_icosphere(bm, subdivisions=2, radius=1.0)
    for v in bm.verts:
        v.co *= 0.8 + 0.35 * rnd.random()
        v.co = Vector((v.co.x * scale[0], v.co.y * scale[1], max(v.co.z, -0.3) * scale[2]))
    bmesh.ops.translate(bm, vec=Vector(loc), verts=bm.verts)
    bm.normal_update()
    uv = bm.loops.layers.uv.verify()
    for f in bm.faces:
        for l in f.loops:
            co = l.vert.co
            l[uv].uv = ((co.x + co.y) / 4.0, co.z / 4.0)
    bm.to_mesh(me); bm.free()
    me.materials.append(mat)
    o = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(o)
    return o


def point_light(name, loc, color, power, radius=0.2):
    l = bpy.data.lights.new(name, "POINT")
    l.color = color
    l.energy = power
    l.shadow_soft_size = radius
    o = bpy.data.objects.new(name, l)
    o.location = loc
    o["kind"] = "Light"
    bpy.context.scene.collection.objects.link(o)
    return o


def flame(name, loc, scale=1.0):
    """炎の代わりの光る円すい(レンダリング用。ゲームではEffekseerの炎)"""
    mat = emit_mat(name + "_mat", (1.0, 0.32, 0.04), 2.5)
    bpy.ops.mesh.primitive_cone_add(vertices=10, radius1=0.35 * scale, radius2=0.0,
                                    depth=0.9 * scale, location=(loc[0], loc[1], loc[2] + 0.45 * scale))
    o = bpy.context.active_object
    o.name = name
    o.data.materials.append(mat)
    inner = emit_mat(name + "_core", (1.0, 0.65, 0.2), 4.0)
    bpy.ops.mesh.primitive_cone_add(vertices=8, radius1=0.18 * scale, radius2=0.0,
                                    depth=0.6 * scale, location=(loc[0], loc[1], loc[2] + 0.3 * scale))
    o2 = bpy.context.active_object
    o2.data.materials.append(inner)
    return o


def camera(name, loc, target, lens=28.0, shift_x=0.0, ortho=None):
    name = bpy.context.scene.name.replace("CampLayout_", "") + "_" + name
    cd = bpy.data.cameras.new(name)
    cd.lens = lens
    cd.shift_x = shift_x
    cd.clip_end = 300
    if ortho:
        cd.type = "ORTHO"
        cd.ortho_scale = ortho
    co = bpy.data.objects.new(name, cd)
    co.location = loc
    co["target"] = list(target)
    d = Vector(target) - Vector(loc)
    co.rotation_euler = d.to_track_quat("-Z", "Y").to_euler()
    bpy.context.scene.collection.objects.link(co)
    return co


# ------------------------------------------------------------
# 組み立て
# ------------------------------------------------------------
def build(mode):
    sc = new_scene(SCENE_NAME + "_" + mode)
    cave = (mode == "Cave")

    # 見た目の設定
    for eng in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
        try:
            sc.render.engine = eng
            break
        except Exception:
            pass
    sc.render.resolution_x = 1280
    sc.render.resolution_y = 720
    sc.render.resolution_percentage = 100
    sc.view_settings.exposure = 0.5
    try:
        sc.eevee.taa_render_samples = 32
    except Exception:
        pass
    world = bpy.data.worlds.new("CampWorld")
    world.use_nodes = True
    bg = next(n for n in world.node_tree.nodes if n.type == "BACKGROUND")
    bg.inputs["Color"].default_value = (0.004, 0.005, 0.009, 1) if cave else (0.012, 0.018, 0.04, 1)
    bg.inputs["Strength"].default_value = 1.0
    sc.world = world

    # 床と壁(ダンジョンと同じ石)
    floor_dir = os.path.join(MODELS, "Terrains", "Map", "Castle", "Floor")
    wall_dir = os.path.join(MODELS, "Terrains", "Map", "Castle", "Wall")
    floor_mat = tex_mat("Layout_Floor", os.path.join(floor_dir, "stone_2_c.png"),
                        os.path.join(floor_dir, "NormalMap.png"))
    wall_mat = tex_mat("Layout_Wall", os.path.join(wall_dir, "stone_c.png"),
                       os.path.join(wall_dir, "NormalMap.png"))

    # 配置は小さくまとめる : 入り口(奥 y=9)の前に、たき火を真ん中にして左にテント、右に露店
    GATE_Y = 9.0
    if cave:
        # ---- 洞窟の広間 : 石の床、奥と左右を石の壁で囲う(幅24・奥行き18くらい) ----
        box("Floor", (26, 22, 0.2), (0, 1, -0.1), floor_mat, uv_tile=4)
        box("Wall_Back_L", (9.5, 1.2, 9), (-7.25, GATE_Y + 0.6, 4.5), wall_mat)
        box("Wall_Back_R", (9.5, 1.2, 9), (7.25, GATE_Y + 0.6, 4.5), wall_mat)
        box("Wall_Back_Top", (5, 1.2, 4), (0, GATE_Y + 0.6, 7.0), wall_mat)
        box("Wall_Left", (1.2, 19, 8), (-12, 0.5, 4), wall_mat)
        box("Wall_Right", (1.2, 19, 8), (12, 0.5, 4), wall_mat)
    else:
        # ---- 屋外 : 土の地面、奥は崖、まわりに木と岩、夜空(Asset/Blender/Camp の屋外用モデル) ----
        def model(name, loc, rot_z=0.0, scale=None, tag=None):
            return import_gltf(os.path.join(CAMP, name, name + ".gltf"), loc=loc, rot_z=rot_z,
                               scale=scale, by_origin=True, name=tag or name)
        cy = GATE_Y + 3.0
        model("GroundDirt", (0, 2, 0))
        # 崖(入り口の左右と上、さらに外側)
        model("CliffA", (-8.4, cy, 0), tag="Cliff_L")
        model("CliffB", (8.0, cy, 0), tag="Cliff_R")
        model("CliffB", (-17.0, cy + 0.8, 0), scale=1.1, tag="Cliff_LL")
        model("CliffA", (17.5, cy + 0.8, 0), tag="Cliff_RR")
        model("CliffA", (0, cy + 0.2, 5.6), scale=(0.75, 1.0, 0.75), tag="Cliff_Top")
        # 岩
        model("BoulderB", (3.9, GATE_Y + 0.6, 0), scale=0.8, tag="Rock_GateR")
        model("BoulderA", (-6.2, GATE_Y - 0.8, 0), rot_z=20, tag="Rock_0")
        model("BoulderB", (6.6, GATE_Y - 0.4, 0), tag="Rock_1")
        model("BoulderA", (-10.5, GATE_Y - 3.0, 0), rot_z=-30, scale=0.8, tag="Rock_2")
        model("BoulderB", (10.6, GATE_Y - 3.4, 0), scale=0.8, tag="Rock_3")
        # 木と茂み
        for i, (x, y, h) in enumerate([(-11.5, 6, 8), (-13.5, 1, 7.5), (-12, -4, 7), (-15, -7, 8.5),
                                       (11.5, 6.5, 7.5), (13.5, 1.5, 8), (12, -3.5, 7), (15, -7, 8.5)]):
            model("PineTree", (x, y, 0), rot_z=i * 47, scale=h / 8.0, tag="Tree_%d" % i)
        for i, (x, y) in enumerate([(-9.5, 3.0), (9.8, 2.5), (-3.8, 7.4), (3.6, 7.8), (-12.0, -1.0), (12.5, -2.0)]):
            model("Bush", (x, y, 0), rot_z=i * 61, scale=1.1, tag="Bush_%d" % i)
        # 月明かり
        moon = bpy.data.lights.new("Moon", "SUN")
        moon.energy = 1.2
        moon.color = (0.6, 0.7, 1.0)
        mo = bpy.data.objects.new("Moon", moon)
        mo.rotation_euler = (math.radians(60), 0, math.radians(35))
        sc.collection.objects.link(mo)

    # 入り口(石の門と奥の通路。Asset/Blender/Camp/DungeonGate)
    import_gltf(os.path.join(CAMP, "DungeonGate", "DungeonGate.gltf"), loc=(0, GATE_Y, 0), by_origin=True, name="DungeonGate")
    ty = GATE_Y + 5.0

    # 入り口の奥(通路の先がうっすら青く光る。ゲームではワープゲートのエフェクトなどに置き換える)
    box("Tunnel_Glow", (4.6, 0.1, 4.6), (0, ty + 3.4, 2.3), emit_mat("Layout_TunnelGlow", (0.25, 0.45, 1.0), 0.35))
    point_light("Light_Gate", (0, ty + 1.0, 2.2), (0.4, 0.65, 1.0), 500, 1.0)

    # 入り口の左右のたいまつ(壁にかける。手前 -Y に向ける)
    torch_path = os.path.join(MODELS, "Terrains", "MapObj", "Torch", "Torch.gltf")
    for s in (-1, 1):
        x = s * 3.9
        import_gltf(torch_path, loc=(x, GATE_Y - 0.5, 2.4), rot_z=180, height=1.3, name="Torch_%d" % s, kind="Torch")
        flame("TorchFlame_%d" % s, (x, GATE_Y - 0.8, 3.6), 0.45)
        point_light("Light_Torch_%d" % s, (x, GATE_Y - 1.1, 4.0), (1.0, 0.55, 0.2), 600, 0.3)

    # ---- キャンプ ----
    def prop(name, loc, rot_z=0.0, kind="Prop"):
        return import_gltf(os.path.join(CAMP, name, name + ".gltf"), loc=loc, rot_z=rot_z, name=name, kind=kind)

    # たき火(真ん中)と、まわりの丸太・切り株(記録の日誌)
    prop("Campfire", (0, 1), kind="Campfire")
    flame("CampFlame", (0, 1, 0.1), 1.0)
    point_light("Light_Campfire", (0, 1, 1.4), (1.0, 0.5, 0.18), 2200, 0.6)
    prop("LogSeat", (-2.2, 1.2), 90)
    prop("LogSeat", (0.2, -1.4), 0)
    prop("Stump", (2.0, 2.4))
    import_gltf(os.path.join(CAMP, "Journal", "Journal.gltf"), loc=(2.0, 2.4, 0.7), rot_z=-20, name="Journal")

    # テントと武器立て(左)
    prop("Tent", (-6.0, 3.6), 90)
    point_light("Light_Tent", (-3.4, 3.6, 2.6), (1.0, 0.7, 0.4), 200, 0.3)
    prop("WeaponRack", (-4.4, -0.6), 50)
    sword_dir = os.path.join(MODELS, "Weapon", "Sword", "Warrior_Sword.gltf")
    dagger_dir = os.path.join(MODELS, "Weapon", "Dagger", "Rogue_Dagger.gltf")
    import_gltf(sword_dir, loc=(-4.85, -1.05, 0.25), rot_z=50, height=1.7, upright=True, name="RackSword")
    import_gltf(dagger_dir, loc=(-4.1, -0.35, 0.25), rot_z=50, height=0.9, upright=True, name="RackDagger")
    prop("Crate", (-7.4, 0.4), 15)

    # 露店と荷物(右)
    prop("MarketStall", (5.6, 3.6), -90)
    point_light("Light_Stall", (4.3, 3.6, 2.4), (1.0, 0.7, 0.4), 600, 0.3)
    prop("Barrel", (7.6, 6.0))
    prop("Barrel", (8.3, 4.9), 30)
    prop("Crate", (7.6, 0.8), -10)
    import_gltf(os.path.join(CAMP, "Crate", "Crate.gltf"), loc=(7.6, 0.8, 1.0), rot_z=20, name="Crate_Top")
    prop("Sack", (4.4, 6.3))
    prop("Sack", (3.8, 5.8), 40)
    prop("Sack", (8.6, 2.2))

    # プレイヤー(入り口を向いて立つ)
    try:
        import_gltf(os.path.join(MODELS, "Character", "Player", "Rogue.gltf"), loc=(0.5, 4.8, 0), rot_z=0, height=3.0, name="Player", kind="Player")
    except Exception:
        traceback.print_exc()

    # 全体を少しだけ照らす(真っ暗にならないように)
    sun = bpy.data.lights.new("Fill", "SUN")
    sun.energy = 0.6 if cave else 0.25
    sun.color = (0.55, 0.65, 1.0)
    so = bpy.data.objects.new("Fill", sun)
    so.rotation_euler = (math.radians(55), 0, math.radians(-30))
    sc.collection.objects.link(so)

    # ---- カメラ ----
    # shift_x を + にすると、映したいものが画面の左に寄る(右にUIのパネルがあるため)
    cams = [
        camera("Cam_Overview_Top", (0, 2, 40), (0, 2, 0), ortho=24),
        camera("Cam_Overview_Wide", (0, -13, 9), (0, 3, 1), lens=24),
        camera("Cam_Explore", (1.6, -3.8, 2.8), (0, GATE_Y, 2.4), lens=26, shift_x=0.18),
        camera("Cam_Equip", (0.6, -5.2, 2.8), (-4.8, 1.4, 1.3), lens=30, shift_x=0.28),
        camera("Cam_Shop", (-0.2, -4.8, 3.0), (5.6, 3.0, 1.5), lens=30, shift_x=0.28),
        camera("Cam_Records", (-1.8, -3.4, 2.5), (1.0, 1.4, 0.5), lens=32, shift_x=0.28),
    ]
    return sc, cams


def render_all(sc, cams, mode):
    out = RENDER
    os.makedirs(out, exist_ok=True)
    for c in cams:
        sc.camera = c
        sc.render.filepath = os.path.join(out, mode + "_" + c.name.split("Cam_")[-1] + ".png")
        bpy.ops.render.render(write_still=True, scene=sc.name)


# ------------------------------------------------------------
# ゲーム用の書き出し(屋外版)
#   ・配置 : Asset/Data/ObjeData/PrepareObjeData/CampLayout.json
#   ・モデル : Asset/Blender/Camp/<名前>/ を Asset/Models/Terrains/Camp/<名前>/ にコピー
#   座標はglTFと同じ(Y上・右手系)。Blenderの(x, y, z) → (x, z, -y)。モデルの頂点と同じ変換なので、
#   ゲームではglTFのノードの位置・回転・大きさと同じように扱えばよい
# ------------------------------------------------------------
DATA_OUT = os.path.join(ASSET, "Data", "ObjeData", "PrepareObjeData", "CampLayout.json")
CONV = Matrix(((1, 0, 0, 0), (0, 0, 1, 0), (0, -1, 0, 0), (0, 0, 0, 1)))   # Blender → glTF


def _to_gltf(mw):
    m = CONV @ mw @ CONV.inverted()
    loc, q, sca = m.decompose()
    e = q.to_euler("XYZ")
    r3 = lambda v: [round(float(x), 4) for x in v]
    return r3(loc), r3([math.degrees(a) for a in e]), r3(sca)


def _pt(v):
    p = CONV @ Vector((v[0], v[1], v[2], 1.0))
    return [round(float(p[0]), 4), round(float(p[1]), 4), round(float(p[2]), 4)]


def export_for_game(sc):
    import json, shutil
    objs, lights, cams = [], [], {}
    for o in sc.objects:
        kind = o.get("kind")
        if kind in ("Prop", "Torch", "Campfire", "Player") and o.get("model"):
            pos, rot, sca = _to_gltf(o.matrix_world)
            objs.append({"Name": o.name, "Kind": kind, "Model": o["model"], "Pos": pos, "Rot": rot, "Scale": sca})
        elif kind == "Light":
            lights.append({"Name": o.name, "Pos": _pt(o.matrix_world.translation),
                           "Color": [round(c, 3) for c in o.data.color], "Power": o.data.energy})
    for o in sc.objects:
        if o.type != "CAMERA" or "Overview" in o.name:
            continue
        tab = o.name.split("Cam_")[-1]
        cd = o.data
        # Blenderでは shift_x で絵を横にずらしているが、ゲームのカメラにはないので、
        # ずらした後の絵の真ん中を向く「注視点」に直して書き出す
        fwd = (o.matrix_world.to_3x3() @ Vector((0, 0, -1))).normalized()
        right = (o.matrix_world.to_3x3() @ Vector((1, 0, 0))).normalized()
        d = (fwd + right * (cd.shift_x * cd.sensor_width / cd.lens)).normalized()
        tgt = o.matrix_world.translation + d * 10.0
        fov_y = math.degrees(2 * math.atan((cd.sensor_width * 9.0 / 16.0) / 2.0 / cd.lens))
        cams[tab] = {"Pos": _pt(o.matrix_world.translation), "Target": _pt(tgt), "FovY": round(fov_y, 2)}
    data = {"Objects": objs, "Lights": lights, "Cameras": cams}
    os.makedirs(os.path.dirname(DATA_OUT), exist_ok=True)
    with open(DATA_OUT, "w", encoding="utf-8") as f:
        json.dump(data, f, ensure_ascii=False, indent=4)

    # キャンプのモデルをゲームのフォルダへコピー
    copied = set()
    for o in objs:
        m = o["Model"]
        if not m.startswith("Asset/Models/Terrains/Camp/"):
            continue
        name = m.split("/")[-2]
        if name in copied:
            continue
        src = os.path.join(CAMP, name)
        dst = os.path.join(ASSET, "Models", "Terrains", "Camp", name)
        os.makedirs(dst, exist_ok=True)
        for fn in os.listdir(src):
            if fn.lower().endswith((".gltf", ".bin", ".png")):
                shutil.copy2(os.path.join(src, fn), os.path.join(dst, fn))
        copied.add(name)
    print("EXPORT:", len(objs), "objects,", len(lights), "lights,", list(cams.keys()), "copied:", sorted(copied))


def save_layout(sc, mode):
    os.makedirs(OUT, exist_ok=True)
    bpy.data.libraries.write(os.path.join(OUT, "CampLayout_%s.blend" % mode), {sc},
                             path_remap="ABSOLUTE", fake_user=True)


try:
    _home_name = bpy.context.window.scene.name
    for _mode in ("Cave", "Outdoor"):
        _sc, _cams = build(_mode)
        save_layout(_sc, _mode)
        render_all(_sc, _cams, _mode)
        if _mode == "Outdoor":
            export_for_game(_sc)
    _home = bpy.data.scenes.get(_home_name)
    if _home is None or _home.name.startswith(SCENE_NAME):
        _home = next((x for x in bpy.data.scenes if not x.name.startswith(SCENE_NAME)), None)
    if _home:
        bpy.context.window.scene = _home
    print("CAMP LAYOUT DONE")
    if os.path.exists(LOG):
        os.remove(LOG)
except Exception:
    os.makedirs(OUT, exist_ok=True)
    with open(LOG, "w", encoding="utf-8") as f:
        f.write(traceback.format_exc())
    traceback.print_exc()
