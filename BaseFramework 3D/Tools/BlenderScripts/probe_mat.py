import bpy, json
S = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts"
res = {"images": [(i.name, i.filepath, i.size[:], i.packed_file is not None, i.colorspace_settings.name) for i in bpy.data.images]}
res["objects"] = []
for o in bpy.data.objects:
    d = {"name": o.name, "type": o.type, "parent": o.parent.name if o.parent else None, "scale": list(o.scale),
         "loc": list(o.location), "hide": o.hide_get() if o.name in bpy.context.view_layer.objects else None,
         "hide_render": o.hide_render, "collections": [c.name for c in o.users_collection]}
    if o.type == 'MESH':
        d["mats"] = [s.material.name if s.material else None for s in o.material_slots]
        d["mods"] = [(m.type, getattr(getattr(m, "object", None), "name", None)) for m in o.modifiers]
        d["uv"] = [u.name for u in o.data.uv_layers]
        d["verts"] = len(o.data.vertices)
    res["objects"].append(d)
res["materials"] = {}
for m in bpy.data.materials:
    if not m.use_nodes: res["materials"][m.name] = "no nodes"; continue
    nodes = [(n.name, n.type, getattr(getattr(n, "image", None), "name", None)) for n in m.node_tree.nodes]
    links = [(l.from_node.name, l.from_socket.name, l.to_node.name, l.to_socket.name) for l in m.node_tree.links]
    res["materials"][m.name] = {"nodes": nodes, "links": links, "users": m.users}
res["collections"] = [(c.name, c.hide_render, [o.name for o in c.objects]) for c in bpy.data.collections]
res["vl_excluded"] = []
def walk(lc):
    for ch in lc.children:
        res["vl_excluded"].append((ch.name, ch.exclude, ch.hide_viewport))
        walk(ch)
walk(bpy.context.view_layer.layer_collection)
json.dump(res, open(S + r"\probe_mat_out.json", "w"), indent=1, ensure_ascii=False)
print("PROBE MAT DONE")
