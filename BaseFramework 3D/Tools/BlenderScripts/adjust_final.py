# Turn the goblin 180 degrees (same facing as the old game model) and resize it.
import bpy, math
from mathutils import Matrix
FACTOR = 0.370          # 身長 約9.57 → 約3.54 (Rogue 3.08 の約1.15倍)
M = Matrix.Rotation(math.radians(180), 4, 'Z') @ Matrix.Scale(FACTOR, 4)
for o in bpy.data.objects:
    if o.type in ('CAMERA', 'LIGHT') or o.parent is not None:
        continue
    o.matrix_world = M @ o.matrix_world
bpy.context.view_layer.update()
bpy.ops.wm.save_mainfile()
S = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts"
bpy.ops.export_scene.gltf(filepath=S + r"\check\goblin_rebuilt2.gltf", export_format='GLTF_SEPARATE')
print("ADJUST DONE")
