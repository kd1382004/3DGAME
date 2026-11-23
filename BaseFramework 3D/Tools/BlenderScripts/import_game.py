# Load the game's working goblin glTF into a fresh file and save it as goblin-GAME.blend.
import bpy, json
S = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts"
SRC = r"C:\GitHub\3DGAME\BaseFramework 3D\Asset\Models\Character\Enemy\Goblin\goblin-FINAL.gltf"
DST = r"C:\GitHub\3DGAME\BaseFramework 3D\Asset\Blender\Enemy\Goblin\goblin-GAME.blend"
bpy.ops.wm.read_homefile(use_empty=True)
bpy.context.scene.render.fps = 24
bpy.ops.import_scene.gltf(filepath=SRC)
arm = [o for o in bpy.data.objects if o.type == 'ARMATURE'][0]
arm.name = "Armature"
for a in bpy.data.actions:
    a.use_fake_user = True
ad = arm.animation_data
info = {"arm": arm.name, "loc": list(arm.location), "scale": list(arm.scale),
        "actions": [(a.name, list(a.frame_range)) for a in bpy.data.actions],
        "nla": [t.name for t in ad.nla_tracks] if ad else [],
        "objects": [(o.name, o.type) for o in bpy.data.objects],
        "rotmodes": sorted(set(pb.rotation_mode for pb in arm.pose.bones)),
        "constraints": sum(len(pb.constraints) for pb in arm.pose.bones)}
if ad:
    for t in list(ad.nla_tracks):
        ad.nla_tracks.remove(t)
    idle = bpy.data.actions.get("Idle")
    if idle:
        ad.action = idle
bpy.ops.wm.save_as_mainfile(filepath=DST)
json.dump(info, open(S + r"\import_out.json", "w"), indent=1)
print("IMPORT DONE")
