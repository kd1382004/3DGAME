# Restore the armature object transform used when the game glTF was exported,
# keeping every bone and mesh where it is in the world.
import bpy, json
from mathutils import Matrix, Quaternion, Vector
S = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts"
GAME_LOC = Vector((0.08839815855026245, -1.1986784934997559, -0.17220091819763184))
GAME_ROT = Quaternion((0.02337798662483692, 0.0, 0.0, -0.9997267127037048))
GAME_SCL = Vector((0.4169665575027466, 0.5658991932868958, 0.3704414665699005))
arm = bpy.data.objects["Armature"]
info = {"before": [list(r) for r in arm.matrix_world]}
M = Matrix.LocRotScale(GAME_LOC, GAME_ROT, GAME_SCL)
cur = arm.matrix_world.copy()
# children keep their world placement
kids = [o for o in bpy.data.objects if o.parent == arm]
kid_world = {o.name: o.matrix_world.copy() for o in kids}
info["children"] = [(o.name, o.parent_type, o.parent_bone) for o in kids]
info["skinned"] = [o.name for o in bpy.data.objects if o.type == 'MESH' and any(m.type == 'ARMATURE' and m.object == arm for m in o.modifiers)]
# move bone rest data from the current space into the game space
bpy.context.view_layer.objects.active = arm
if bpy.context.object and bpy.context.object.mode != 'OBJECT':
    bpy.ops.object.mode_set(mode='OBJECT')
arm.data.transform(M.inverted() @ cur)
arm.matrix_world = M
bpy.context.view_layer.update()
for o in kids:
    o.matrix_world = kid_world[o.name]
bpy.context.view_layer.update()
info["after"] = [list(r) for r in arm.matrix_world]
json.dump(info, open(S + r"\fix_out.json", "w"), indent=1)
print("FIX DONE")
