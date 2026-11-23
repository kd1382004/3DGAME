import bpy, json
from mathutils import Vector
OUT = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts\neck_probe_out.json"
res = {}
dg = bpy.context.evaluated_depsgraph_get()
for o in bpy.data.objects:
    if o.type != 'MESH': continue
    d = {"parent": o.parent.name if o.parent else None, "ptype": o.parent_type, "pbone": o.parent_bone,
         "mw": [list(r) for r in o.matrix_world], "mpi": [list(r) for r in o.matrix_parent_inverse],
         "mods": [(m.type, getattr(m, "object", None) and m.object.name) for m in o.modifiers],
         "vg": [g.name for g in o.vertex_groups][:30], "nvg": len(o.vertex_groups), "nv": len(o.data.vertices),
         "hide": o.hide_get(), "hide_render": o.hide_render}
    ev = o.evaluated_get(dg)
    me = ev.to_mesh()
    ws = [ev.matrix_world @ v.co for v in me.vertices]
    if ws:
        d["eval_min"] = [min(w[i] for w in ws) for i in range(3)]
        d["eval_max"] = [max(w[i] for w in ws) for i in range(3)]
    ev.to_mesh_clear()
    # 頂点グループの重みが一番多い骨
    cnt = {}
    for v in o.data.vertices:
        for g in v.groups:
            n = o.vertex_groups[g.group].name
            cnt[n] = cnt.get(n, 0) + g.weight
    d["top_groups"] = sorted(cnt.items(), key=lambda x: -x[1])[:6]
    res[o.name] = d
arm = bpy.data.objects["Armature"]
res["_arm"] = {"mw": [list(r) for r in arm.matrix_world], "pose_pos": arm.data.pose_position,
               "action": arm.animation_data.action.name if arm.animation_data and arm.animation_data.action else None}
json.dump(res, open(OUT, "w", encoding="utf-8"), indent=1, default=str)
print("NECK PROBE DONE")
