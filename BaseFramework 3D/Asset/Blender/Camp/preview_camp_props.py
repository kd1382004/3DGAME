# 作ったキャンプの小物を、確認用のシーン「CampPreview」に並べて表示する(確認が終わったら remove_preview() で消す)
import bpy, os, math

ROOT = r"C:\GitHub\3DGAME\BaseFramework 3D\Asset\Blender\Camp"
NAMES = ["Campfire", "Tent", "WeaponRack", "MarketStall", "Barrel", "Crate", "Sack", "LogSeat", "Stump", "Journal"]
POS = {
    "Tent": (-9, 6), "MarketStall": (-2, 6), "WeaponRack": (4, 6),
    "Campfire": (-6, 0), "LogSeat": (-2.5, 0), "Stump": (0.8, 0), "Journal": (0.8, 0, 0.7),
    "Barrel": (3, 0), "Crate": (5, 0), "Sack": (7, 0),
}


def remove_preview():
    sc = bpy.data.scenes.get("CampPreview")
    if not sc:
        return
    objs = list(sc.objects)
    meshes = set(o.data for o in objs if o.type == "MESH")
    for o in objs:
        bpy.data.objects.remove(o, do_unlink=True)
    for m in meshes:
        if m.users == 0:
            bpy.data.meshes.remove(m)
    bpy.data.scenes.remove(sc)
    for m in list(bpy.data.materials):
        if m.users == 0 and m.name.startswith("Camp_"):
            bpy.data.materials.remove(m)
    for im in list(bpy.data.images):
        if im.users == 0:
            bpy.data.images.remove(im)


def show_preview():
    remove_preview()
    sc = bpy.data.scenes.new("CampPreview")
    bpy.context.window.scene = sc
    for name in NAMES:
        before = set(bpy.data.objects)
        bpy.ops.import_scene.gltf(filepath=os.path.join(ROOT, name, name + ".gltf"))
        new = [o for o in bpy.data.objects if o not in before]
        p = POS[name]
        for o in new:
            o.location = (p[0], p[1], p[2] if len(p) > 2 else 0.0)
    # 地面と明かり
    bpy.ops.mesh.primitive_plane_add(size=40, location=(0, 2, 0))
    sun = bpy.data.lights.new("PreviewSun", "SUN")
    sun.energy = 3.0
    so = bpy.data.objects.new("PreviewSun", sun)
    so.rotation_euler = (math.radians(50), 0, math.radians(30))
    sc.collection.objects.link(so)
    # 3Dビューをマテリアルプレビューにして全体を映す
    for area in bpy.context.window.screen.areas:
        if area.type == "VIEW_3D":
            for sp in area.spaces:
                if sp.type == "VIEW_3D":
                    sp.shading.type = "MATERIAL"
                    sp.overlay.show_overlays = False
            region = next(r for r in area.regions if r.type == "WINDOW")
            with bpy.context.temp_override(area=area, region=region):
                bpy.ops.view3d.view_all()


show_preview()
