# Replace combined attacks with single-move attacks built from Attack.000/001/002.
import bpy, json, importlib, sys
S = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts"
if S not in sys.path: sys.path.append(S)
import anim_lib as L
importlib.reload(L)

for a in [a for a in bpy.data.actions if a.name.startswith("Attack.") and a.name not in ("Attack.000", "Attack.001", "Attack.002")]:
    bpy.data.actions.remove(a)

SLAM  = L.fc_map(bpy.data.actions["Attack.000"])   # 両手叩きつけ 0-25 (振り下ろし 13-15)
SLAPR = L.fc_map(bpy.data.actions["Attack.001"])   # 右ビンタ 0-16 (振り 6-8)
SLAPL = L.fc_map(bpy.data.actions["Attack.002"])   # 左ビンタ 0-16 (振り 6-8)

def seg(m, a, b):
    return [L.sample(m, float(t)) for t in range(a, b + 1)]

def warp(m, a, b, n):
    """Play a..b stretched over n frames, easing in so the wind-up holds."""
    return [L.sample(m, a + (b - a) * ((i / (n - 1)) ** 1.6)) for i in range(n)]

made = {
    "Attack.003": warp(SLAM, 0, 11, 30) + seg(SLAM, 12, 25),    # 溜め叩きつけ
    "Attack.004": warp(SLAPR, 0, 5, 14) + seg(SLAPR, 6, 16),    # 溜め右ビンタ
    "Attack.005": warp(SLAPL, 0, 5, 14) + seg(SLAPL, 6, 16),    # 溜め左ビンタ
}
ad = L.ARM.animation_data
orig = ad.action
for name, frames in made.items():
    L.write_action(name, frames)
ad.action = bpy.data.actions["Idle"]
try: ad.action_slot = ad.action.slots[0]
except Exception: pass
bpy.ops.wm.save_mainfile()
bpy.ops.export_scene.gltf(filepath=S + r"\check\goblin_singles.gltf", export_format='GLTF_SEPARATE')
print("SINGLES DONE", {k: len(v) for k, v in made.items()})
