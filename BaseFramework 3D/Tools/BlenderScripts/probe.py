import bpy, json, os
OUT = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts\probe_out.json"
res = {"version": bpy.app.version_string, "file": bpy.data.filepath}
res["scene_fps"] = bpy.context.scene.render.fps
res["objects"] = [(o.name, o.type) for o in bpy.data.objects]
arms = [o for o in bpy.data.objects if o.type == 'ARMATURE']
res["armatures"] = []
for a in arms:
    ad = {"name": a.name, "action": a.animation_data.action.name if a.animation_data and a.animation_data.action else None}
    if a.animation_data:
        ad["slot"] = getattr(a.animation_data, "action_slot", None) and a.animation_data.action_slot.identifier
        ad["nla"] = [t.name for t in a.animation_data.nla_tracks]
    ad["bones"] = []
    for pb in a.pose.bones:
        ad["bones"].append({"n": pb.name, "p": pb.parent.name if pb.parent else None, "rot": pb.rotation_mode,
                            "c": [(c.type, getattr(c, "subtarget", "")) for c in pb.constraints],
                            "deform": a.data.bones[pb.name].use_deform})
    res["armatures"].append(ad)
res["actions"] = []
for act in bpy.data.actions:
    d = {"name": act.name, "range": list(act.frame_range), "users": act.users, "fake": act.use_fake_user,
         "slots": [s.identifier for s in act.slots], "layers": len(act.layers)}
    paths = []
    try:
        for L in act.layers:
            for st in L.strips:
                for cb in st.channelbags:
                    for fc in cb.fcurves:
                        paths.append((fc.data_path, fc.array_index, len(fc.keyframe_points)))
    except Exception as e:
        d["err"] = str(e)
    d["n_fc"] = len(paths)
    d["paths_sample"] = paths[:400] if act.name in ("Attack.001",) else sorted(set(p[0].split('"')[1] if '"' in p[0] else p[0] for p in paths))
    res["actions"].append(d)
os.makedirs(os.path.dirname(OUT), exist_ok=True)
with open(OUT, "w", encoding="utf-8") as f:
    json.dump(res, f, ensure_ascii=False, indent=1)
print("PROBE DONE", OUT)
