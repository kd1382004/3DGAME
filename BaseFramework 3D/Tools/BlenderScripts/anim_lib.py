# Helper functions for building new actions from existing ones (baked per frame).
import bpy, math, json
from mathutils import Quaternion, Vector

ARM = bpy.data.objects["Armature"]
CHANNELS = ("location", "rotation_quaternion", "rotation_axis_angle", "rotation_euler", "scale")
SIZES = {"location": 3, "rotation_quaternion": 4, "rotation_axis_angle": 4, "rotation_euler": 3, "scale": 3}

def iter_fcurves(act):
    for L in act.layers:
        for st in L.strips:
            for cb in st.channelbags:
                for fc in cb.fcurves:
                    yield fc

def fc_map(act):
    m = {}
    for fc in iter_fcurves(act):
        if not fc.data_path.startswith('pose.bones["'):
            continue
        bone = fc.data_path.split('"')[1]
        prop = fc.data_path.rsplit(".", 1)[1]
        m[(bone, prop, fc.array_index)] = fc
    return m

def default_val(prop, i):
    if prop == "scale": return 1.0
    if prop == "rotation_quaternion": return 1.0 if i == 0 else 0.0
    if prop == "rotation_axis_angle": return 1.0 if i == 2 else 0.0
    return 0.0

def sample(act_map, t):
    """Return {bone: {"loc":Vector,"rot":Quaternion,"scale":Vector}} at time t (frames)."""
    pose = {}
    for pb in ARM.pose.bones:
        b = pb.name
        def get(prop, n):
            return [act_map[(b, prop, i)].evaluate(t) if (b, prop, i) in act_map else default_val(prop, i) for i in range(n)]
        loc = Vector(get("location", 3))
        scl = Vector(get("scale", 3))
        if pb.rotation_mode == "QUATERNION":
            q = Quaternion(get("rotation_quaternion", 4))
        elif pb.rotation_mode == "AXIS_ANGLE":
            aa = get("rotation_axis_angle", 4)
            q = Quaternion(Vector(aa[1:4]).normalized() if Vector(aa[1:4]).length > 1e-8 else Vector((0, 1, 0)), aa[0])
        else:
            q = Quaternion((1, 0, 0, 0))
        q.normalize()
        pose[b] = {"loc": loc, "rot": q, "scale": scl}
    return pose

def mirror_name(n):
    if n.endswith("_L"): return n[:-2] + "_R"
    if n.endswith("_R"): return n[:-2] + "_L"
    if "_L." in n: return n.replace("_L.", "_R.")
    if "_R." in n: return n.replace("_R.", "_L.")
    return n

def mirror_pose(pose):
    out = {}
    for b, v in pose.items():
        m = mirror_name(b)
        q = v["rot"]
        out[m] = {"loc": Vector((-v["loc"].x, v["loc"].y, v["loc"].z)),
                  "rot": Quaternion((q.w, q.x, -q.y, -q.z)),
                  "scale": v["scale"].copy()}
    return out

def blend_pose(a, b, t):
    out = {}
    for k in a:
        qa, qb = a[k]["rot"], b[k]["rot"]
        out[k] = {"loc": a[k]["loc"].lerp(b[k]["loc"], t),
                  "rot": qa.slerp(qb, t),
                  "scale": a[k]["scale"].lerp(b[k]["scale"], t)}
    return out

def smooth(t):
    return t * t * (3 - 2 * t)

def write_action(name, frames):
    """frames: list of pose dicts, one per frame starting at 0. Creates/overwrites the action."""
    old = bpy.data.actions.get(name)
    if old:
        bpy.data.actions.remove(old)
    act = bpy.data.actions.new(name)
    act.use_fake_user = True
    slot = act.slots.new(id_type='OBJECT', name="Legacy Slot")
    ad = ARM.animation_data or ARM.animation_data_create()
    prev_act, prev_slot = ad.action, ad.action_slot
    ad.action = act
    ad.action_slot = slot
    n = len(frames)
    for pb in ARM.pose.bones:
        b = pb.name
        grp = b
        def put(prop, idx, values):
            fc = act.fcurve_ensure_for_datablock(ARM, 'pose.bones["%s"].%s' % (b, prop), index=idx, group_name=grp)
            fc.keyframe_points.add(n)
            co = []
            for f, v in enumerate(values):
                co += [float(f), float(v)]
            fc.keyframe_points.foreach_set("co", co)
            for kp in fc.keyframe_points:
                kp.interpolation = 'LINEAR'
            fc.update()
        for i in range(3):
            put("location", i, [fr[b]["loc"][i] for fr in frames])
            put("scale", i, [fr[b]["scale"][i] for fr in frames])
        # keep quaternion sign continuous
        qs = []
        for fr in frames:
            q = fr[b]["rot"].copy()
            if qs and qs[-1].dot(q) < 0:
                q.negate()
            qs.append(q)
        if pb.rotation_mode == "AXIS_ANGLE":
            aas = []
            for q in qs:
                axis, ang = q.to_axis_angle()
                aas.append((ang, axis.x, axis.y, axis.z))
            for i in range(4):
                put("rotation_axis_angle", i, [a[i] for a in aas])
        else:
            for i in range(4):
                put("rotation_quaternion", i, [q[i] for q in qs])
    act.frame_range = (0, n - 1)
    act.use_frame_range = True
    ad.action = prev_act
    if prev_slot:
        try: ad.action_slot = prev_slot
        except Exception: pass
    return act

def bone_positions(act, frame, names):
    ad = ARM.animation_data
    ad.action = act
    try:
        ad.action_slot = act.slots[0]
    except Exception:
        pass
    bpy.context.scene.frame_set(int(frame))
    bpy.context.view_layer.update()
    return {n: tuple(ARM.pose.bones[n].head) for n in names}
