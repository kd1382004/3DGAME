#include "SceneManager.h"

#include"../Info/MouseInfo/MouseInfo.h"
#include"../Info/KeyInfo/KeyInfo.h"

#include "BaseScene/BaseScene.h"
#include "TitleScene/TitleScene.h"
#include "GameScene/GameScene.h"
#include "ResultScene/ResultScene.h"

void SceneManager::PreUpdate()
{
	// シーン切替
	if (m_currentSceneType != m_nextSceneType)
	{
		ChangeScene(m_nextSceneType);
	}

	m_currentScene->PreUpdate();
}

void SceneManager::Update()
{
	MouseInfo::Instance().Update();
	KeyInfo::Instance().UpdateKey();

	m_currentScene->Update();
}

void SceneManager::PostUpdate()
{
	m_currentScene->PostUpdate();
}

void SceneManager::PreDraw()
{
	m_currentScene->PreDraw();
}

void SceneManager::Draw()
{
	m_currentScene->Draw();
}

void SceneManager::DrawSprite()
{

	m_currentScene->DrawSprite();

	MouseInfo::Instance().DrawSprite();
}

void SceneManager::DrawDebug()
{
	m_currentScene->DrawDebug();
}

const std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetObjList()
{
	return m_currentScene->GetObjList();
}

void SceneManager::AddObject(const std::shared_ptr<KdGameObject>& _obj)
{
	m_currentScene->AddObject(_obj);
}

void SceneManager::ImGUI()
{
	m_currentScene->ImGUi();

	//FOGImGUI
	if (ImGui::TreeNode(U8("FOG")))
	{
		ImGui::InputFloat(U8("R"), &m_FOGCol.x, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("G"), &m_FOGCol.y, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("B"), &m_FOGCol.z, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("m_FOGdensity"), &m_FOGdensity, 0.01f, 1.0f, "%.2f");
		ImGui::Checkbox(U8("Fog"), &m_fogFlg);

		if (ImGui::Button(U8("hozonn")))
		{
			FogAmbientJsonSave();
		}

		ImGui::TreePop();
	}
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogFlg, false);
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_FOGCol, m_FOGdensity);

	//FOGImGUI
	if (ImGui::TreeNode(U8("ambient")))
	{
		ImGui::InputFloat(U8("X"), &m_ambient.x, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("Y"), &m_ambient.y, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("Z"), &m_ambient.z, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("W"), &m_ambient.w, 0.01f, 1.0f, "%.2f");
		ImGui::TreePop();

		if (ImGui::Button(U8("hozonn")))
		{
			FogAmbientJsonSave();
		}

	}

	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambient);

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

		FogAmbientJsonLode();

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
	if (m_currentScene)
	{
		//ゲーム結果の保存
		m_currentScene->SetGameResult();
	}


	// 次のシーンを作成し、現在のシーンにする
	switch (_sceneType)
	{
	case SceneType::Title:
		m_currentScene = std::make_shared<TitleScene>();
		break;
	case SceneType::Game:
		m_currentScene = std::make_shared<GameScene>();
		break;
	case SceneType::Result:
		m_currentScene = std::make_shared<ResultScene>();
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


	//エフェクシアリセット
	KdEffekseerManager::GetInstance().StopAllEffect();

	m_currentScene->Init();

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
	data["Ambient"]["Color"] = { m_ambient.x, m_ambient.y, m_ambient.z, m_ambient.w };

	std::ofstream ofs(m_FogAmbientJsonFilePath);
	ofs << data.dump(4);
}

void SceneManager::FogAmbientJsonLode()
{
	std::ifstream ifs(m_FogAmbientJsonFilePath);
	if (!ifs.is_open())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] FogAmbientファイルが見つかりません : %s\n"), m_FogAmbientJsonFilePath.c_str());
		return;
	}

	nlohmann::json data = nlohmann::json::parse(ifs, nullptr, false);

	if (data.is_discarded())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] FogAmbientファイルの読み込みに失敗しました : %s\n"), m_FogAmbientJsonFilePath.c_str());
		return;
	}

	auto getFloat = [&](const nlohmann::json& j, const char* key, float def = 0.0f)
		{
			if (j.contains(key) && j[key].is_number())
				return j[key].get<float>();
			return def;
		};

	auto getArray = [&](const nlohmann::json& j, const char* key, int count)
		{
			Math::Vector4 v = { 0,0,0,0 };
			if (j.contains(key) && j[key].is_array())
			{
				for (int i = 0; i < count; i++)
				{
					if (j[key].size() > i && j[key][i].is_number())
					{
						switch (i)
						{
						case 0:
							v.x = j[key][i].get<float>();
							break;
						case 1:
							v.y = j[key][i].get<float>();
							break;
						case 2:
							v.z = j[key][i].get<float>();
							break;
						case 3:
							v.w = j[key][i].get<float>();
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
		m_ambient = col;
	}

	//距離フォグの設定
	//col...色
	//density...密度
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_FOGCol, m_FOGdensity);

	//アンビエント
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambient);
}
