import bpy, bmesh, math, os, traceback
from mathutils import Vector

def _run():

    OUT_DIR = r"C:\GitHub\3DGAME\BaseFramework 3D\Asset\Blender\DropCoin"

    R = 0.30        # radius (player is ~3.0 tall)
    T = 0.07        # thickness
    RIM = 0.035     # rim width
    RECESS = 0.012  # face recess depth
    STAR_R, STAR_r, STAR_H = 0.15, 0.065, 0.016

    for o in list(bpy.data.objects): bpy.data.objects.remove(o, do_unlink=True)
    for m in list(bpy.data.meshes): bpy.data.meshes.remove(m)
    for m in list(bpy.data.materials): bpy.data.materials.remove(m)

    me = bpy.data.meshes.new("Coin")
    obj = bpy.data.objects.new("Coin", me)
    bpy.context.scene.collection.objects.link(obj)

    bm = bmesh.new()
    bmesh.ops.create_cone(bm, cap_ends=True, cap_tris=False, segments=64,
                          radius1=R, radius2=R, depth=T, calc_uvs=True)

    # rim + recessed face on both sides
    for s in (1, -1):
        bm.faces.ensure_lookup_table()
        cap = [f for f in bm.faces if f.normal.z * s > 0.9][0]
        bmesh.ops.inset_region(bm, faces=[cap], thickness=RIM, depth=0.0)
        ext = bmesh.ops.extrude_face_region(bm, geom=[cap])
        verts = [e for e in ext["geom"] if isinstance(e, bmesh.types.BMVert)]
        bmesh.ops.translate(bm, verts=verts, vec=Vector((0, 0, -s * RECESS)))
        bmesh.ops.delete(bm, geom=[cap], context='FACES_ONLY')

    # raised star on both sides
    for s in (1, -1):
        z0 = s * (T / 2 - RECESS)
        pts = []
        for i in range(10):
            a = math.pi / 2 + i * math.pi / 5
            r = STAR_R if i % 2 == 0 else STAR_r
            pts.append(bm.verts.new((r * math.cos(a), r * math.sin(a), z0)))
        if s < 0:
            pts.reverse()
        base = bm.faces.new(pts)
        ext = bmesh.ops.extrude_face_region(bm, geom=[base])
        verts = [e for e in ext["geom"] if isinstance(e, bmesh.types.BMVert)]
        bmesh.ops.translate(bm, verts=verts, vec=Vector((0, 0, s * STAR_H)))
        bmesh.ops.delete(bm, geom=[base], context='FACES_ONLY')

    bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
    bm.to_mesh(me)
    bm.free()

    bpy.context.view_layer.objects.active = obj
    obj.select_set(True)

    # bevel edges then smooth by angle
    bev = obj.modifiers.new("Bevel", 'BEVEL')
    bev.width = 0.004
    bev.segments = 2
    bev.limit_method = 'ANGLE'
    bev.harden_normals = False
    bpy.ops.object.modifier_apply(modifier=bev.name)
    try:
        bpy.ops.object.shade_smooth_by_angle(angle=math.radians(35))
    except Exception:
        bpy.ops.object.shade_smooth()

    # stand upright (face toward -Y / front), spin axis = Z (up)
    obj.rotation_euler = (math.radians(90), 0, 0)
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=False)
    # origin (0,0,0) at the bottom of the coin
    SCALE = 1.5
    me.transform(__import__("mathutils").Matrix.Scale(SCALE, 4))
    me.transform(__import__("mathutils").Matrix.Translation((0, 0, R * SCALE)))
    obj.location = (0, 0, 0)

    # gold material
    mat = bpy.data.materials.new("Coin_Gold")
    mat.use_nodes = True
    bsdf = next(n for n in mat.node_tree.nodes if n.type == "BSDF_PRINCIPLED")
    bsdf.inputs["Base Color"].default_value = (1.0, 0.70, 0.18, 1.0)
    bsdf.inputs["Metallic"].default_value = 1.0
    bsdf.inputs["Roughness"].default_value = 0.28
    me.materials.append(mat)

    os.makedirs(OUT_DIR, exist_ok=True)
    bpy.ops.wm.save_as_mainfile(filepath=os.path.join(OUT_DIR, "Coin.blend"))
    bpy.ops.export_scene.gltf(filepath=os.path.join(OUT_DIR, "Coin.gltf"),
                              export_format='GLTF_SEPARATE')
    print("COIN DONE")

try:
    _run()
except Exception:
    traceback.print_exc()
    raise
