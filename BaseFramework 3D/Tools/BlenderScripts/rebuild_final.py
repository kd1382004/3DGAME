# goblin-FINAL.blend: bake mesh transforms, hook up textures, rebuild attacks, resize, save.
import bpy, json, importlib, sys, os
from mathutils import Matrix
S = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts"
if S not in sys.path: sys.path.append(S)
log = {}
TARGET_SCALE = float(os.environ.get("GOB_SCALE", "0.794"))   # 身長 約4.46 → 約3.54 (Rogue 3.08 の約1.15倍)

if bpy.context.object and bpy.context.object.mode != 'OBJECT':
    bpy.ops.object.mode_set(mode='OBJECT')
arm = bpy.data.objects["Armature"]

# 1. スキンメッシュの位置・回転・拡大縮小をメッシュに焼き込む(書き出しでずれないように)
skinned = [o for o in bpy.data.objects if o.type == 'MESH' and any(m.type == 'ARMATURE' for m in o.modifiers)]
log["baked"] = []
for o in skinned:
    if o.data.users > 1:
        o.data = o.data.copy()
    with bpy.context.temp_override(active_object=o, object=o, selected_objects=[o], selected_editable_objects=[o]):
        bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    log["baked"].append(o.name)

# 2. テクスチャとノーマルマップ
blend_dir = os.path.dirname(bpy.data.filepath)
def load(fname, noncolor=False):
    path = os.path.join(blend_dir, fname)
    img = bpy.data.images.load(path, check_existing=True)
    if noncolor:
        img.colorspace_settings.name = 'Non-Color'
    return img
tex = load("gobli-tex.png")
nrm = load("NormalMap.png", noncolor=True)
mat = bpy.data.materials["goblin"]
nt = mat.node_tree
bsdf = [n for n in nt.nodes if n.type == 'BSDF_PRINCIPLED'][0]
for n in [n for n in nt.nodes if n.name in ("GOB_Base", "GOB_Normal", "GOB_NormalMap")]:
    nt.nodes.remove(n)
tb = nt.nodes.new("ShaderNodeTexImage"); tb.name = "GOB_Base"; tb.image = tex; tb.location = (bsdf.location.x - 500, bsdf.location.y + 100)
tn = nt.nodes.new("ShaderNodeTexImage"); tn.name = "GOB_Normal"; tn.image = nrm; tn.location = (bsdf.location.x - 700, bsdf.location.y - 300)
nm = nt.nodes.new("ShaderNodeNormalMap"); nm.name = "GOB_NormalMap"; nm.location = (bsdf.location.x - 300, bsdf.location.y - 300)
nt.links.new(tb.outputs["Color"], bsdf.inputs["Base Color"])
nt.links.new(tn.outputs["Color"], nm.inputs["Color"])
nt.links.new(nm.outputs["Normal"], bsdf.inputs["Normal"])
log["material"] = [(l.from_node.name, l.to_node.name, l.to_socket.name) for l in nt.links]

# 3. 攻撃アニメを作り直す(古い Attack.003 以降は消す)
for a in [a for a in bpy.data.actions if a.name.startswith("Attack.") and a.name not in ("Attack.000", "Attack.001", "Attack.002")]:
    bpy.data.actions.remove(a)
exec(open(S + r"\make_attacks.py", encoding="utf-8").read())

# 4. 全体の大きさを変える(焼き込みはせず、オブジェクトの拡大縮小で)
Sm = Matrix.Scale(TARGET_SCALE, 4)
log["scaled"] = []
for o in bpy.data.objects:
    if o.type in ('CAMERA', 'LIGHT') or o.parent is not None:
        continue
    o.matrix_world = Sm @ o.matrix_world
    log["scaled"].append(o.name)
bpy.context.view_layer.update()

ad = arm.animation_data
ad.action = bpy.data.actions["Idle"]
try: ad.action_slot = ad.action.slots[0]
except Exception: pass
bpy.ops.wm.save_mainfile()
json.dump(log, open(S + r"\rebuild_out.json", "w"), indent=1, ensure_ascii=False)
print("REBUILD DONE")
