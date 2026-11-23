# Builds new goblin attack animations from the existing Attack.000/001/002.
import bpy, json, importlib, sys
S = r"C:\GitHub\3DGAME\BaseFramework 3D\Tools\BlenderScripts"
if S not in sys.path: sys.path.append(S)
import anim_lib as L
importlib.reload(L)

SLAM  = L.fc_map(bpy.data.actions["Attack.000"])   # 両手叩きつけ(0-25, 当たり 14前後)
SLAPR = L.fc_map(bpy.data.actions["Attack.001"])   # 右手ビンタ(0-16)
SLAPL = L.fc_map(bpy.data.actions["Attack.002"])   # 左手ビンタ(0-16)

def seg(m, a, b, speed=1.0):
    """Frames from source m between a..b (inclusive), played at speed."""
    out, t = [], float(a)
    while t <= b + 1e-6:
        out.append(L.sample(m, t)); t += speed
    return out

def join(frames, nxt, blend):
    """Append nxt to frames, smoothly blending over `blend` frames."""
    if frames and blend > 0:
        a, b = frames[-1], nxt[0]
        for i in range(1, blend + 1):
            frames.append(L.blend_pose(a, b, L.smooth(i / (blend + 1))))
    frames.extend(nxt)
    return frames

def warp_seg(m, a, b, frames_out):
    """Play source a..b stretched to frames_out frames (ease-in so the build-up holds)."""
    out = []
    for i in range(frames_out):
        u = i / (frames_out - 1)
        out.append(L.sample(m, a + (b - a) * (u ** 1.6)))
    return out

made = {}
# Attack.003 左右連続ビンタ: 右ビンタ → 左ビンタ
f = seg(SLAPR, 0, 10)
f = join(f, seg(SLAPL, 3, 16), 3)
made["Attack.003"] = f
# Attack.004 二連叩きつけ: 叩きつけ → 少し戻して もう一度
f = seg(SLAM, 0, 18)
f = join(f, seg(SLAM, 6, 25), 4)
made["Attack.004"] = f
# Attack.005 溜め叩きつけ: 振りかぶりをゆっくり溜めてから一気に
f = warp_seg(SLAM, 0, 11, 30)
f.extend(seg(SLAM, 12, 25))
made["Attack.005"] = f
# Attack.006 ひっかき → 叩きつけ
f = seg(SLAPL, 0, 10)
f = join(f, seg(SLAM, 4, 25), 3)
made["Attack.006"] = f
# Attack.007 三連撃: 右ビンタ → 左ビンタ → 叩きつけ
f = seg(SLAPR, 0, 10)
f = join(f, seg(SLAPL, 3, 10), 3)
f = join(f, seg(SLAM, 4, 25), 3)
made["Attack.007"] = f
# Attack.008 素早い叩きつけ: 振りかぶりを短くして速く
f = seg(SLAM, 4, 25, speed=1.35)
made["Attack.008"] = f

ad = L.ARM.animation_data
orig_act, orig_frame = ad.action, bpy.context.scene.frame_current
report = {}
names = ["hand_L", "hand_R", "head", "spine1"]
for name, frames in made.items():
    act = L.write_action(name, frames)
    pos = [L.bone_positions(act, i, names) for i in range(len(frames))]
    jumps = []
    for i in range(1, len(pos)):
        jumps.append(max(sum((pos[i][n][k] - pos[i-1][n][k]) ** 2 for k in range(3)) ** 0.5 for n in names))
    report[name] = {"frames": len(frames), "max_jump": max(jumps), "jumps": [round(j, 2) for j in jumps]}
ad.action = orig_act
bpy.context.scene.frame_set(orig_frame)
json.dump(report, open(S + r"\make_out.json", "w"), indent=1)
print("MAKE DONE", {k: v["frames"] for k, v in report.items()})
