#include "SceneManager.h"

#include"../Info/MouseInfo/MouseInfo.h"
#include"../Info/KeyInfo/KeyInfo.h"

#include "BaseScene/BaseScene.h"
#include "TitleScene/TitleScene.h"
#include "PrepareScene/PrepareScene.h"
#include "GameScene/GameScene.h"
#include "ResultScene/ResultScene.h"

#include"../Info/DebugInfo/DebugInfo.h"
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

const std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetObjList()
{
	return m_spCurrentScene->GetObjList();
}

void SceneManager::AddObject(const std::shared_ptr<KdGameObject>& _spObj)
{
	m_spCurrentScene->AddObject(_spObj);
}

void SceneManager::ImGUI()
{
	//FOGImGUI
	if (ImGui::TreeNode(U8("FOG")))
	{
		ImGui::InputFloat(U8("R"), &m_FOGCol.x, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("G"), &m_FOGCol.y, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("B"), &m_FOGCol.z, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("m_FOGdensity"), &m_FOGdensity, 0.01f, 1.0f, "%.2f");
		ImGui::Checkbox(U8("Fog"), &m_fogFlg);

		if (ImGui::Button(U8("変更")))
		{
			KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogFlg, false);
			KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_FOGCol, m_FOGdensity);
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
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_FOGCol, m_FOGdensity);
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambient);
}

void SceneManager::Init()
{
	{
		// 開始シーンに切り替え
		ChangeScene(m_currentSceneType);
		m_fogFlg = true;

		KdShaderManager::Instance().WorkAmbientController().SetDirLightShadowArea({ 400.0f, 400.0f }, 70.0f);

		//Fog(霧)
		//distance...距離
		//height ...高さ
		KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogFlg, false);

		FogAmbientJsonLoad();

		//距離フォグの設定
		//col...色
		//density...密度
		KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_FOGCol, m_FOGdensity);

		//アンビエント
		KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambient);
	}
}

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
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_FOGCol, m_FOGdensity);

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
	data["Fog"]["Color"] = { m_FOGCol.x, m_FOGCol.y, m_FOGCol.z };
	data["Fog"]["Density"] = m_FOGdensity;

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
		m_FOGCol = { col.x, col.y, col.z };
		m_FOGdensity = getFloat(fog, "Density", 0.02f);
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
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_FOGCol, m_FOGdensity);

	//アンビエント
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambient);
}
