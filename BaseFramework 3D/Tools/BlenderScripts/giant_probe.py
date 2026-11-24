# ジャイアントの blend を調べる(何も保存しない)
import bpy, json, os
OUT = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts\giant_probe_out.json"
res = {"file": bpy.data.filepath, "fps": bpy.context.scene.render.fps}
res["objects"] = [{"n": o.name, "t": o.type, "parent": o.parent.name if o.parent else None,
                   "loc": [round(x,3) for x in o.location], "rot": [round(x,3) for x in o.rotation_euler], "scl": [round(x,3) for x in o.scale],
                   "dim": [round(x,3) for x in o.dimensions], "mods": [m.type for m in getattr(o, "modifiers", [])]} for o in bpy.data.objects]
arm = bpy.data.objects["Armature"]
ad = arm.animation_data
res["cur_action"] = ad.action.name if ad and ad.action else None
res["nla"] = [(t.name, [s.action.name for s in t.strips if s.action]) for t in ad.nla_tracks] if ad else []
res["bones"] = [{"n": pb.name, "p": pb.parent.name if pb.parent else None, "rm": pb.rotation_mode,
                 "c": [c.type for c in pb.constraints],
                 "head": [round(v,3) for v in arm.matrix_world @ arm.data.bones[pb.name].head_local],
                 "tail": [round(v,3) for v in arm.matrix_world @ arm.data.bones[pb.name].tail_local]} for pb in arm.pose.bones]
res["actions"] = []
key = [b for b in ("mixamorigHips","mixamorigHead","mixamorigLeftHand","mixamorigRightHand","mixamorigLeftFoot","mixamorigRightFoot","mixamorigSpine2") if b in arm.pose.bones]
prev = (ad.action, ad.action_slot)
for act in bpy.data.actions:
    d = {"name": act.name, "range": list(act.frame_range), "users": act.users, "fake": act.use_fake_user}
    props = set()
    for L in act.layers:
        for st in L.strips:
            for cb in st.channelbags:
                for fc in cb.fcurves:
                    props.add(fc.data_path.rsplit(".",1)[-1])
    d["props"] = sorted(props)
    ad.action = act
    try: ad.action_slot = act.slots[0]
    except Exception: pass
    track = {}
    a, b = int(act.frame_range[0]), int(act.frame_range[1])
    step = max(1, (b - a) // 20)
    for f in range(a, b + 1, step):
        bpy.context.scene.frame_set(f)
        bpy.context.view_layer.update()
        track[f] = {k[9:]: [round(v, 2) for v in (arm.matrix_world @ arm.pose.bones[k].head)] for k in key}
    d["track"] = track
    res["actions"].append(d)
ad.action = prev[0]
try: ad.action_slot = prev[1]
except Exception: pass
json.dump(res, open(OUT, "w", encoding="utf-8"), ensure_ascii=False, indent=1)
print("GIANT PROBE DONE")
