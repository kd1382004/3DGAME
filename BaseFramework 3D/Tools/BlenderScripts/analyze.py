import bpy, json, importlib, sys
S = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts"
if S not in sys.path: sys.path.append(S)
import anim_lib as L
importlib.reload(L)
res = {}
names = ["hand_L", "hand_R", "head", "spine4", "foot_L", "foot_R", "spine1"]
ad = L.ARM.animation_data
orig_act, orig_frame = ad.action, bpy.context.scene.frame_current
# rest positions
res["rest"] = {n: tuple(L.ARM.data.bones[n].head_local) for n in names}
for an in ["Attack.000", "Attack.001", "Attack.002"]:
    act = bpy.data.actions[an]
    a, b = [int(x) for x in act.frame_range]
    res[an] = [L.bone_positions(act, f, names) for f in range(a, b + 1)]
# mirror test on Attack.001
src = bpy.data.actions["Attack.001"]
m = L.fc_map(src)
frames = [L.mirror_pose(L.sample(m, f)) for f in range(0, 17)]
tmp = L.write_action("ZZ_MirrorTest", frames)
res["mirror"] = [L.bone_positions(tmp, f, names) for f in range(0, 17)]
# copy test (no mirror) to verify sampling/writing roundtrip
frames2 = [L.sample(m, f) for f in range(0, 17)]
tmp2 = L.write_action("ZZ_CopyTest", frames2)
res["copy"] = [L.bone_positions(tmp2, f, names) for f in range(0, 17)]
bpy.data.actions.remove(tmp); bpy.data.actions.remove(tmp2)
ad.action = orig_act
bpy.context.scene.frame_set(orig_frame)
json.dump(res, open(S + r"\analyze_out.json", "w"), indent=0)
print("ANALYZE DONE")
