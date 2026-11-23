# ゴブリン：浮いていた首飾り(Neck)を体と同じ位置・大きさに戻し、足元の板(Plane.003)を消して書き出す
import bpy
GAME = r"C:\GitHub\3DGAME\BaseFramework 3D\Asset\Models\Character\Enemy\Goblin\goblin-FINAL.gltf"
if bpy.context.object and bpy.context.object.mode != 'OBJECT':
    bpy.ops.object.mode_set(mode='OBJECT')
neck = bpy.data.objects["Neck"]
body = bpy.data.objects["Body"]
neck.matrix_world = body.matrix_world.copy()
plane = bpy.data.objects.get("Plane.003")
if plane:
    bpy.data.objects.remove(plane, do_unlink=True)
bpy.context.view_layer.update()
bpy.ops.wm.save_mainfile()
bpy.ops.export_scene.gltf(filepath=GAME, export_format='GLTF_SEPARATE')
print("NECK FIX DONE")
