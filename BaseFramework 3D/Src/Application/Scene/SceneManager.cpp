//---- 自分のヘッダー ----
#include "SceneManager.h"					//シーンの管理

//---- シーン ----
#include "BaseScene/BaseScene.h"			//シーンの基底クラス
#include "TitleScene/TitleScene.h"			//タイトルシーン
#include "PrepareScene/PrepareScene.h"		//準備シーン
#include "GameScene/GameScene.h"			//ゲームシーン
#include "ResultScene/ResultScene.h"		//リザルトシーン

//---- 便利機能 ----
#include"../Info/MouseInfo/MouseInfo.h"		//マウス入力
#include"../Info/KeyInfo/KeyInfo.h"			//キー入力
#include"../Info/DebugInfo/DebugInfo.h"		//デバッグ用の状態

//=============================================
// 基本の処理
//=============================================

void SceneManager::PreUpdate()
{
	// シーン切替
	if (m_currentSceneType != m_nextSceneType)
	{
		ChangeScene(m_nextSceneType);
	}

	if (DebugInfo::Instance().GetSceneManagerImGUIFlg())
	{
		return;
	}

	m_spCurrentScene->PreUpdate();
}

void SceneManager::Update()
{
	MouseInfo::Instance().Update();
	KeyInfo::Instance().UpdateKey();


	if (DebugInfo::Instance().GetSceneManagerImGUIFlg())
	{
		MouseInfo::Instance().SetMouseFreeFlg(true);
		while (ShowCursor(true) < 0) {}    // カウンターが0以上になるまで上げる＝必ず表示
		return;
	}
	else
	{
		while (ShowCursor(false) >= 0) {}  // カウンターが負になるまで下げる＝必ず非表示
	}

	m_spCurrentScene->Update();
}

void SceneManager::PostUpdate()
{
	if (DebugInfo::Instance().GetSceneManagerImGUIFlg())
	{
		return;
	}

	m_spCurrentScene->PostUpdate();
}

void SceneManager::PreDraw()
{
	m_spCurrentScene->PreDraw();
}

void SceneManager::Draw()
{
	m_spCurrentScene->Draw();
}

void SceneManager::DrawSprite()
{

	m_spCurrentScene->DrawSprite();

	MouseInfo::Instance().DrawSprite();
}

void SceneManager::DrawDebug()
{
	m_spCurrentScene->DrawDebug();
}

void SceneManager::ImGUI()
{
	//---- どのシーンでも使うデバッグ設定 ----
	if (ImGui::TreeNode(U8("デバッグ表示")))
	{
		//当たり判定(m_pDebugWire)を描画するか
		ImGui::Checkbox(U8("当たり判定を表示"), DebugInfo::Instance().WorkDrawDebugWireFlg());
		ImGui::TreePop();
	}

	//FOGImGUI
	if (ImGui::TreeNode(U8("FOG")))
	{
		ImGui::InputFloat(U8("R"), &m_fogColor.x, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("G"), &m_fogColor.y, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("B"), &m_fogColor.z, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("m_FOGdensity"), &m_fogDensity, 0.01f, 1.0f, "%.2f");
		ImGui::Checkbox(U8("Fog"), &m_fogFlg);

		if (ImGui::Button(U8("変更")))
		{
			KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogFlg, false);
			KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_fogColor, m_fogDensity);
		}


		if (ImGui::Button(U8("保存")))
		{
			FogAmbientJsonSave();
		}

		ImGui::TreePop();
	}


	//FOGImGUI
	if (ImGui::TreeNode(U8("ambient")))
	{
		ImGui::InputFloat(U8("X"), &m_ambient.x, 0.01f, 1.0f, "%.3f");
		ImGui::InputFloat(U8("Y"), &m_ambient.y, 0.01f, 1.0f, "%.3f");
		ImGui::InputFloat(U8("Z"), &m_ambient.z, 0.01f, 1.0f, "%.3f");
		ImGui::InputFloat(U8("W"), &m_ambient.w, 0.01f, 1.0f, "%.3f");
		ImGui::TreePop();

		if (ImGui::Button(U8("変更")))
		{
			KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambient);

			m_defaultAmbient = m_ambient;
		}

		if (ImGui::Button(U8("保存")))
		{
			FogAmbientJsonSave();
		}

	}

	m_spCurrentScene->ImGUI();


	if (ImGui::Button(U8("ImGuiを閉じる")))
	{
		DebugInfo::Instance().SetSceneManagerImGUIFlg(false);
		MouseInfo::Instance().SetMouseFreeFlg(false);
	}
}

//=============================================
// 取得
//=============================================

const std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetObjList()
{
	return m_spCurrentScene->GetObjList();
}

//=============================================
// 設定
//=============================================

void SceneManager::AddObject(const std::shared_ptr<KdGameObject>& _spObj)
{
	m_spCurrentScene->AddObject(_spObj);
}

//=============================================
// その他の処理
//=============================================

void SceneManager::ChangeRendertarget(std::shared_ptr<KdRenderTargetPack> _spRtPack)
{
	if (!_spRtPack) { return; }
	_spRtPack->ClearTexture();
	m_rtChanger.ChangeRenderTarget(*_spRtPack);
}

void SceneManager::UndoRenderTarget()
{
	m_rtChanger.UndoRenderTarget();
}

void SceneManager::UnFogAmbient()
{
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogFlg, false);
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_fogColor, m_fogDensity);
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambient);
}

//=============================================
// 基本の処理
//=============================================

void SceneManager::Init()
{
	{
		// 前回の装備を読み込む(シーンより先に読んでおく)
		PlayerWeaponJsonLoad();

		// 開始シーンに切り替え
		ChangeScene(m_currentSceneType);
		m_fogFlg = true;

		//Fog(霧)
		//distance...距離
		//height ...高さ
		KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogFlg, false);

		FogAmbientJsonLoad();

		//距離フォグの設定
		//col...色
		//density...密度
		KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_fogColor, m_fogDensity);

		//アンビエント
		KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambient);
	}
}

//=============================================
// その他の処理
//=============================================

void SceneManager::ChangeScene(SceneType _sceneType)
{
	if (m_spCurrentScene)
	{
		//ゲーム結果の保存
		m_spCurrentScene->SetGameResult();
	}


	// 次のシーンを作成し、現在のシーンにする
	switch (_sceneType)
	{
	case SceneType::Title:
		m_spCurrentScene = std::make_shared<TitleScene>();
		break;
	case SceneType::Prepare:
		m_spCurrentScene = std::make_shared<PrepareScene>();
		break;
	case SceneType::Game:
		m_spCurrentScene = std::make_shared<GameScene>();
		break;
	case SceneType::Result:
		m_spCurrentScene = std::make_shared<ResultScene>();
		break;
	}



	//Fog(霧)
	//distance...距離
	//height ...高さ
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogFlg, false);

	//距離フォグの設定
	//col...色
	//density...密度
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_fogColor, m_fogDensity);

	//アンビエント
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambient);


	SetFogEnable(true);
	SetDefaultAmbient();

	//エフェクシアリセット
	KdEffekseerManager::GetInstance().StopAllEffect();

	m_spCurrentScene->Init();

	// 現在のシーン情報を更新
	m_currentSceneType = _sceneType;
}

void SceneManager::FogAmbientJsonSave()
{
	nlohmann::json data;

	// Fog
	data["Fog"]["Color"] = { m_fogColor.x, m_fogColor.y, m_fogColor.z };
	data["Fog"]["Density"] = m_fogDensity;

	// Ambient
	data["Ambient"]["Color"] = { m_defaultAmbient.x, m_defaultAmbient.y, m_defaultAmbient.z, m_defaultAmbient.w };

	std::ofstream ofs(m_fogAmbientJsonFilePath);
	ofs << data.dump(4);
}

void SceneManager::FogAmbientJsonLoad()
{
	std::ifstream ifs(m_fogAmbientJsonFilePath);
	if (!ifs.is_open())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] FogAmbientファイルが見つかりません : %s\n"), m_fogAmbientJsonFilePath.c_str());
		return;
	}

	nlohmann::json data = nlohmann::json::parse(ifs, nullptr, false);

	if (data.is_discarded())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] FogAmbientファイルの読み込みに失敗しました : %s\n"), m_fogAmbientJsonFilePath.c_str());
		return;
	}

	auto getFloat = [&](const nlohmann::json& _j, const char* _key, float _def = 0.0f)
		{
			if (_j.contains(_key) && _j[_key].is_number())
				return _j[_key].get<float>();
			return _def;
		};

	auto getArray = [&](const nlohmann::json& _j, const char* _key, int _count)
		{
			Math::Vector4 v = { 0,0,0,0 };
			if (_j.contains(_key) && _j[_key].is_array())
			{
				for (int i = 0; i < _count; i++)
				{
					if (_j[_key].size() > i && _j[_key][i].is_number())
					{
						switch (i)
						{
						case 0:
							v.x = _j[_key][i].get<float>();
							break;
						case 1:
							v.y = _j[_key][i].get<float>();
							break;
						case 2:
							v.z = _j[_key][i].get<float>();
							break;
						case 3:
							v.w = _j[_key][i].get<float>();
							break;
						default:
							break;
						}
					}

				}
			}
			return v;
		};

	// Fog
	{
		auto fog = data["Fog"];
		Math::Vector4 col = getArray(fog, "Color", 3);
		m_fogColor = { col.x, col.y, col.z };
		m_fogDensity = getFloat(fog, "Density", 0.02f);
	}

	// Ambient
	{
		auto amb = data["Ambient"];
		Math::Vector4 col = getArray(amb, "Color", 4);
		m_defaultAmbient = col;
		m_ambient = m_defaultAmbient;
	}

	//距離フォグの設定
	//col...色
	//density...密度
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_fogColor, m_fogDensity);

	//アンビエント
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambient);
}

void SceneManager::PlayerWeaponJsonSave()
{
	nlohmann::json data;
	data["equippedWeapon"] = static_cast<int>(m_playerWeaponInfo);

	std::ofstream ofs(m_playerWeaponJsonFilePath);
	if (!ofs) { return; }
	ofs << data.dump(4);
}

void SceneManager::PlayerWeaponJsonLoad()
{
	std::ifstream ifs(m_playerWeaponJsonFilePath);
	if (!ifs) { return; }	// 初回起動などでファイルが無いときはダガーのまま

	nlohmann::json data = nlohmann::json::parse(ifs, nullptr, false);
	if (data.is_discarded()) { return; }

	int weapon = data.value("equippedWeapon", static_cast<int>(PlayerWeaponInfo_Dagger));

	// 範囲外の値ならダガーにする
	if (weapon < 0 || weapon >= PlayerWeaponInfoSize) { weapon = PlayerWeaponInfo_Dagger; }

	// ここではSetPlayerWeaponInfoを使わない(読み込んだだけで保存しないため)
	m_playerWeaponInfo = static_cast<PlayerWeaponInfo>(weapon);
}
