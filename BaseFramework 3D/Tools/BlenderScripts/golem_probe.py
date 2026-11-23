# ゴーレムの blend を調べる(何も保存しない)
import bpy, json, os
OUT = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts\golem_probe_out.json"
res = {"file": bpy.data.filepath, "fps": bpy.context.scene.render.fps}
res["objects"] = [{"n": o.name, "t": o.type, "parent": o.parent.name if o.parent else None,
                   "loc": list(o.location), "rot": list(o.rotation_euler), "scl": list(o.scale),
                   "dim": list(o.dimensions), "mods": [m.type for m in getattr(o, "modifiers", [])]} for o in bpy.data.objects]
arm = [o for o in bpy.data.objects if o.type == 'ARMATURE'][0]
res["arm"] = arm.name
ad = arm.animation_data
res["cur_action"] = ad.action.name if ad and ad.action else None
res["bones"] = [{"n": pb.name, "p": pb.parent.name if pb.parent else None, "rm": pb.rotation_mode,
                 "c": [(c.type, getattr(c, "subtarget", ""), getattr(c, "pole_subtarget", ""), getattr(c, "chain_count", 0)) for c in pb.constraints],
                 "deform": arm.data.bones[pb.name].use_deform,
                 "head": list(arm.matrix_world @ arm.data.bones[pb.name].head_local),
                 "tail": list(arm.matrix_world @ arm.data.bones[pb.name].tail_local)} for pb in arm.pose.bones]
res["actions"] = []
key = ["hand.L", "hand.R", "IKhand.L", "IKhand.R", "hips", "root", "foot.L", "foot.R", "IKheel.L", "IKheel.R", "head", "spine1", "ribs"]
for act in bpy.data.actions:
    d = {"name": act.name, "range": list(act.frame_range), "slots": [s.identifier for s in act.slots]}
    bones = set()
    for L in act.layers:
        for st in L.strips:
            for cb in st.channelbags:
                for fc in cb.fcurves:
                    if '"' in fc.data_path: bones.add(fc.data_path.split('"')[1])
    d["bones"] = sorted(bones)
    ad.action = act
    try: ad.action_slot = act.slots[0]
    except Exception: pass
    track = {}
    a, b = int(act.frame_range[0]), int(act.frame_range[1])
    step = max(1, (b - a) // 24)
    for f in range(a, b + 1, step):
        bpy.context.scene.frame_set(f)
        bpy.context.view_layer.update()
        track[f] = {k: [round(v, 3) for v in (arm.matrix_world @ arm.pose.bones[k].head)] for k in key if k in arm.pose.bones}
    d["track"] = track
    res["actions"].append(d)
os.makedirs(os.path.dirname(OUT), exist_ok=True)
json.dump(res, open(OUT, "w", encoding="utf-8"), ensure_ascii=False, indent=1)
print("GOLEM PROBE DONE")
