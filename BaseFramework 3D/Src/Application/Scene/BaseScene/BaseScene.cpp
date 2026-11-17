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




void BaseScene::SaveInventoryData(bool _isSave, const std::vector<Inventory>& _inventoryPotionsList, const std::vector<Inventory>& _inventoryWeaponsList, std::string _filePath, long _money)
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

	// 所持金:力尽きたら(リセット)全部失う
	// 持ち帰れたら PlayerInventory の値。-1 のときは下で元のファイルから引き継ぐ
	data["money"] = (_isSave && _money >= 0) ? _money : 0;

	// 武器の所持情報はゲーム中に変わらないので、元のファイルから引き継ぐ
	// (ここで書き直すと、買った武器の情報が消えてしまうため)
	// 力尽きたら(リセット)、最初から持っているダガーだけを残して、買った武器は失う
	{
		std::ifstream oldFile(_filePath);
		if (oldFile)
		{
			nlohmann::json oldData = nlohmann::json::parse(oldFile, nullptr, false);
			if (!oldData.is_discarded() && oldData.contains("weapons"))
			{
				data["weapons"] = oldData["weapons"];

				if (!_isSave && data["weapons"].is_array())
				{
					for (auto& weapon : data["weapons"])
					{
						weapon["owned"] = (weapon.value("id", -1) == PlayerWeaponInfo_Dagger);
					}
				}
			}

			// 所持金:値が渡されなかった(-1)ときだけ、元のファイルから引き継ぐ
			if (_isSave && _money < 0 && !oldData.is_discarded() && oldData.contains("money"))
			{
				data["money"] = oldData["money"];
			}
		}
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
			WeaponsStrengtheningInfo item = infoList[i];

			if (i == WeaponsStrengthening_Body)
			{
				item.Lv = 1;
			}

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
		item.MyNodeID = i; // MyNodeID がないデータは種類の番号にしておく

		// 保存データがない項目は初期値のまま追加し、番号がずれないようにする
		if (weaponData.contains(kStrengtheningKeys[i]) && weaponData[kStrengtheningKeys[i]].is_object())
		{
			const nlohmann::json& itemData = weaponData[kStrengtheningKeys[i]];

			item.MyNodeID = itemData.value("MyNodeID", item.MyNodeID);
			item.Lv = itemData.value("Lv", item.Lv);
			item.nextLvNum = itemData.value("nextLvNum", item.nextLvNum);
			item.strengtheningItemNum = itemData.value("strengtheningItemNum", item.strengtheningItemNum);
			item.strengtheningFlg = itemData.value("strengtheningFlg", item.strengtheningFlg);
			item.LvMax = itemData.value("LvMax", item.LvMax);
			item.strengtheningItem_ID = itemData.value("strengtheningItem_ID", item.strengtheningItem_ID);
			item.InitialNextLvNum = itemData.value("InitialNextLvNum", item.InitialNextLvNum);
			item.GrowthRate = itemData.value("GrowthRate", item.GrowthRate);
			item.PreconditionNodeID = itemData.value("PreconditionNodeID", item.PreconditionNodeID);
		}

		list.push_back(item);
	}

	return list;
}

std::vector<WeaponsStrengtheningInfo> BaseScene::LoadMergedWeaponStrengtheningData(const std::string& _saveFilePath)
{
	const std::vector<WeaponsStrengtheningInfo> save = LoadWeaponStrengtheningData(_saveFilePath);   // セーブデータ
	std::vector<WeaponsStrengtheningInfo> now = LoadWeaponStrengtheningData(m_WeaponStrengtheningMSDateFilePath); // マスタを土台にする

	for (size_t i = 0; i < now.size(); ++i)
	{
		WeaponsStrengtheningInfo& n = now[i];

		// セーブにある分だけ進行状況を上書き（マスタで項目が増えてもOK）
		if (i < save.size())
		{
			n.Lv = save[i].Lv;
			n.strengtheningItemNum = save[i].strengtheningItemNum;
			n.strengtheningFlg = save[i].strengtheningFlg;
		}

		// レベルが最大を超えないようにする
		if (n.Lv > n.LvMax) { n.Lv = n.LvMax; }

		// 次のレベルに必要な数はマスタの式で計算し直す
		n.nextLvNum = static_cast<int>(std::ceil(n.InitialNextLvNum * std::pow(n.GrowthRate, n.Lv)));
	}

	return now;
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

void BaseScene::SaveGameRecord(const GameResult& _result)
{
	// 中身が空の結果(プレイしていない)は保存しない
	if (_result.displayFloorInfo.empty()) { return; }

	GameRecord record = LoadGameRecord();

	// 最新を上書き
	record.latest = _result;
	record.hasLatest = true;

	// ベスト3に入れて並べ替え、4位以下を切る
	record.best.push_back(_result);
	std::stable_sort(record.best.begin(), record.best.end(),
		[&](const GameResult& a, const GameResult& b) { return IsBetterResult(a, b); });
	if (record.best.size() > 3) { record.best.resize(3); }

	// 書き出し
	nlohmann::json data;
	data["Latest"] = GameResultToJson(record.latest);
	data["Best"] = nlohmann::json::array();
	for (const auto& r : record.best)
	{
		data["Best"].push_back(GameResultToJson(r));
	}

	std::ofstream ofs(m_GameRecordFilePath);
	ofs << data.dump(4);
}

GameRecord BaseScene::LoadGameRecord()
{
	GameRecord record;

	std::ifstream ifs(m_GameRecordFilePath);
	if (!ifs.is_open())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] GameRecordファイルが見つかりません : %s\n"), m_GameRecordFilePath.c_str());
		return record;
	}

	nlohmann::json data = nlohmann::json::parse(ifs, nullptr, false);
	if (data.is_discarded())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] GameRecordファイルの読み込みに失敗しました : %s\n"), m_GameRecordFilePath.c_str());
		return record;
	}

	if (data.contains("Latest") && data["Latest"].is_object())
	{
		record.latest = JsonToGameResult(data["Latest"]);
		record.hasLatest = true;
	}

	if (data.contains("Best") && data["Best"].is_array())
	{
		for (const auto& item : data["Best"])
		{
			record.best.push_back(JsonToGameResult(item));
		}
	}

	return record;
}

nlohmann::json BaseScene::GameResultToJson(const GameResult& _result)
{
	nlohmann::json data;
	data["PlayerDead"] = _result.playerDead;
	data["Floors"] = nlohmann::json::array();

	for (const auto& f : _result.displayFloorInfo)
	{
		nlohmann::json floor;
		floor["DisplayFloor"] = f.DisplayFloor;
		floor["EnemyKillCount"] = f.enemyKillCount;
		floor["TreasureChestsNum"] = f.TreasureChestsNum;
		floor["PlayTimeSc"] = f.playTimeSc;
		data["Floors"].push_back(floor);
	}
	return data;
}

GameResult BaseScene::JsonToGameResult(const nlohmann::json& _data)
{
	GameResult result;
	result.playerDead = _data.value("PlayerDead", false);

	if (_data.contains("Floors") && _data["Floors"].is_array())
	{
		for (const auto& item : _data["Floors"])
		{
			DisplayFloorInfo f;
			f.DisplayFloor = item.value("DisplayFloor", 0);
			f.enemyKillCount = item.value("EnemyKillCount", 0);
			f.TreasureChestsNum = item.value("TreasureChestsNum", 0);
			f.playTimeSc = item.value("PlayTimeSc", 0.0f);
			result.displayFloorInfo.push_back(f);
		}
	}
	return result;
}

bool BaseScene::IsBetterResult(const GameResult& _a, const GameResult& _b)
{
	// [0]は全体の情報
	const DisplayFloorInfo a = _a.displayFloorInfo.empty() ? DisplayFloorInfo{} : _a.displayFloorInfo[0];
	const DisplayFloorInfo b = _b.displayFloorInfo.empty() ? DisplayFloorInfo{} : _b.displayFloorInfo[0];

	// ① 到達階が深いほうが上
	if (a.DisplayFloor != b.DisplayFloor) { return a.DisplayFloor > b.DisplayFloor; }
	// ② 同じ階なら、脱出したほうが上
	if (_a.playerDead != _b.playerDead) { return !_a.playerDead; }
	// ③ それも同じなら、時間が短いほうが上
	return a.playTimeSc < b.playTimeSc;
}

void BaseScene::RestockShop()
{
	//---------- 2つのJSONを読む ----------
	auto readJson = [](const std::string& _path) -> nlohmann::json
		{
			std::ifstream file(_path);
			if (!file) { return nlohmann::json::object(); }
			nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
			if (data.is_discarded()) { return nlohmann::json::object(); }
			return data;
		};

	const nlohmann::json master = readJson(m_ShopMasterFilePath);	// 値段・最初の在庫数
	nlohmann::json       stock = readJson(m_ShopStockFilePath);	// 今の在庫

	//---------- 配列1つ分を再入荷する ----------
	// _key … "potions" / "materials" / "weapons"
	auto restock = [&](const char* _key)
		{
			if (!master.contains(_key) || !stock.contains(_key)) { return; }

			for (auto& item : stock[_key])
			{
				// 在庫が残っている商品はそのまま
				if (item.value("stock", 0) > 0) { continue; }

				// マスターから、同じidの最初の在庫数を探す
				const int id = item.value("id", -1);
				for (const auto& masterItem : master[_key])
				{
					if (masterItem.value("id", -1) == id)
					{
						item["stock"] = masterItem.value("startStock", 0);
						break;
					}
				}
			}
		};

	restock("potions");
	restock("materials");
	restock("weapons");

	//---------- 保存 ----------
	std::ofstream file(m_ShopStockFilePath);
	if (!file) { return; }
	file << stock.dump(4);
}


std::vector<bool> BaseScene::LoadOwnedWeapons()
{
	// ファイルや項目が無いときは、ダガー(最初の武器)だけ持っている状態にする
	std::vector<bool> owned(PlayerWeaponInfoSize, false);
	owned[PlayerWeaponInfo_Dagger] = true;

	std::ifstream file(m_InventoryFilePath);
	if (!file) { return owned; }

	nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
	if (data.is_discarded() || !data.contains("weapons") || !data["weapons"].is_array()) { return owned; }

	for (const auto& weapon : data["weapons"])
	{
		int id = weapon.value("id", -1);
		if (id < 0 || id >= PlayerWeaponInfoSize) { continue; }

		owned[id] = weapon.value("owned", false);
	}

	return owned;
}

void BaseScene::SaveOwnedWeapons(const std::vector<bool>& _owned)
{
	// ポーションなど他の項目は消さずに、"weapons" だけを書き換える
	nlohmann::json data = nlohmann::json::object();
	{
		std::ifstream file(m_InventoryFilePath);
		if (file)
		{
			data = nlohmann::json::parse(file, nullptr, false);
			if (data.is_discarded()) { data = nlohmann::json::object(); }
		}
	}

	const std::string weaponNames[PlayerWeaponInfoSize] = { "Dagger", "Sword" };	// PlayerWeaponInfoの順

	data["weapons"] = nlohmann::json::array();
	for (int id = 0; id < PlayerWeaponInfoSize; id++)
	{
		bool owned = (id < static_cast<int>(_owned.size())) ? static_cast<bool>(_owned[id]) : false;
		data["weapons"].push_back({
			{"id", id},
			{"name", weaponNames[id]},
			{"owned", owned}
			});
	}

	std::ofstream file(m_InventoryFilePath);
	if (!file) { return; }
	file << data.dump(4);
}


long BaseScene::LoadMoney()
{
	std::ifstream file(m_InventoryFilePath);
	if (!file) { return 0; }

	nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
	if (data.is_discarded()) { return 0; }

	return data.value("money", 0L);
}

void BaseScene::SaveMoney(long _money)
{
	// ポーションなど他の項目は消さずに、"money" だけを書き換える
	nlohmann::json data = nlohmann::json::object();
	{
		std::ifstream file(m_InventoryFilePath);
		if (file)
		{
			data = nlohmann::json::parse(file, nullptr, false);
			if (data.is_discarded()) { data = nlohmann::json::object(); }
		}
	}

	data["money"] = std::max(0L, _money);	// マイナスにはしない

	std::ofstream file(m_InventoryFilePath);
	if (!file) { return; }
	file << data.dump(4);
}
