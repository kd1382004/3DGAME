import bpy, json
S = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts"
DST = r"C:\GitHub\3DGAME\BaseFramework 3D\Asset\Blender\Enemy\Goblin\goblin-GAME.blend"
# remove leftovers from the first (failed) attempt: unlinked armature
for o in list(bpy.data.objects):
    if not o.users_collection:
        bpy.data.objects.remove(o)
arms = [o for o in bpy.data.objects if o.type == 'ARMATURE']
arm = arms[0]
arm.name = "Armature"
for a in bpy.data.actions:
    a.use_fake_user = True
ad = arm.animation_data
info = {"arms": [o.name for o in arms], "loc": list(arm.location), "rot": list(arm.rotation_quaternion) if arm.rotation_mode == 'QUATERNION' else list(arm.rotation_euler),
        "scale": list(arm.scale), "actions": [(a.name, list(a.frame_range), [s.identifier for s in a.slots]) for a in bpy.data.actions],
        "nla": [t.name for t in ad.nla_tracks] if ad else [], "objects": [(o.name, o.type) for o in bpy.data.objects],
        "rotmodes": sorted(set(pb.rotation_mode for pb in arm.pose.bones)),
        "constraints": sum(len(pb.constraints) for pb in arm.pose.bones), "fps": bpy.context.scene.render.fps}
if ad:
    info["active"] = ad.action.name if ad.action else None
    info["slot"] = ad.action_slot.identifier if ad.action_slot else None
    for t in list(ad.nla_tracks):
        ad.nla_tracks.remove(t)
    ad.action = bpy.data.actions["Idle"]
bpy.ops.wm.save_as_mainfile(filepath=DST)
json.dump(info, open(S + r"\import_out.json", "w"), indent=1)
print("FINISH IMPORT DONE")
