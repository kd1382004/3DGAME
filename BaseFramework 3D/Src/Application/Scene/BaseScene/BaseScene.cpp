#include "BaseScene.h"
#include"../../Info/FadeDraw/FadeDraw.h"


#include"../SceneManager.h"


#include"../../GameObject/Character/Player/PlayerInventory/PlayerInventory.h"
#include"../../GameObject/Weapon/WeaponBase.h"
void BaseScene::PreUpdate()
{
	// Updateの前の更新処理
	// オブジェクトリストの整理 ・・・ 無効なオブジェクトを削除
	auto it = m_objList.begin();

	while (it != m_objList.end())
	{
		if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_objList.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}

	// ↑の後には有効なオブジェクトだけのリストになっている


	for (auto& obj : m_objList)
	{
		if (m_poseFlg)
		{
			if (!obj->GetPauseStop())
			{
				obj->PreUpdate();
			}
		}
		else
		{
			obj->PreUpdate();
		}
	}
}

void BaseScene::Update()
{
	// シーン毎のイベント処理
	if (!m_poseFlg)
	{
		Event();
	}

	// KdGameObjectを継承した全てのオブジェクトの更新 (ポリモーフィズム)
	for (auto& obj : m_objList)
	{
		if (m_poseFlg)
		{
			if (!obj->GetPauseStop())
			{
				obj->Update();
			}
		}
		else
		{
			obj->Update();
		}
	}

	FadeDraw::Instance().Update();
}

void BaseScene::PostUpdate()
{

	for (auto& obj : m_objList)
	{
		if (m_poseFlg)
		{
			if (!obj->GetPauseStop())
			{
				obj->PostUpdate();
			}
		}
		else
		{
			obj->PostUpdate();
		}
	}

}

void BaseScene::PreDraw()
{
	for (auto& obj : m_objList)
	{
		obj->PreDraw();
	}
}

void BaseScene::Draw()
{
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光を遮るオブジェクト(影を生み出す要因となるオブジェクト)をBeginとEndの間にまとめてDrawする


	/*KdShaderManager::Instance().m_StandardShader.BeginGenerateDepthMapFromLight();
	{
		for (auto& obj : m_objList)
		{
			obj->GenerateDepthMapFromLight();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndGenerateDepthMapFromLight();*/

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// スポットライト用のシャドウマップ生成
	if (KdShaderManager::Instance().WorkAmbientController().IsSpotLightShadowEnable())
	{
		KdShaderManager::Instance().m_StandardShader.BeginGenerateDepthMapFromSpotLight(
			KdShaderManager::Instance().WorkAmbientController().GetSpotLightVP()
		);
		{
			for (auto& obj : m_objList)
			{
				obj->GenerateDepthMapFromLight();
			}
		}
		KdShaderManager::Instance().m_StandardShader.EndGenerateDepthMapFromSpotLight();
	}

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のないオブジェクト(背景など)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawUnLit();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のあるオブジェクト(光源の影響を受けるオブジェクト)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawLit();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のないオブジェクト(エフェクトなど)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawEffect();
		}


		// 描画処理
		KdEffekseerManager::GetInstance().Draw();
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光源オブジェクト(自ら光るオブジェクトやエフェクト)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_postProcessShader.BeginBright();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawBright();
		}
	}
	KdShaderManager::Instance().m_postProcessShader.EndBright();
}

void BaseScene::DrawSprite()
{
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 2Dの描画はこの間で行う
	KdShaderManager::Instance().m_spriteShader.Begin();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawSprite();
		}
	}
	KdShaderManager::Instance().m_spriteShader.End();


	FadeDraw::Instance().DrawSprite();
}

void BaseScene::DrawDebug()
{
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// デバッグ情報の描画はこの間で行う
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawDebug();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();
}


void BaseScene::ImGUi()
{

}

void BaseScene::Event()
{
	// 各シーンで必要な内容を実装(オーバーライド)する
}

void BaseScene::Init()
{
	// 各シーンで必要な内容を実装(オーバーライド)する
}




void BaseScene::SaveInventoryData(bool _isSave, const std::vector<Inventory>& _inventoryPotionsList, const std::vector<Inventory>& _inventoryWeaponsList, std::string _filePath)
{
	nlohmann::json data;

	// アイテムが0個のときも、nullではなく [] として保存する
	data["potions"] = nlohmann::json::array();
	data["weaponStrengtheningMaterials"] = nlohmann::json::array();

	for (const Inventory& potion : _inventoryPotionsList)
	{
		int num = 0;
		if (_isSave)
		{
			num = potion.m_num;
		}

		data["potions"].push_back({
			{"id", potion.m_ID},
			{"num", num},
			{"name", potion.m_name}
			});
	}

	for (const Inventory& weapon : _inventoryWeaponsList)
	{
		int num = 0;
		if (_isSave)
		{
			num = weapon.m_num;
		}

		data["weaponStrengtheningMaterials"].push_back({
			{"id", weapon.m_ID},
			{"num", num},
			{"name", weapon.m_name}
			});
	}

	std::ofstream file(_filePath);
	if (!file) return; // 必要に応じてエラー処理を追加

	file << data.dump(4);

}

std::vector<std::vector<Inventory>> BaseScene::LoadInventoryData(std::string _filePath)
{
	std::ifstream file(_filePath);
	if (!file) return { {}, {} }; // 初回起動などでファイルがない場合は空のリストを返す


	nlohmann::json data = nlohmann::json::parse(file);

	std::vector<Inventory> potions = ReadInventoryList(data, "potions");
	std::vector<Inventory> materials = ReadInventoryList(data, "weaponStrengtheningMaterials");

	return { potions, materials };
}

std::vector<Inventory> BaseScene::ReadInventoryList(const nlohmann::json& _data, const std::string& _key)
{
	std::vector<Inventory> list;

	if (!_data.contains(_key) || !_data[_key].is_array())
	{
		return list;
	}

	for (const nlohmann::json& itemData : _data[_key])
	{
		Inventory item;
		item.m_ID = itemData.value("id", 0);
		item.m_num = itemData.value("num", 0);
		item.m_name = itemData.value("name", std::string{});
		list.push_back(item);
	}

	return list;
}

void BaseScene::SaveWeaponStrengtheningData(bool _isSave, std::string _filePath, std::vector<WeaponsStrengtheningInfo> weaponStrengtheningList)
{
	// 他の武器のデータを残すために1度開く（ファイルがなくても続行する）
	nlohmann::json oldData = nlohmann::json::object();
	{
		std::ifstream oldFile(_filePath);
		if (oldFile)
		{
			oldData = nlohmann::json::parse(oldFile, nullptr, false);
			if (!oldData.is_object())
			{
				oldData = nlohmann::json::object();
			}
		}
	}

	nlohmann::json data = nlohmann::json::object();
	const int currentWeapon = SceneManager::Instance().GetPlayerWeaponInfo();

	// 武器ごとのループ
	for (int weapon = 0; weapon < PlayerWeaponInfo::PlayerWeaponInfoSize; ++weapon)
	{
		const std::string key = GetWeaponKey(static_cast<PlayerWeaponInfo>(weapon));

		std::vector<WeaponsStrengtheningInfo> infoList;

		if (_isSave)
		{
			if (weapon == currentWeapon)
			{
				infoList = weaponStrengtheningList;
			}
			else
			{
				infoList = ReadWeaponStrengtheningList(oldData, key);
			}
		}

		// falseのときや保存データがないときは、初期値で全項目を作る
		infoList.resize(WeaponsStrengtheningSiz);

		// 強化項目ごとのループ
		nlohmann::json weaponData = nlohmann::json::object();
		for (int i = 0; i < WeaponsStrengtheningSiz; ++i)
		{
			const WeaponsStrengtheningInfo& item = infoList[i];

			weaponData[kStrengtheningKeys[i]] = {
				{"Lv", item.Lv},
				{"nextLvNum", item.nextLvNum},
				{"strengtheningItemNum", item.strengtheningItemNum},
				{"strengtheningFlg", item.strengtheningFlg}
			};
		}

		data[key] = weaponData;
	}

	// ファイルに書き込む
	std::ofstream file(_filePath);
	if (!file) return; // 必要に応じてエラー処理を追加

	file << data.dump(4);
}
std::vector<WeaponsStrengtheningInfo> BaseScene::LoadWeaponStrengtheningData(std::string _filePath)
{
	std::ifstream file(_filePath);
	if (!file) return {}; // 初回起動などでファイルがない場合は何もしない
	nlohmann::json data = nlohmann::json::parse(file);

	std::string _key = GetWeaponKey(SceneManager::Instance().GetPlayerWeaponInfo());

	return ReadWeaponStrengtheningList(data, _key);

}

std::vector<WeaponsStrengtheningInfo> BaseScene::ReadWeaponStrengtheningList(const nlohmann::json& _data, const std::string& _key)
{
	std::vector<WeaponsStrengtheningInfo> list;

	if (!_data.contains(_key) || !_data[_key].is_object())
	{
		return list;
	}

	const nlohmann::json& weaponData = _data[_key];

	for (int i = 0; i < WeaponsStrengtheningSiz; ++i)
	{
		WeaponsStrengtheningInfo item;

		// 保存データがない項目は初期値のまま追加し、番号がずれないようにする
		if (weaponData.contains(kStrengtheningKeys[i]) && weaponData[kStrengtheningKeys[i]].is_object())
		{
			const nlohmann::json& itemData = weaponData[kStrengtheningKeys[i]];

			item.Lv = itemData.value("Lv", 0);
			item.nextLvNum = itemData.value("nextLvNum", 1);
			item.strengtheningItemNum = itemData.value("strengtheningItemNum", 0);
			item.strengtheningFlg = itemData.value("strengtheningFlg", false);
		}

		list.push_back(item);
	}

	return list;
}

std::string BaseScene::GetWeaponKey(int _weaponType)
{
	switch (_weaponType)
	{
	case PlayerWeaponInfo_Dagger: return "dagger";
	case PlayerWeaponInfo_Sword:  return "sword";
	default:                      return "";
	}
}
