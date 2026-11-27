//---- 自分のヘッダー ----
#include "CampLayoutData.h"		//キャンプの配置データ


namespace
{
	//---- Blender の Power(ワット)からゲーム用の値を作る ----
	// 決まった変換式は無いので、松明(色{24,10,2}・半径25)に近い見た目になるよう決めた目安
	//  焚き火 2200W → 明るさ 30 / 半径 16
	//  屋台   600W  → 明るさ 15 / 半径 9
	//  入口   500W  → 明るさ 12.5 / 半径 8
	//  テント 200W  → 明るさ 5 / 半径 5

	// 明るさ(色に掛ける倍率)
	// _power ... Blender の Power
	// 戻り値 ... 明るさ
	float PowerToIntensity(float _power)
	{
		return std::clamp(_power / 40.0f, 3.0f, 30.0f);
	}

	// 光が届く半径
	// _power ... Blender の Power
	// 戻り値 ... 半径
	float PowerToRadius(float _power)
	{
		return std::clamp(std::sqrt(_power) * 0.35f, 4.0f, 20.0f);
	}
}

CampLayoutData LoadCampLayout(const std::string& _filePath)
{
	CampLayoutData result;

	std::ifstream ifs(_filePath);
	if (!ifs.is_open()) { return result; }

	// 壊れていても例外を投げず is_discarded() になる
	nlohmann::json data = nlohmann::json::parse(ifs, nullptr, false);
	if (data.is_discarded()) { return result; }

	//---- 置物 ----
	if (data.contains("Objects"))
	{
		for (const auto& obj : data["Objects"])
		{
			CampObjectData d;
			d.m_name      = obj.value("Name", "");
			d.m_kind      = obj.value("Kind", "Prop");
			d.m_modelPath = obj.value("Model", "");

			if (obj.contains("Pos"))   { d.m_pos    = ConvPos(JsonToVec3(obj["Pos"])); }
			if (obj.contains("Rot"))   { d.m_rotDeg = ConvRot(JsonToVec3(obj["Rot"])); }
			if (obj.contains("Scale")) { d.m_scale  = JsonToVec3(obj["Scale"]); }

			result.m_objects.push_back(d);
		}
	}

	//---- ライト ----
	if (data.contains("Lights"))
	{
		for (const auto& light : data["Lights"])
		{
			CampLightData d;
			d.m_name  = light.value("Name", "");
			d.m_power = light.value("Power", 0.0f);

			if (light.contains("Pos"))   { d.m_pos   = ConvPos(JsonToVec3(light["Pos"])); }
			if (light.contains("Color")) { d.m_color = JsonToVec3(light["Color"]); }

			// ゲーム用の明るさと半径
			// JSONに書いてあればそれを使い、無ければ Blender の Power から仮の値を作る
			// (仮の値はエディタで画面を見ながら合わせて、CampLayoutTune.json に保存する)
			d.m_intensity = light.value("Intensity", PowerToIntensity(d.m_power));
			d.m_radius    = light.value("Radius",    PowerToRadius(d.m_power));

			result.m_lights.push_back(d);
		}
	}

	//---- カメラ("Explore" などの名前付き) ----
	if (data.contains("Cameras"))
	{
		for (const auto& item : data["Cameras"].items())
		{
			const auto& cam = item.value();

			CampCameraData d;
			if (cam.contains("Pos"))    { d.m_pos    = ConvPos(JsonToVec3(cam["Pos"])); }
			if (cam.contains("Target")) { d.m_target = ConvPos(JsonToVec3(cam["Target"])); }
			d.m_fovY = cam.value("FovY", 40.0f);

			result.m_cameras[item.key()] = d;
		}
	}

	return result;
}

namespace
{
	// 出発の行き先が決まっていないときの仮の値
	// Explore カメラは入口を映しているので、その注視点をプレイヤーの足の高さにして使う
	// 入口の奥のエフェクトの位置が決まっていないときは、プレイヤー → 行き先 の向きにさらに奥へ進めた所にする
	void SetDefaultDepartTarget(CampLayoutData& _layout)
	{
		CampDepartData& d = _layout.m_depart;

		// プレイヤーの位置
		bool hasPlayer = false;
		Math::Vector3 playerPos = Math::Vector3::Zero;
		for (const auto& obj : _layout.m_objects)
		{
			if (obj.m_kind == "Player") { playerPos = obj.m_pos; hasPlayer = true; break; }
		}

		//---- 行き先 ----
		if (!d.m_hasTarget)
		{
			auto it = _layout.m_cameras.find("Explore");
			if (it != _layout.m_cameras.end())
			{
				d.m_target = it->second.m_target;
				if (hasPlayer) { d.m_target.y = playerPos.y; }
			}
		}

		//---- 入口の奥のエフェクト ----
		if (!d.m_hasEffectPos)
		{
			constexpr float kEffectBackDist = 4.0f;	// 行き先からさらに奥へ何m

			Math::Vector3 dir = d.m_target - playerPos;
			dir.y = 0.0f;
			if (hasPlayer && dir.LengthSquared() > 0.0001f)
			{
				dir.Normalize();
				d.m_effectPos = d.m_target + dir * kEffectBackDist;
			}
			else
			{
				d.m_effectPos = d.m_target;
			}
		}
	}
}

void ApplyCampLayoutTune(CampLayoutData& _layout, const std::string& _filePath)
{
	std::ifstream ifs(_filePath);
	if (!ifs.is_open()) { SetDefaultDepartTarget(_layout); return; }	// まだ調整していなければ仮の値だけ

	nlohmann::json data = nlohmann::json::parse(ifs, nullptr, false);
	if (data.is_discarded()) { SetDefaultDepartTarget(_layout); return; }

	//---- カメラを丸ごと差し替える ----
	if (data.contains("Cameras"))
	{
		for (const auto& item : data["Cameras"].items())
		{
			const auto& cam = item.value();
			CampCameraData& d = _layout.m_cameras[item.key()];

			if (cam.contains("Pos"))    { d.m_pos    = ConvPos(JsonToVec3(cam["Pos"])); }
			if (cam.contains("Target")) { d.m_target = ConvPos(JsonToVec3(cam["Target"])); }
			d.m_fovY = cam.value("FovY", d.m_fovY);
		}
	}

	//---- ライトは名前が同じものだけ上書き ----
	if (data.contains("Lights"))
	{
		const auto& lights = data["Lights"];
		for (auto& d : _layout.m_lights)
		{
			if (!lights.contains(d.m_name)) { continue; }
			const auto& light = lights[d.m_name];

			if (light.contains("Color")) { d.m_color = JsonToVec3(light["Color"]); }
			d.m_intensity = light.value("Intensity", d.m_intensity);
			d.m_radius    = light.value("Radius",    d.m_radius);
		}
	}

	//---- 出発の演出 ----
	if (data.contains("Depart"))
	{
		const auto& dep = data["Depart"];
		CampDepartData& d = _layout.m_depart;

		if (dep.contains("Target"))
		{
			d.m_target    = ConvPos(JsonToVec3(dep["Target"]));	// カメラと同じく glTF の座標で保存している
			d.m_hasTarget = true;
		}
		d.m_walkTime      = dep.value("WalkTime",      d.m_walkTime);
		d.m_fadeStartRate = dep.value("FadeStartRate", d.m_fadeStartRate);
		d.m_fadeTime      = dep.value("FadeTime",      d.m_fadeTime);

		if (dep.contains("EffectPos"))
		{
			d.m_effectPos    = ConvPos(JsonToVec3(dep["EffectPos"]));
			d.m_hasEffectPos = true;
		}
		d.m_effectScale   = dep.value("EffectScale",   d.m_effectScale);
	}

	SetDefaultDepartTarget(_layout);
}

bool SaveCampLayout(const CampLayoutData& _layout, const std::string& _filePath)
{
	// 今のファイルを読んで、置物とライト以外(カメラなど)を残す
	nlohmann::json root = nlohmann::json::object();
	{
		std::ifstream ifs(_filePath);
		if (ifs.is_open())
		{
			nlohmann::json old = nlohmann::json::parse(ifs, nullptr, false);
			if (!old.is_discarded() && old.is_object()) { root = old; }
		}
	}

	// 初めて上書きするときだけ、元のファイル(Blender から書き出した物)を残しておく
	const std::string backupPath = _filePath + ".bak";
	if (std::filesystem::exists(_filePath) && !std::filesystem::exists(backupPath))
	{
		std::error_code ec;
		std::filesystem::copy_file(_filePath, backupPath, ec);
	}

	//---- 置物(glTFの座標に戻す) ----
	nlohmann::json objects = nlohmann::json::array();
	for (const auto& d : _layout.m_objects)
	{
		objects.push_back({
			{ "Name",  d.m_name },
			{ "Kind",  d.m_kind },
			{ "Model", d.m_modelPath },
			{ "Pos",   Vec3ToJson(ConvPos(d.m_pos)) },
			{ "Rot",   Vec3ToJson(ConvRot(d.m_rotDeg)) },
			{ "Scale", Vec3ToJson(d.m_scale) } });
	}
	root["Objects"] = objects;

	//---- ライト(glTFの座標に戻す。ゲーム用の明るさと半径も書く) ----
	nlohmann::json lights = nlohmann::json::array();
	for (const auto& d : _layout.m_lights)
	{
		lights.push_back({
			{ "Name",      d.m_name },
			{ "Pos",       Vec3ToJson(ConvPos(d.m_pos)) },
			{ "Color",     Vec3ToJson(d.m_color) },
			{ "Power",     d.m_power },
			{ "Intensity", d.m_intensity },
			{ "Radius",    d.m_radius } });
	}
	root["Lights"] = lights;

	std::ofstream ofs(_filePath);
	if (!ofs.is_open()) { return false; }

	ofs << root.dump(4);
	return true;
}
