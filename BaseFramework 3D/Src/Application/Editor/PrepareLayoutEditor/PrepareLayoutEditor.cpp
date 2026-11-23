#include "PrepareLayoutEditor.h"

#include"../../Scene/PrepareScene/PrepareScene.h"
#include"../../GameObject/Camera/PrepareCamera/PrepareCamera.h"
#include"../../GameObject/Terrains/Camp/CampLayoutData/CampLayoutData.h"
#include"../../GameObject/Terrains/Camp/CampLight/CampLight.h"
#include"../../GameObject/Terrains/Camp/CampObject/CampObject.h"

namespace
{
	// カメラの名前と、そのカメラを使うタブ(表示する順番)
	struct CameraTab
	{
		const char*              m_name;	// JSONのキー
		const char*              m_label;	// 画面に出す名前(U8 で UTF-8 にしたもの)
		PrepareScene::PrepareTab m_tab;		// 切り替えるタブ
	};

	const CameraTab kCameraTabs[] =
	{
		{ "Explore", U8("EXPLORE (入口)"),           PrepareScene::PrepareTab::None    },
		{ "Equip",   U8("EQUIP (テント・武器ラック)"), PrepareScene::PrepareTab::Equip   },
		{ "Shop",    U8("SHOP (屋台)"),              PrepareScene::PrepareTab::Shop    },
		{ "Records", U8("RECORDS (焚き火・日誌)"),    PrepareScene::PrepareTab::Records },
	};
}

void PrepareLayoutEditor::SetTargets(const std::shared_ptr<PrepareScene>& _spScene,
	const std::shared_ptr<PrepareCamera>& _spCamera,
	const std::vector<std::shared_ptr<CampLight>>& _lights,
	const std::vector<std::shared_ptr<CampObject>>& _objects)
{
	m_wpScene  = _spScene;
	m_wpCamera = _spCamera;

	m_wpLights.clear();
	for (const auto& spLight : _lights) { m_wpLights.push_back(spLight); }

	m_wpObjects.clear();
	for (const auto& spObj : _objects) { m_wpObjects.push_back(spObj); }
}

void PrepareLayoutEditor::Draw(bool* _open)
{
	if (!ImGui::Begin(U8("背景とカメラ (PrepareLayoutEditor)"), _open, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	// 保存はメニューに置く(誤って押しにくいように)
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(U8("ファイル")))
		{
			if (ImGui::MenuItem(U8("カメラとライトを保存")))
			{
				SaveTune();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Text(U8("保存先 : %s"), m_tuneFilePath.c_str());
	if (m_dirty) { ImGui::TextColored({ 1.0f, 0.8f, 0.3f, 1.0f }, U8("未保存の変更があります")); }
	if (!m_message.empty()) { ImGui::Text("%s", m_message.c_str()); }

	ImGui::Separator();

	if (ImGui::BeginTabBar("PrepareLayoutTab"))
	{
		if (ImGui::BeginTabItem(U8("カメラ"))) { CameraImGui(); ImGui::EndTabItem(); }
		if (ImGui::BeginTabItem(U8("ライト"))) { LightImGui();  ImGui::EndTabItem(); }
		if (ImGui::BeginTabItem(U8("置物")))   { ObjectImGui(); ImGui::EndTabItem(); }
		ImGui::EndTabBar();
	}

	ImGui::End();
}

void PrepareLayoutEditor::CameraImGui()
{
	auto spCam = m_wpCamera.lock();
	if (!spCam) { ImGui::Text(U8("カメラがありません")); return; }

	auto& cams = spCam->WorkCameraData();

	ImGui::TextWrapped(U8("「このタブで見る」で画面も切り替えてから値を動かすと、本番と同じ見た目で合わせられます"));

	for (const auto& info : kCameraTabs)
	{
		if (!cams.contains(info.m_name)) { continue; }
		CampCameraData& cam = cams[info.m_name];

		// 今映しているカメラは開いた状態にする
		const bool isCurrent = (spCam->GetCurrentName() == info.m_name);
		ImGui::SetNextItemOpen(isCurrent, ImGuiCond_Appearing);

		if (!ImGui::TreeNode(info.m_name, "%s%s", info.m_label, isCurrent ? U8("  ← 表示中") : "")) { continue; }

		if (ImGui::Button(U8("このタブで見る")))
		{
			// UIのタブも切り替える(ChangeTab の中でカメラも動く)
			if (auto spScene = m_wpScene.lock()) { spScene->ChangeTab(info.m_tab); }
			else                                 { spCam->MoveTo(info.m_name); }
		}

		bool changed = false;
		changed |= ImGui::DragFloat3(U8("位置"),   &cam.m_pos.x,    0.02f);
		changed |= ImGui::DragFloat3(U8("注視点"), &cam.m_target.x, 0.02f);
		changed |= ImGui::DragFloat(U8("画角(縦・度)"), &cam.m_fovY, 0.1f, 15.0f, 90.0f, "%.1f");

		if (changed)
		{
			m_dirty = true;

			// いじっているカメラを、その場で画面に映す
			// 別のカメラを映していたら、UIのタブごと切り替えてから瞬間移動する
			if (!isCurrent)
			{
				if (auto spScene = m_wpScene.lock()) { spScene->ChangeTab(info.m_tab); }
			}
			spCam->MoveTo(info.m_name, true);
		}

		ImGui::TreePop();
	}
}

void PrepareLayoutEditor::LightImGui()
{
	ImGui::TextWrapped(U8("明るさは色に掛ける倍率です(松明は色{24,10,2}・半径25が目安)"));

	int i = 0;
	for (const auto& wpLight : m_wpLights)
	{
		auto spLight = wpLight.lock();
		if (!spLight) { continue; }

		CampLightData& d = spLight->WorkData();

		ImGui::PushID(i++);
		if (ImGui::TreeNode(d.m_name.c_str()))
		{
			if (ImGui::ColorEdit3(U8("色"), &d.m_color.x))                            { m_dirty = true; }
			if (ImGui::DragFloat(U8("明るさ"), &d.m_intensity, 0.1f, 0.0f, 60.0f, "%.1f")) { m_dirty = true; }
			if (ImGui::DragFloat(U8("半径"),   &d.m_radius,    0.05f, 0.0f, 40.0f, "%.2f")) { m_dirty = true; }

			// 位置は Blender で決めるので表示だけ
			ImGui::Text(U8("位置 : %.2f, %.2f, %.2f (Blenderで変更)"), d.m_pos.x, d.m_pos.y, d.m_pos.z);
			ImGui::Text(U8("Blender の Power : %.0f"), d.m_power);

			ImGui::TreePop();
		}
		ImGui::PopID();
	}
}

void PrepareLayoutEditor::ObjectImGui()
{
	ImGui::TextWrapped(U8("ここで動かした値は保存されません。決まったら「Blenderでの値」をBlenderに入れて書き出し直してください"));

	// 左 : 一覧
	ImGui::BeginChild("ObjectList", { 200.0f, 0.0f }, true);
	for (int i = 0; i < static_cast<int>(m_wpObjects.size()); ++i)
	{
		auto spObj = m_wpObjects[i].lock();
		if (!spObj) { continue; }

		ImGui::PushID(i);
		if (ImGui::Selectable(spObj->WorkData().m_name.c_str(), m_selectObject == i))
		{
			m_selectObject = i;
		}
		ImGui::PopID();
	}
	ImGui::EndChild();

	ImGui::SameLine();

	// 右 : 選んだ置物
	ImGui::BeginChild("ObjectDetail", { 0.0f, 0.0f }, false);
	if (m_selectObject >= 0 && m_selectObject < static_cast<int>(m_wpObjects.size()))
	{
		if (auto spObj = m_wpObjects[m_selectObject].lock())
		{
			CampObjectData& d = spObj->WorkData();

			ImGui::Text("%s", d.m_name.c_str());
			ImGui::TextDisabled("%s", d.m_modelPath.c_str());

			bool changed = false;
			changed |= ImGui::DragFloat3(U8("位置"),     &d.m_pos.x,    0.02f);
			changed |= ImGui::DragFloat3(U8("回転(度)"), &d.m_rotDeg.x, 0.5f);
			changed |= ImGui::DragFloat3(U8("拡大"),     &d.m_scale.x,  0.01f);
			if (changed) { spObj->ApplyData(); }

			// Blender での値(ゲーム → glTF → Blender の変換をした値)
			// ゲーム(x, y, z) → Blender(x, z, y)、Y回転 → Blender の Z回転(符号反転)
			ImGui::Separator();
			ImGui::Text(U8("Blenderでの値"));
			ImGui::Text(U8("位置 : %.3f, %.3f, %.3f"), d.m_pos.x, d.m_pos.z, d.m_pos.y);
			ImGui::Text(U8("Z回転 : %.1f 度"), -d.m_rotDeg.y);
		}
	}
	else
	{
		ImGui::Text(U8("左の一覧から選んでください"));
	}
	ImGui::EndChild();
}

bool PrepareLayoutEditor::SaveTune()
{
	nlohmann::json root;

	//---- カメラ(glTFの座標に戻して保存) ----
	if (auto spCam = m_wpCamera.lock())
	{
		for (const auto& [name, cam] : spCam->WorkCameraData())
		{
			root["Cameras"][name] = {
				{ "Pos",    Vec3ToJson(ConvPos(cam.m_pos)) },
				{ "Target", Vec3ToJson(ConvPos(cam.m_target)) },
				{ "FovY",   cam.m_fovY } };
		}
	}

	//---- ライト(名前ごとに、色・明るさ・半径) ----
	for (const auto& wpLight : m_wpLights)
	{
		auto spLight = wpLight.lock();
		if (!spLight) { continue; }

		const CampLightData& d = spLight->WorkData();
		root["Lights"][d.m_name] = {
			{ "Color",     Vec3ToJson(d.m_color) },
			{ "Intensity", d.m_intensity },
			{ "Radius",    d.m_radius } };
	}

	std::ofstream file(m_tuneFilePath);
	if (!file.is_open())
	{
		m_message = U8("保存できませんでした");
		return false;
	}

	file << root.dump(4);

	m_dirty   = false;
	m_message = U8("保存しました");
	return true;
}
