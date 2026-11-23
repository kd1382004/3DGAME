# 攻撃の予備動作(振りかぶり)を分かりやすくする
#   振りかぶりを長く・大きくして、頂点で少し溜めてから振り下ろす。始めと終わりの姿勢は変えない
#   元のアクションは *_before_tele.blend から読むので、何度流しても同じ結果になる
#   使い方：TELE = "goblin" か "golem" を入れてから exec
import bpy, sys, importlib, math, json, os
from mathutils import Quaternion, Vector
S = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts"
if S not in sys.path: sys.path.append(S)
import anim_lib as L
importlib.reload(L)
ARM = L.ARM
BLEND_DIR = os.path.dirname(bpy.data.filepath)

CFG = {
    "goblin": {
        "backup": "goblin-FINAL_before_tele.blend",
        "game": r"C:\GitHub\3DGAME\BaseFramework 3D\Asset\Models\Character\Enemy\Goblin\goblin-FINAL.gltf",
        "eff": ["thumb_2_L", "hand_R", "hand_L"],
        "idle": "Idle",
        # 名前: (振りかぶりの伸ばし, 溜めのフレーム数, 振りかぶりの大きさ+)
        "acts": {"Attack.000": (1.4, 5, 0.3), "Attack.001": (1.5, 4, 0.35), "Attack.002": (1.5, 4, 0.35),
                 "Attack.003": (1.1, 5, 0.25), "Attack.004": (1.15, 4, 0.3), "Attack.005": (1.15, 4, 0.3)},
    },
    "golem": {
        "backup": "golem_clean_before_tele.blend",
        "game": r"C:\GitHub\3DGAME\BaseFramework 3D\Asset\Models\Character\Enemy\Golem\golem_clean.gltf",
        "eff": ["hand.R", "hand.L", "foot.R"],
        "idle": "walk",
        "acts": {"smash": (1.0, 14, 0.15), "SmashQuick": (1.25, 12, 0.2), "SwipeR": (1.3, 12, 0.25),
                 "SwipeL": (1.3, 12, 0.25), "Stomp": (1.2, 12, 0.2)},
    },
}[TELE]

if bpy.context.object and bpy.context.object.mode != 'OBJECT':
    bpy.ops.object.mode_set(mode='OBJECT')

# ---- 元のアクションを読み込む(名前の後ろに _src) ----
import re
names = tuple(str(k) for k in CFG["acts"].keys())
for a in [a for a in bpy.data.actions if a.name.endswith("_src") or any(re.fullmatch(re.escape(n) + r"\.\d{3}", a.name) for n in names)]:
    bpy.data.actions.remove(a)
with bpy.data.libraries.load(os.path.join(BLEND_DIR, CFG["backup"]), link=False) as (src_data, dst_data):
    dst_data.actions = list(names)
src = {}
loaded = list(dst_data.actions)
for i, a in enumerate(loaded):
    n = names[i]
    a.name = n + "_src"
    src[n] = a

def ext(p0, p, k):
    """p0(始めの姿勢)から p への動きを k 倍にした姿勢"""
    out = {}
    for b, v in p.items():
        a = p0[b]
        d = a["rot"].conjugated() @ v["rot"]
        if d.w < 0: d.negate()
        axis, ang = d.to_axis_angle()
        out[b] = {"loc": a["loc"] + (v["loc"] - a["loc"]) * k,
                  "rot": a["rot"] @ Quaternion(axis, ang * k),
                  "scale": v["scale"].copy()}
    return out

def smooth(t):
    t = max(0.0, min(1.0, t)); return t * t * (3 - 2 * t)

log = {}
for name, (stretch, hold, ex) in CFG["acts"].items():
    act = src[name]
    last = int(round(act.frame_range[1]))
    m = L.fc_map(act)
    # 効果を出す部位(手・足)の速さから、当たる瞬間(一番速い)と振りかぶりの頂点(その直前で一番遅い)を探す
    pos = []
    for f in range(last + 1):
        pos.append(L.bone_positions(act, f, CFG["eff"]))
    speed = [0.0] + [max((Vector(pos[f][e]) - Vector(pos[f - 1][e])).length for e in CFG["eff"]) for f in range(1, last + 1)]
    impact = max(range(1, last + 1), key=lambda f: speed[f])
    # 頂点 = 当たる瞬間の位置から一番離れている(一番引いている)フレーム
    lo = max(1, int(impact * 0.3))
    hit_pos = {e: Vector(pos[impact][e]) for e in CFG["eff"]}
    apex = max(range(lo, impact), key=lambda f: max((Vector(pos[f][e]) - hit_pos[e]).length for e in CFG["eff"])) if impact - lo >= 1 else max(1, impact - 1)
    p0 = L.sample(m, 0.0)
    nw = max(apex + 1, int(round(apex * stretch)) + 1)        # 振りかぶりのフレーム数
    frames = []
    for i in range(nw):                                      # 振りかぶり：ゆっくり・大きく
        u = i / (nw - 1)
        frames.append(ext(p0, L.sample(m, apex * u), 1 + ex * smooth(u)))
    for i in range(1, hold + 1):                             # 溜め：頂点で少しだけ引き絞る
        frames.append(ext(p0, L.sample(m, float(apex)), 1 + ex + ex * 0.15 * smooth(i / hold)))
    dk = max(1, impact - apex)
    for f in range(apex + 1, last + 1):                      # 振り下ろし：当たる瞬間までに元の動きに戻す
        k = 1 + ex * 1.15 * (1 - smooth((f - apex) / dk))
        frames.append(ext(p0, L.sample(m, float(f)), k))
    L.write_action(name, frames)
    new_last = len(frames) - 1
    log[name] = {"old_last": last, "new_last": new_last, "apex": apex, "impact": impact,
                 "apex_new": nw - 1, "hold": hold}
    print(name, log[name])

for a in src.values():
    bpy.data.actions.remove(a)

ad = ARM.animation_data
ad.action = bpy.data.actions[CFG["idle"]]
try: ad.action_slot = ad.action.slots[0]
except Exception: pass
bpy.context.scene.frame_set(0)
bpy.ops.wm.save_mainfile()
bpy.ops.export_scene.gltf(filepath=S + r"\check\%s_tele.gltf" % TELE, export_format='GLTF_SEPARATE')
bpy.ops.export_scene.gltf(filepath=CFG["game"], export_format='GLTF_SEPARATE')
json.dump(log, open(S + r"\tele_%s_out.json" % TELE, "w", encoding="utf-8"), indent=1)
print("TELE DONE", TELE)
