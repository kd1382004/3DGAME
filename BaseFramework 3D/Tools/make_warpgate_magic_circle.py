# -*- coding: utf-8 -*-
# WarpGate（Fine / Next）を魔法陣モデルに作り替えて glTF に書き出すスクリプト
# Blender の Python コンソールで次のように実行:
#   exec(open(r"C:\3DGAME\3DGAME\BaseFramework 3D\Tools\make_warpgate_magic_circle.py", encoding="utf-8").read())

import bpy, bmesh, math, os, shutil, random
from mathutils import Vector

BASE_DIR = r"C:\3DGAME\3DGAME\BaseFramework 3D\Asset\Models\WarpGate"

GATES = {
    # 名前 : (フォルダ, ベースの色 = 元モデルの水の色)
    "Fine": ("Fine", (0.0, 1.0, 0.0)),
    "Next": ("Next", (1.0, 0.0, 0.0)),   # 赤
}

R = 3.7          # 魔法陣の半径(m) 元の噴水とほぼ同じ大きさ
BASE_H = 0.06    # 土台の厚み
GLOW_Z = 0.065   # 模様の高さ（土台の少し上）


# ------------------------------------------------------------
# 形を作る道具
# ------------------------------------------------------------
def add_quad(bm, pts):
    vs = [bm.verts.new((p[0], p[1], GLOW_Z)) for p in pts]
    bm.faces.new(vs)


def ring(bm, r_in, r_out, segs=96, cx=0.0, cy=0.0):
    for i in range(segs):
        a0 = 2 * math.pi * i / segs
        a1 = 2 * math.pi * (i + 1) / segs
        add_quad(bm, [
            (cx + r_in * math.cos(a0), cy + r_in * math.sin(a0)),
            (cx + r_out * math.cos(a0), cy + r_out * math.sin(a0)),
            (cx + r_out * math.cos(a1), cy + r_out * math.sin(a1)),
            (cx + r_in * math.cos(a1), cy + r_in * math.sin(a1)),
        ])


def strip(bm, p1, p2, w):
    d = Vector((p2[0] - p1[0], p2[1] - p1[1]))
    if d.length < 1e-6:
        return
    n = Vector((-d.y, d.x)).normalized() * (w * 0.5)
    add_quad(bm, [
        (p1[0] - n.x, p1[1] - n.y),
        (p2[0] - n.x, p2[1] - n.y),
        (p2[0] + n.x, p2[1] + n.y),
        (p1[0] + n.x, p1[1] + n.y),
    ])


def polygon_lines(bm, radius, count, start_deg, w):
    pts = []
    for i in range(count):
        a = math.radians(start_deg + 360.0 * i / count)
        pts.append((radius * math.cos(a), radius * math.sin(a)))
    for i in range(count):
        strip(bm, pts[i], pts[(i + 1) % count], w)
    return pts


def diamond(bm, cx, cy, size, angle):
    pts = []
    for k in range(4):
        a = angle + math.pi * 0.5 * k
        s = size if k % 2 == 0 else size * 0.45
        pts.append((cx + s * math.cos(a), cy + s * math.sin(a)))
    add_quad(bm, pts)


# ルーン文字っぽい記号（-1〜1 の箱の中の線分）
GLYPHS = [
    [((-0.6, -1), (-0.6, 1)), ((-0.6, 1), (0.6, 0.3)), ((-0.6, 0.0), (0.6, -0.7))],
    [((0, -1), (0, 1)), ((-0.7, 0.4), (0.7, -0.4)), ((-0.7, -0.4), (0.7, 0.4))],
    [((-0.6, -1), (-0.6, 1)), ((0.6, -1), (0.6, 1)), ((-0.6, 1), (0.6, -0.2))],
    [((0, -1), (0, 1)), ((0, 1), (0.7, 0.4)), ((0, 0.4), (-0.7, -0.2))],
    [((-0.7, -1), (0, 1)), ((0, 1), (0.7, -1)), ((-0.35, -0.1), (0.35, -0.1))],
    [((-0.6, 1), (0.6, -1)), ((0.6, 1), (-0.6, -1)), ((-0.6, 0), (0.6, 0))],
    [((0, -1), (0, 1)), ((-0.6, 1), (0.6, 1)), ((-0.6, -1), (0.6, -1))],
    [((-0.6, -1), (-0.6, 1)), ((-0.6, 1), (0.6, 0.5)), ((0.6, 0.5), (-0.6, 0))],
]


def glyph(bm, cx, cy, angle, size, idx, w):
    g = GLYPHS[idx % len(GLYPHS)]
    ca, sa = math.cos(angle), math.sin(angle)

    def tr(p):
        # 記号の上方向を魔法陣の外側に向ける
        x, y = p[0] * size, p[1] * size
        return (cx + x * (-sa) + y * ca, cy + x * ca + y * sa)

    for a, b in g:
        strip(bm, tr(a), tr(b), w)


# ------------------------------------------------------------
# マテリアル
# ------------------------------------------------------------
def make_material(name, base, emissive=None, rough=0.6):
    mat = bpy.data.materials.get(name) or bpy.data.materials.new(name)
    if not mat.node_tree:
        mat.use_nodes = True
    # UI が日本語だとノード名が翻訳されるので、種類で探す
    bsdf = next(n for n in mat.node_tree.nodes if n.type == "BSDF_PRINCIPLED")
    bsdf.inputs["Base Color"].default_value = (*base, 1.0)
    bsdf.inputs["Roughness"].default_value = rough
    bsdf.inputs["Metallic"].default_value = 0.0
    if emissive:
        bsdf.inputs["Emission Color"].default_value = (*emissive, 1.0)
        bsdf.inputs["Emission Strength"].default_value = 1.0
    else:
        bsdf.inputs["Emission Strength"].default_value = 0.0
    return mat


def finish_mesh(bm, name, mat):
    # 面を全部上向きに揃え、UV を真上から貼る
    uv = bm.loops.layers.uv.verify()
    for f in bm.faces:
        f.normal_update()
        if f.normal.z < 0:
            f.normal_flip()
        for loop in f.loops:
            co = loop.vert.co
            loop[uv].uv = (co.x / (2 * R) + 0.5, co.y / (2 * R) + 0.5)
    me = bpy.data.meshes.new(name)
    bm.to_mesh(me)
    bm.free()
    me.materials.append(mat)
    obj = bpy.data.objects.new(name, me)
    return obj


# ------------------------------------------------------------
# 魔法陣 1つ分を作る
# ------------------------------------------------------------
def build_gate(name, color):
    random.seed(7 if name == "Fine" else 21)

    col = bpy.data.collections.new("MagicCircle_" + name)
    bpy.context.scene.collection.children.link(col)

    # 土台（暗い円盤）… TypeGround の当たり判定がスカスカにならないように
    dark = tuple(0.02 + c * 0.05 for c in color)
    base_mat = make_material(name + "_Base", dark, None, 0.85)
    bm = bmesh.new()
    bmesh.ops.create_cone(bm, cap_ends=True, cap_tris=False, segments=96,
                          radius1=R + 0.05, radius2=R + 0.05, depth=BASE_H)
    bmesh.ops.translate(bm, verts=bm.verts, vec=(0, 0, BASE_H * 0.5))
    uv = bm.loops.layers.uv.verify()
    for f in bm.faces:
        for loop in f.loops:
            co = loop.vert.co
            loop[uv].uv = (co.x / (2 * R) + 0.5, co.y / (2 * R) + 0.5)
    me = bpy.data.meshes.new(name + "_Base")
    bm.to_mesh(me)
    bm.free()
    me.materials.append(base_mat)
    base_obj = bpy.data.objects.new(name + "_Base", me)
    col.objects.link(base_obj)

    # 当たり判定専用ノード（名前に COL を含むと描画されず判定だけに使われる）
    bm = bmesh.new()
    bmesh.ops.create_cone(bm, cap_ends=True, cap_tris=False, segments=48,
                          radius1=R + 0.05, radius2=R + 0.05, depth=BASE_H)
    bmesh.ops.translate(bm, verts=bm.verts, vec=(0, 0, BASE_H * 0.5))
    me = bpy.data.meshes.new(name + "_COL")
    bm.to_mesh(me)
    bm.free()
    me.materials.append(base_mat)
    col_obj = bpy.data.objects.new(name + "_COL", me)
    col.objects.link(col_obj)

    # 光る模様（回転させるため、パーツごとに別オブジェクトにする）
    glow_mat = make_material(name + "_Glow", color, color, 0.4)
    objs = [col_obj, base_obj]
    w = 0.07

    # Outer：外周リング＋ルーンの帯
    bm = bmesh.new()
    ring(bm, R - 0.14, R - 0.02, 128)
    ring(bm, R - 0.30, R - 0.26, 128)
    ring(bm, 3.02, 3.10, 128)
    n = 28
    for i in range(n):
        a = 2 * math.pi * i / n
        rr = 3.33
        glyph(bm, rr * math.cos(a), rr * math.sin(a), a, 0.11,
              random.randrange(len(GLYPHS)), 0.035)
    objs.append(finish_mesh(bm, name + "_Outer", glow_mat))

    # Star：六芒星＋頂点の飾り
    bm = bmesh.new()
    pts1 = polygon_lines(bm, 3.02, 3, 90, w)
    pts2 = polygon_lines(bm, 3.02, 3, 30, w)
    for p in pts1 + pts2:
        ring(bm, 0.17, 0.23, 32, p[0], p[1])
        diamond(bm, p[0], p[1], 0.11, math.atan2(p[1], p[0]))
    objs.append(finish_mesh(bm, name + "_Star", glow_mat))

    # Inner：内側の円と六角形
    bm = bmesh.new()
    inner = 3.02 / math.sqrt(3.0)
    ring(bm, inner - 0.06, inner, 96)
    polygon_lines(bm, inner - 0.03, 6, 0, 0.04)
    for i in range(12):
        a = 2 * math.pi * (i + 0.5) / 12
        diamond(bm, 1.32 * math.cos(a), 1.32 * math.sin(a), 0.09, a)
    objs.append(finish_mesh(bm, name + "_Inner", glow_mat))

    # Core：中心部
    bm = bmesh.new()
    ring(bm, 1.00, 1.05, 96)
    ring(bm, 0.62, 0.66, 64)
    for i in range(8):
        a = 2 * math.pi * i / 8
        strip(bm, (0.66 * math.cos(a), 0.66 * math.sin(a)),
              (1.00 * math.cos(a), 1.00 * math.sin(a)), 0.035)
    ring(bm, 0.24, 0.30, 48)
    diamond(bm, 0, 0, 0.18, 0)
    diamond(bm, 0, 0, 0.18, math.pi * 0.25)
    objs.append(finish_mesh(bm, name + "_Core", glow_mat))

    # Halo：少し浮いて上下するルーンの輪（高さはプログラム側で動かす）
    bm = bmesh.new()
    ring(bm, 2.20, 2.25, 96)
    ring(bm, 1.85, 1.88, 96)
    for i in range(16):
        a = 2 * math.pi * i / 16
        glyph(bm, 2.03 * math.cos(a), 2.03 * math.sin(a), a, 0.07,
              random.randrange(len(GLYPHS)), 0.025)
    objs.append(finish_mesh(bm, name + "_Halo", glow_mat))

    for o in objs[2:]:
        col.objects.link(o)
    return col, objs


# ------------------------------------------------------------
# 書き出し
# ------------------------------------------------------------
def export_gate(objs, folder):
    path = os.path.join(BASE_DIR, folder, "WarpGateblend.gltf")
    binp = os.path.join(BASE_DIR, folder, "WarpGateblend.bin")

    # 初回だけバックアップ
    for src in (path, binp):
        bak = src.replace("WarpGateblend.", "WarpGateblend_backup.")
        if os.path.exists(src) and not os.path.exists(bak):
            shutil.copy2(src, bak)

    bpy.ops.object.select_all(action="DESELECT")
    for o in objs:
        o.select_set(True)
    bpy.context.view_layer.objects.active = objs[0]

    bpy.ops.export_scene.gltf(
        filepath=path,
        export_format="GLTF_SEPARATE",
        use_selection=True,
        export_yup=True,
        export_apply=True,
        export_materials="EXPORT",
    )
    return path


# 既存の作成物を消してから作り直す
for c in list(bpy.data.collections):
    if c.name.startswith("MagicCircle_"):
        for o in list(c.objects):
            bpy.data.objects.remove(o, do_unlink=True)
        bpy.data.collections.remove(c)

results = []
offset = {"Fine": -4.5, "Next": 4.5}
for name, (folder, color) in GATES.items():
    col, objs = build_gate(name, color)
    results.append(export_gate(objs, folder))
    # 書き出し後、見比べやすいように横にずらす（書き出したデータには影響なし）
    for o in objs:
        o.location.x = offset[name]

print("WarpGate magic circle exported:", results)
