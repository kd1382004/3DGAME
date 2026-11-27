//---- 自分のヘッダー ----
#include "PrepareLayoutEditor.h"												//準備シーンの配置編集エディター

//---- シーン ----
#include"../../Scene/PrepareScene/PrepareScene.h"								//準備シーン

//---- カメラ ----
#include"../../GameObject/Camera/PrepareCamera/PrepareCamera.h"					//準備シーン用カメラ

//---- キャンプ ----
#include"../../GameObject/Terrains/Camp/CampManager.h"							//キャンプの管理
#include"../../GameObject/Terrains/Camp/CampLayoutData/CampLayoutData.h"		//キャンプの配置データ
#include"../../GameObject/Terrains/Camp/CampLight/CampLight.h"					//キャンプのライト
#include"../../GameObject/Terrains/Camp/CampObject/CampObject.h"				//キャンプの置物

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

	// 置物の種類(Torch は自分で光る松明なのでキャンプには出さない)
	//  Prop     ... ただの置物
	//  Campfire ... 炎とパチパチ音が付く
	//  Player   ... 出発の演出で入口へ走るプレイヤー(1体だけ)
	//  Merchant ... 商人
	const char* kObjectKinds[] = { "Prop", "Campfire", "Player", "Merchant" };

	// 追加するモデルを探すフォルダ
	const char* kCampModelDir = "Asset/Models/Terrains/Camp";

	// 黄色の注意書き
	const ImVec4 kWarnColor = { 1.0f, 0.8f, 0.3f, 1.0f };

	// "Asset/Models/Terrains/Camp/Barrel/Barrel.gltf" → "Barrel"
	std::string ModelBaseName(const std::string& _path)
	{
		return std::filesystem::path(_path).stem().string();
	}

	// "Barrel_2" → "Barrel" / "Crate.006" → "Crate"(番号を外す)
	std::string StripNumber(const std::string& _name)
	{
		size_t end = _name.size();
		while (end > 0 && std::isdigit(static_cast<unsigned char>(_name[end - 1]))) { --end; }
		if (end < _name.size() && end > 0 && (_name[end - 1] == '_' || _name[end - 1] == '.')) { return _name.substr(0, end - 1); }
		return _name;
	}
}

//=============================================
// 設定
//=============================================

void PrepareLayoutEditor::SetTargets(const std::shared_ptr<PrepareScene>& _spScene, const std::shared_ptr<CampManager>& _spCampManager)
{
	m_wpScene       = _spScene;
	m_wpCampManager = _spCampManager;

	RefreshModelPaths();
}

//=============================================
// 基本の処理
//=============================================

void PrepareLayoutEditor::Draw(bool* _open)
{
	// 画面の上でのマウス操作(ウィンドウを畳んでいても使えるように先にやる)
	UpdateMouse();
	UpdateHighlight();

	if (!ImGui::Begin(U8("背景とカメラ (PrepareLayoutEditor)"), _open, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	auto spCamp = m_wpCampManager.lock();
	if (!spCamp)
	{
		ImGui::Text(U8("キャンプがありません"));
		ImGui::End();
		return;
	}

	// 保存はメニューに置く(誤って押しにくいように)
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(U8("ファイル")))
		{
			if (ImGui::MenuItem(U8("全部保存")))
			{
				const bool layoutOk = SaveLayout();
				const bool tuneOk   = SaveTune();
				m_message = (layoutOk && tuneOk) ? U8("全部保存しました") : U8("保存できなかったファイルがあります");
			}
			ImGui::Separator();
			if (ImGui::MenuItem(U8("置物・ライトの配置を保存 (CampLayout.json)"))) { SaveLayout(); }
			if (ImGui::MenuItem(U8("カメラ・ライトの明るさ・出発を保存 (CampLayoutTune.json)"))) { SaveTune(); }
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::TextDisabled(U8("配置 : %s"), spCamp->GetLayoutFilePath().c_str());
	ImGui::TextDisabled(U8("調整 : %s"), spCamp->GetTuneFilePath().c_str());
	if (m_layoutDirty) { ImGui::TextColored(kWarnColor, U8("置物・ライトの配置に未保存の変更があります")); }
	if (m_tuneDirty)   { ImGui::TextColored(kWarnColor, U8("カメラ・ライトの明るさ・出発に未保存の変更があります")); }
	if (!m_message.empty()) { ImGui::Text("%s", m_message.c_str()); }

	ImGui::Separator();

	ImGui::Checkbox(U8("マウスで置物を選ぶ・動かす"), &m_mousePickFlg);
	ImGui::SameLine();
	ImGui::Checkbox(U8("地面・崖も選ぶ"), &m_pickGroundFlg);
	ImGui::TextDisabled(U8("クリック:選ぶ / ドラッグ:動かす / Shift+ドラッグ:上下 / ホイール:回転(Shiftで細かく) / Delete:消す"));

	ImGui::Separator();

	if (ImGui::BeginTabBar("PrepareLayoutTab"))
	{
		if (ImGui::BeginTabItem(U8("置物")))   { ObjectImGui(); ImGui::EndTabItem(); }
		if (ImGui::BeginTabItem(U8("ライト"))) { LightImGui();  ImGui::EndTabItem(); }
		if (ImGui::BeginTabItem(U8("カメラ"))) { CameraImGui(); ImGui::EndTabItem(); }
		if (ImGui::BeginTabItem(U8("出発")))   { DepartImGui(); ImGui::EndTabItem(); }
		ImGui::EndTabBar();
	}

	ImGui::End();
}

//=============================================
// その他の処理
//=============================================

bool PrepareLayoutEditor::SaveTune()
{
	auto spCamp = m_wpCampManager.lock();
	if (!spCamp) { return false; }

	nlohmann::json root;

	//---- カメラ(glTFの座標に戻して保存) ----
	if (const auto& spCam = spCamp->GetCamera())
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
	for (const auto& spLight : spCamp->GetCampLights())
	{
		if (!spLight) { continue; }

		const CampLightData& d = spLight->WorkData();
		root["Lights"][d.m_name] = {
			{ "Color",     Vec3ToJson(d.m_color) },
			{ "Intensity", d.m_intensity },
			{ "Radius",    d.m_radius } };
	}

	//---- 出発の演出(行き先はカメラと同じく glTF の座標に戻して保存) ----
	const CampDepartData& dep = spCamp->WorkDepartData();
	root["Depart"] = {
		{ "Target",        Vec3ToJson(ConvPos(dep.m_target)) },
		{ "WalkTime",      dep.m_walkTime },
		{ "FadeStartRate", dep.m_fadeStartRate },
		{ "FadeTime",      dep.m_fadeTime },
		{ "EffectPos",     Vec3ToJson(ConvPos(dep.m_effectPos)) },
		{ "EffectScale",   dep.m_effectScale } };

	std::ofstream file(spCamp->GetTuneFilePath());
	if (!file.is_open())
	{
		m_message = U8("CampLayoutTune.json を保存できませんでした");
		return false;
	}

	file << root.dump(4);

	m_tuneDirty = false;
	m_message   = U8("CampLayoutTune.json を保存しました");
	return true;
}

bool PrepareLayoutEditor::SaveLayout()
{
	auto spCamp = m_wpCampManager.lock();
	if (!spCamp) { return false; }

	// プレイヤーを行き先に置いて見ている間は、元の位置で保存する
	auto spPlayer = spCamp->GetCampPlayer();
	Math::Vector3 previewPos = Math::Vector3::Zero;
	if (m_departPreviewFlg && spPlayer)
	{
		previewPos = spPlayer->WorkData().m_pos;
		spPlayer->WorkData().m_pos = m_departPreviewOrgPos;
	}

	const bool ok = spCamp->SaveLayout();

	if (m_departPreviewFlg && spPlayer) { spPlayer->WorkData().m_pos = previewPos; }

	if (!ok)
	{
		m_message = U8("CampLayout.json を保存できませんでした");
		return false;
	}

	m_layoutDirty = false;
	m_message     = U8("CampLayout.json を保存しました(初回は元のファイルを .bak に残しています)");
	return true;
}

//=============================================
// 基本の処理
//=============================================

void PrepareLayoutEditor::CameraImGui()
{
	auto spCamp = m_wpCampManager.lock();
	if (!spCamp || !spCamp->GetCamera()) { ImGui::Text(U8("カメラがありません")); return; }

	const auto& spCam = spCamp->GetCamera();
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
			m_tuneDirty = true;

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

void PrepareLayoutEditor::ObjectImGui()
{
	auto spCamp = m_wpCampManager.lock();
	if (!spCamp) { return; }

	const auto& objects = spCamp->GetCampObjects();
	if (m_selectObject >= static_cast<int>(objects.size())) { m_selectObject = -1; }

	//---- 追加 ----
	if (!m_modelPaths.empty())
	{
		m_addModelIndex = std::clamp(m_addModelIndex, 0, static_cast<int>(m_modelPaths.size()) - 1);

		ImGui::SetNextItemWidth(260.0f);
		if (ImGui::BeginCombo(U8("##AddModel"), ModelBaseName(m_modelPaths[m_addModelIndex]).c_str()))
		{
			for (int i = 0; i < static_cast<int>(m_modelPaths.size()); ++i)
			{
				if (ImGui::Selectable(m_modelPaths[i].c_str(), m_addModelIndex == i)) { m_addModelIndex = i; }
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		if (ImGui::Button(U8("追加")))
		{
			// 今のカメラが見ている地面に置く(画面の真ん中に出る)
			CampObjectData d;
			d.m_modelPath = m_modelPaths[m_addModelIndex];
			d.m_name      = MakeUniqueObjectName(ModelBaseName(d.m_modelPath));
			d.m_kind      = "Prop";
			d.m_pos       = GetCameraGroundPos();

			spCamp->AddCampObject(d);
			m_selectObject = static_cast<int>(spCamp->GetCampObjects().size()) - 1;
			m_layoutDirty  = true;
		}
		ImGui::SameLine();
		if (ImGui::SmallButton(U8("一覧を更新"))) { RefreshModelPaths(); }
	}
	else
	{
		ImGui::TextDisabled(U8("%s にモデルが見つかりません"), kCampModelDir);
	}

	ImGui::Separator();

	//---- 左 : 一覧 ----
	ImGui::BeginGroup();
	ImGui::SetNextItemWidth(200.0f);
	ImGui::InputTextWithHint("##ObjFilter", U8("名前で絞り込み"), &m_objectFilter);

	ImGui::BeginChild("ObjectList", { 200.0f, 0.0f }, true);
	for (int i = 0; i < static_cast<int>(objects.size()); ++i)
	{
		const auto& spObj = objects[i];
		if (!spObj) { continue; }

		const std::string& name = spObj->WorkData().m_name;
		if (!m_objectFilter.empty() && name.find(m_objectFilter) == std::string::npos) { continue; }

		ImGui::PushID(i);
		if (ImGui::Selectable(name.c_str(), m_selectObject == i)) { m_selectObject = i; }
		ImGui::PopID();
	}
	ImGui::EndChild();
	ImGui::EndGroup();

	ImGui::SameLine();

	//---- 右 : 選んだ置物 ----
	ImGui::BeginChild("ObjectDetail", { 0.0f, 0.0f }, false);

	if (m_selectObject < 0)
	{
		ImGui::Text(U8("左の一覧から選んでください"));
		ImGui::EndChild();
		return;
	}

	std::shared_ptr<CampObject> spObj = objects[m_selectObject];
	if (!spObj) { ImGui::EndChild(); return; }

	CampObjectData& d = spObj->WorkData();

	//---- 複製・削除 ----
	if (ImGui::Button(U8("複製")))
	{
		CampObjectData copy = d;
		copy.m_name  = MakeUniqueObjectName(StripNumber(d.m_name));
		copy.m_pos.x += 1.0f;	// 重ならないように少しずらす
		if (copy.m_kind == "Player") { copy.m_kind = "Prop"; }	// プレイヤーは1体だけ

		spCamp->AddCampObject(copy);
		m_selectObject = static_cast<int>(spCamp->GetCampObjects().size()) - 1;
		m_layoutDirty  = true;

		ImGui::EndChild();
		return;
	}
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, { 0.6f, 0.2f, 0.2f, 1.0f });
	if (ImGui::Button(U8("削除")))
	{
		spCamp->RemoveCampObject(spObj);
		m_selectObject = -1;
		m_layoutDirty  = true;

		ImGui::PopStyleColor();
		ImGui::EndChild();
		return;
	}
	ImGui::PopStyleColor();

	ImGui::Separator();

	//---- 名前・種類・モデル ----
	if (ImGui::InputText(U8("名前"), &d.m_name)) { m_layoutDirty = true; }
	if (d.m_name.rfind("Rack", 0) == 0)
	{
		ImGui::TextDisabled(U8("名前が RackSword / RackDagger で始まる物は、武器を持っているときだけ出ます"));
	}

	bool rebuild = false;

	if (ImGui::BeginCombo(U8("種類"), d.m_kind.c_str()))
	{
		for (const char* kind : kObjectKinds)
		{
			if (ImGui::Selectable(kind, d.m_kind == kind) && d.m_kind != kind)
			{
				d.m_kind = kind;
				rebuild  = true;
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo(U8("モデル"), ModelBaseName(d.m_modelPath).c_str()))
	{
		for (const auto& path : m_modelPaths)
		{
			if (ImGui::Selectable(path.c_str(), d.m_modelPath == path) && d.m_modelPath != path)
			{
				d.m_modelPath = path;
				rebuild = true;
			}
		}
		ImGui::EndCombo();
	}
	ImGui::TextDisabled("%s", d.m_modelPath.c_str());

	if (rebuild)
	{
		RebuildObject(spObj);	// 作り直すと一覧の最後に移るので、選び直す
		m_selectObject = static_cast<int>(spCamp->GetCampObjects().size()) - 1;
		m_layoutDirty  = true;
		ImGui::EndChild();
		return;
	}

	ImGui::Separator();

	//---- 位置・回転・拡大 ----
	bool moved = false;
	moved |= ImGui::DragFloat3(U8("位置"),     &d.m_pos.x,    0.02f);
	moved |= ImGui::DragFloat3(U8("回転(度)"), &d.m_rotDeg.x, 0.5f);

	// 拡大はまとめて変えることが多いので、そろえるボタンも付ける
	moved |= ImGui::DragFloat3(U8("拡大"), &d.m_scale.x, 0.01f, 0.01f, 100.0f);
	float uniform = d.m_scale.x;
	if (ImGui::DragFloat(U8("拡大(全部そろえる)"), &uniform, 0.01f, 0.01f, 100.0f))
	{
		d.m_scale = { uniform, uniform, uniform };
		moved = true;
	}

	if (ImGui::Button(U8("地面に置く (Y=0)")))     { d.m_pos.y = 0.0f; moved = true; }
	ImGui::SameLine();
	if (ImGui::Button(U8("カメラの見ている所へ"))) { d.m_pos = GetCameraGroundPos(); moved = true; }
	ImGui::SameLine();
	if (ImGui::Button(U8("回転を戻す")))           { d.m_rotDeg = Math::Vector3::Zero; moved = true; }

	if (moved)
	{
		spCamp->OnCampObjectMoved(spObj);	// 焚き火なら炎と音もついてくる
		m_layoutDirty = true;
	}

	ImGui::EndChild();
}

void PrepareLayoutEditor::LightImGui()
{
	auto spCamp = m_wpCampManager.lock();
	if (!spCamp) { return; }

	ImGui::TextWrapped(U8("明るさは色に掛ける倍率です(松明は色{24,10,2}・半径25が目安)。位置は「配置」、色・明るさ・半径は「調整」のファイルに保存されます"));

	if (ImGui::Button(U8("ライトを追加")))
	{
		// カメラが見ている所の少し上に置く
		CampLightData d;
		d.m_name      = MakeUniqueLightName("Light");
		d.m_pos       = GetCameraGroundPos() + Math::Vector3(0.0f, 2.0f, 0.0f);
		d.m_color     = { 1.0f, 0.7f, 0.4f };
		d.m_intensity = 10.0f;
		d.m_radius    = 8.0f;

		spCamp->AddCampLight(d);
		m_selectLight = static_cast<int>(spCamp->GetCampLights().size()) - 1;
		m_layoutDirty = true;
		m_tuneDirty   = true;
	}

	ImGui::Separator();

	const auto& lights = spCamp->GetCampLights();
	for (int i = 0; i < static_cast<int>(lights.size()); ++i)
	{
		const auto& spLight = lights[i];
		if (!spLight) { continue; }

		CampLightData& d = spLight->WorkData();

		ImGui::PushID(i);
		ImGui::SetNextItemOpen(m_selectLight == i, ImGuiCond_Always);
		const bool open = ImGui::TreeNode(d.m_name.c_str());
		if (ImGui::IsItemClicked()) { m_selectLight = (m_selectLight == i) ? -1 : i; }

		if (open)
		{
			if (ImGui::DragFloat3(U8("位置"), &d.m_pos.x, 0.02f))                          { m_layoutDirty = true; }
			if (ImGui::ColorEdit3(U8("色"), &d.m_color.x))                                  { m_tuneDirty = true; }
			if (ImGui::DragFloat(U8("明るさ"), &d.m_intensity, 0.1f, 0.0f, 60.0f, "%.1f"))  { m_tuneDirty = true; }
			if (ImGui::DragFloat(U8("半径"),   &d.m_radius,    0.05f, 0.0f, 40.0f, "%.2f")) { m_tuneDirty = true; }

			if (ImGui::Button(U8("カメラの見ている所へ")))
			{
				d.m_pos = GetCameraGroundPos() + Math::Vector3(0.0f, 2.0f, 0.0f);
				m_layoutDirty = true;
			}
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.6f, 0.2f, 0.2f, 1.0f });
			const bool remove = ImGui::Button(U8("削除"));
			ImGui::PopStyleColor();

			ImGui::TreePop();

			if (remove)
			{
				spCamp->RemoveCampLight(spLight);
				m_selectLight = -1;
				m_layoutDirty = true;
				m_tuneDirty   = true;
				ImGui::PopID();
				break;	// 一覧が変わったので、続きは次のフレームで描く
			}
		}
		ImGui::PopID();
	}
}

void PrepareLayoutEditor::DepartImGui()
{
	auto spCamp = m_wpCampManager.lock();
	if (!spCamp) { return; }

	CampDepartData& d = spCamp->WorkDepartData();
	auto spPlayer = spCamp->GetCampPlayer();

	ImGui::TextWrapped(U8("出発ボタンを押したら、プレイヤーが「行き先」まで歩き、途中から暗転してゲームへ移ります。行き先は入口の少し奥に置くと、入口に入っていくように見えます"));

	if (!d.m_hasTarget)
	{
		ImGui::TextColored(kWarnColor, U8("行き先はまだ仮の値です(Explore カメラの注視点)"));
	}

	ImGui::Separator();

	bool changed = false;

	if (ImGui::DragFloat3(U8("行き先"), &d.m_target.x, 0.02f))
	{
		d.m_hasTarget = true;
		changed = true;
	}

	if (spPlayer)
	{
		// 今のプレイヤーの位置を行き先にする(置物タブでプレイヤーを動かして決めたいとき用)
		if (ImGui::Button(U8("今のプレイヤーの位置を行き先にする")))
		{
			d.m_target    = m_departPreviewFlg ? d.m_target : spPlayer->WorkData().m_pos;
			d.m_hasTarget = true;
			changed = true;
		}
	}

	changed |= ImGui::DragFloat(U8("歩く時間(秒)"),        &d.m_walkTime,      0.02f, 0.1f, 10.0f, "%.2f");
	changed |= ImGui::SliderFloat(U8("暗転を始める割合"),  &d.m_fadeStartRate, 0.0f,  1.0f,        "%.2f");
	changed |= ImGui::DragFloat(U8("暗転の時間(秒)"),      &d.m_fadeTime,      0.02f, 0.1f, 5.0f,  "%.2f");

	//---- 入口の奥のエフェクト ----
	ImGui::Separator();
	ImGui::Text(U8("入口の奥のエフェクト (Stairs/Stairs.efkefc)"));

	bool effectChanged = false;
	if (ImGui::DragFloat3(U8("エフェクトの位置"), &d.m_effectPos.x, 0.02f))
	{
		d.m_hasEffectPos = true;
		effectChanged = true;
	}
	effectChanged |= ImGui::DragFloat(U8("エフェクトの大きさ"), &d.m_effectScale, 0.01f, 0.05f, 10.0f, "%.2f");

	if (effectChanged)
	{
		changed = true;

		// その場で動かす(ループで出し直すときも同じ位置・大きさになるように再生情報も書き換える)
		if (auto spEffect = spCamp->GetEntranceEffect().lock())
		{
			spEffect->SetPos(d.m_effectPos);
			spEffect->SetScale(d.m_effectScale);
			spEffect->WorkPlayEfkInfo().Pos  = d.m_effectPos;
			spEffect->WorkPlayEfkInfo().Size = Math::Vector3(d.m_effectScale);
		}
	}

	if (changed) { m_tuneDirty = true; }

	ImGui::Separator();

	if (!spPlayer)
	{
		ImGui::Text(U8("キャンプにプレイヤー(種類 Player)がいません"));
		return;
	}

	// 目安の表示
	const Math::Vector3 startPos = m_departPreviewFlg ? m_departPreviewOrgPos : spPlayer->WorkData().m_pos;
	Math::Vector3 diff = d.m_target - startPos;
	diff.y = 0.0f;
	const float dist = diff.Length();

	ImGui::Text(U8("歩く距離 : %.2f"), dist);
	ImGui::Text(U8("歩く速さ : %.2f / 秒 (歩きアニメの足と合うように時間を合わせる)"), d.m_walkTime > 0.0f ? dist / d.m_walkTime : 0.0f);
	ImGui::Text(U8("暗転が始まる : %.2f 秒後 / 暗くなり切る : %.2f 秒後"),
		d.m_walkTime * d.m_fadeStartRate, d.m_walkTime * d.m_fadeStartRate + d.m_fadeTime);

	if (d.m_walkTime * d.m_fadeStartRate + d.m_fadeTime > d.m_walkTime)
	{
		ImGui::TextColored(kWarnColor, U8("暗くなり切る前に行き先に着きます(着いたら止まって見えます)"));
	}

	// プレイヤーを行き先に置いて、画面で位置を確かめる(保存はされない。外すと元の位置に戻る)
	bool preview = m_departPreviewFlg;
	if (ImGui::Checkbox(U8("プレイヤーを行き先に置いて見る"), &preview))
	{
		if (preview) { m_departPreviewOrgPos = spPlayer->WorkData().m_pos; }
		else         { spPlayer->WorkData().m_pos = m_departPreviewOrgPos; spPlayer->ApplyData(); }
		m_departPreviewFlg = preview;
	}

	if (m_departPreviewFlg)
	{
		spPlayer->WorkData().m_pos = d.m_target;
		spPlayer->ApplyData();
	}
}

//=============================================
// その他の処理
//=============================================

void PrepareLayoutEditor::OnClose()
{
	m_draggingFlg = false;
	m_wpDragObject.reset();

	if (auto spOld = m_wpHighlight.lock()) { spOld->SetHighlight(false); }
	m_wpHighlight.reset();
}

void PrepareLayoutEditor::UpdateMouse()
{
	auto spCamp = m_wpCampManager.lock();
	if (!spCamp || !m_mousePickFlg)
	{
		m_draggingFlg = false;
		return;
	}

	const ImGuiIO& io = ImGui::GetIO();

	//---- ドラッグ中 ----
	if (m_draggingFlg)
	{
		auto spObj = m_wpDragObject.lock();

		// ボタンを離したら終わり(ImGui のウィンドウの上を通っても続ける)
		if (!spObj || !ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			m_draggingFlg = false;
			m_wpDragObject.reset();
			return;
		}

		CampObjectData& d = spObj->WorkData();
		bool moved = false;

		if (io.KeyShift)
		{
			// 上下 : マウスを上に動かすと上がる。カメラから遠いほど大きく動かす
			const float dy = m_dragLastMousePos.y - io.MousePos.y;
			float dist = 10.0f;
			if (const auto& spCam = spCamp->GetCamera())
			{
				if (spCam->GetCamera()) { dist = (spCam->GetCamera()->GetCameraMatrix().Translation() - d.m_pos).Length(); }
			}
			if (dy != 0.0f)
			{
				d.m_pos.y += dy * dist * 0.0015f;
				moved = true;
			}

			// 上下に動かした後に地面にそって動かしても飛ばないように、面の高さとずれを合わせ直す
			Math::Vector3 onPlane;
			m_dragPlaneY = d.m_pos.y;
			if (MouseOnPlaneY(m_dragPlaneY, onPlane)) { m_dragOffset = d.m_pos - onPlane; m_dragOffset.y = 0.0f; }
		}
		else
		{
			// 地面にそって : クリックした所との ずれ を保ったまま動かす
			Math::Vector3 onPlane;
			if (MouseOnPlaneY(m_dragPlaneY, onPlane))
			{
				const Math::Vector3 newPos = { onPlane.x + m_dragOffset.x, d.m_pos.y, onPlane.z + m_dragOffset.z };
				if (newPos != d.m_pos) { d.m_pos = newPos; moved = true; }
			}
		}

		if (moved)
		{
			spCamp->OnCampObjectMoved(spObj);
			m_layoutDirty = true;
		}

		m_dragLastMousePos = io.MousePos;
		return;
	}

	// ImGui のウィンドウの上では何もしない
	if (io.WantCaptureMouse) { return; }

	const auto& objects = spCamp->GetCampObjects();

	//---- クリックで選ぶ → そのままドラッグ ----
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		const int index = PickObject().first;
		m_selectObject = index;

		if (index >= 0)
		{
			const auto& spObj = objects[index];

			m_draggingFlg      = true;
			m_wpDragObject     = spObj;
			m_dragLastMousePos = io.MousePos;

			// 置物の足元の高さの面で動かす。クリックした所と置物の位置のずれを覚えておく
			m_dragPlaneY = spObj->WorkData().m_pos.y;
			Math::Vector3 onPlane;
			if (MouseOnPlaneY(m_dragPlaneY, onPlane)) { m_dragOffset = spObj->WorkData().m_pos - onPlane; }
			else                                      { m_dragOffset = Math::Vector3::Zero; }
			m_dragOffset.y = 0.0f;
		}
		return;
	}

	if (m_selectObject < 0 || m_selectObject >= static_cast<int>(objects.size())) { return; }
	const auto& spSelect = objects[m_selectObject];
	if (!spSelect) { return; }

	//---- ホイールで回す ----
	if (io.MouseWheel != 0.0f)
	{
		const float step = io.KeyShift ? 1.0f : 15.0f;
		float& rotY = spSelect->WorkData().m_rotDeg.y;
		rotY += (io.MouseWheel > 0.0f ? step : -step);

		// -180〜180 に収める
		while (rotY >  180.0f) { rotY -= 360.0f; }
		while (rotY < -180.0f) { rotY += 360.0f; }

		spCamp->OnCampObjectMoved(spSelect);
		m_layoutDirty = true;
	}

	//---- Delete で消す(ImGui で文字を打っているときは除く) ----
	if (!io.WantCaptureKeyboard && ImGui::IsKeyPressed(ImGuiKey_Delete, false))
	{
		spCamp->RemoveCampObject(spSelect);
		m_selectObject = -1;
		m_layoutDirty  = true;
	}
}

void PrepareLayoutEditor::UpdateHighlight()
{
	std::shared_ptr<CampObject> spNow;
	if (auto spCamp = m_wpCampManager.lock())
	{
		const auto& objects = spCamp->GetCampObjects();
		if (m_selectObject >= 0 && m_selectObject < static_cast<int>(objects.size())) { spNow = objects[m_selectObject]; }
	}

	auto spOld = m_wpHighlight.lock();
	if (spOld == spNow) { return; }

	if (spOld) { spOld->SetHighlight(false); }
	if (spNow) { spNow->SetHighlight(true); }
	m_wpHighlight = spNow;
}

bool PrepareLayoutEditor::MakeMouseRay(Math::Vector3& _rayPos, Math::Vector3& _rayDir, float& _range) const
{
	auto spCamp = m_wpCampManager.lock();
	if (!spCamp || !spCamp->GetCamera() || !spCamp->GetCamera()->GetCamera()) { return false; }

	// ImGui のマウスの位置はウィンドウの左上が(0,0)(GenerateRayInfoFromClientPos と同じ)
	const ImVec2 mouse = ImGui::GetIO().MousePos;
	if (!ImGui::IsMousePosValid(&mouse)) { return false; }

	const POINT clientPos = { static_cast<LONG>(mouse.x), static_cast<LONG>(mouse.y) };
	spCamp->GetCamera()->GetCamera()->GenerateRayInfoFromClientPos(clientPos, _rayPos, _rayDir, _range);
	_rayDir.Normalize();
	return true;
}

std::pair<int, Math::Vector3> PrepareLayoutEditor::PickObject() const
{
	std::pair<int, Math::Vector3> result = { -1, Math::Vector3::Zero };

	auto spCamp = m_wpCampManager.lock();
	if (!spCamp) { return result; }

	Math::Vector3 rayPos, rayDir;
	float range = 0.0f;
	if (!MakeMouseRay(rayPos, rayDir, range)) { return result; }

	const KdCollider::RayInfo ray(KdCollider::TypeEvent, rayPos, rayDir, range);

	float nearest = FLT_MAX;
	const auto& objects = spCamp->GetCampObjects();
	for (int i = 0; i < static_cast<int>(objects.size()); ++i)
	{
		const auto& spObj = objects[i];
		if (!spObj || !spObj->GetDrawFlg()) { continue; }	// 隠れている武器は選ばない

		// 地面と崖は大きくて手前の物を選ぶ邪魔になるので、普段は選ばない
		const std::string& name = spObj->WorkData().m_name;
		if (!m_pickGroundFlg && (name.rfind("Ground", 0) == 0 || name.rfind("Cliff", 0) == 0)) { continue; }

		std::list<KdCollider::CollisionResult> hits;
		if (!spObj->Intersects(ray, &hits)) { continue; }

		for (const auto& hit : hits)
		{
			const float dist = (hit.m_hitPos - rayPos).Length();
			if (dist < nearest)
			{
				nearest = dist;
				result  = { i, hit.m_hitPos };
			}
		}
	}

	return result;
}

bool PrepareLayoutEditor::MouseOnPlaneY(float _y, Math::Vector3& _out) const
{
	Math::Vector3 rayPos, rayDir;
	float range = 0.0f;
	if (!MakeMouseRay(rayPos, rayDir, range)) { return false; }

	// 水平に近いレイは遠くへ飛びすぎるので使わない
	if (std::abs(rayDir.y) < 0.0001f) { return false; }

	const float t = (_y - rayPos.y) / rayDir.y;
	if (t <= 0.0f) { return false; }	// カメラの後ろ

	_out = rayPos + rayDir * t;
	return true;
}

void PrepareLayoutEditor::RefreshModelPaths()
{
	std::vector<std::string> paths;

	// キャンプ用のモデルのフォルダ
	std::error_code ec;
	if (std::filesystem::exists(kCampModelDir, ec))
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(kCampModelDir, ec))
		{
			if (!entry.is_regular_file()) { continue; }
			if (entry.path().extension() != ".gltf") { continue; }

			std::string path = entry.path().generic_string();	// 区切りを '/' にそろえる
			paths.push_back(path);
		}
	}

	// 今置いている物(武器・プレイヤーなど、フォルダの外のモデルも選べるように)
	if (auto spCamp = m_wpCampManager.lock())
	{
		for (const auto& spObj : spCamp->GetCampObjects())
		{
			if (spObj && !spObj->WorkData().m_modelPath.empty()) { paths.push_back(spObj->WorkData().m_modelPath); }
		}
	}

	// 名前順に並べて、同じ物を1つにする
	std::sort(paths.begin(), paths.end());
	paths.erase(std::unique(paths.begin(), paths.end()), paths.end());
	m_modelPaths = std::move(paths);
}

Math::Vector3 PrepareLayoutEditor::GetCameraGroundPos() const
{
	auto spCamp = m_wpCampManager.lock();
	if (!spCamp || !spCamp->GetCamera()) { return Math::Vector3::Zero; }

	const auto& spCam = spCamp->GetCamera();
	auto& cams = spCam->WorkCameraData();

	auto it = cams.find(spCam->GetCurrentName());
	if (it == cams.end()) { return Math::Vector3::Zero; }

	Math::Vector3 pos = it->second.m_target;
	pos.y = 0.0f;
	return pos;
}

std::string PrepareLayoutEditor::MakeUniqueObjectName(const std::string& _base) const
{
	auto spCamp = m_wpCampManager.lock();
	if (!spCamp) { return _base; }

	auto used = [&](const std::string& _name)
	{
		for (const auto& spObj : spCamp->GetCampObjects())
		{
			if (spObj && spObj->WorkData().m_name == _name) { return true; }
		}
		return false;
	};

	if (!used(_base)) { return _base; }

	for (int n = 2; ; ++n)
	{
		std::string name = _base + "_" + std::to_string(n);
		if (!used(name)) { return name; }
	}
}

std::string PrepareLayoutEditor::MakeUniqueLightName(const std::string& _base) const
{
	auto spCamp = m_wpCampManager.lock();
	if (!spCamp) { return _base; }

	auto used = [&](const std::string& _name)
	{
		for (const auto& spLight : spCamp->GetCampLights())
		{
			if (spLight && spLight->WorkData().m_name == _name) { return true; }
		}
		return false;
	};

	for (int n = 1; ; ++n)
	{
		std::string name = _base + "_" + std::to_string(n);
		if (!used(name)) { return name; }
	}
}

void PrepareLayoutEditor::RebuildObject(const std::shared_ptr<CampObject>& _spObj)
{
	auto spCamp = m_wpCampManager.lock();
	if (!spCamp || !_spObj) { return; }

	const CampObjectData data = _spObj->WorkData();
	spCamp->RemoveCampObject(_spObj);
	spCamp->AddCampObject(data);
}
