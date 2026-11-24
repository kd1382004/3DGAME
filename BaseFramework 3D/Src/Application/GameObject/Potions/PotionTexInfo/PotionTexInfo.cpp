//---- 自分のヘッダー ----
#include "PotionTexInfo.h"					//アイテムの名前・画像情報

//---- シーン ----
#include"../../../Scene/SceneManager.h"		//シーンの管理

//=============================================
// 取得
//=============================================

std::shared_ptr<KdTexture> PotionTexInfo::GetPotionIcon(int _ID)
{
	for (auto& tex : m_potionTexs)
	{
		if (tex.m_ID == _ID)
		{
			return tex.m_spIcon;
		}
	}

	return nullptr;
}

std::shared_ptr<KdTexture> PotionTexInfo::GetPotionExplanation(int _ID)
{
	for (auto& tex : m_potionTexs)
	{
		if (tex.m_ID == _ID)
		{
			return tex.m_spExplanation;
		}
	}

	return nullptr;
}

std::shared_ptr<KdTexture> PotionTexInfo::GetPotionName(int _ID)
{
	for (auto& tex : m_potionTexs)
	{
		if (tex.m_ID == _ID)
		{
			return tex.m_spName;
		}
	}
	return nullptr;
}

std::shared_ptr<KdTexture> PotionTexInfo::GetWeaponsStrengtheningIcon(int _ID)
{
	for (auto& tex : m_weaponsStrengtheningTexs)
	{
		if (tex.m_ID == _ID)
		{
			return tex.m_spIcon;
		}
	}

	return nullptr;
}

std::shared_ptr<KdTexture> PotionTexInfo::GetWeaponsStrengtheningExplanation(int _ID)
{
	for (auto& tex : m_weaponsStrengtheningTexs)
	{
		if (tex.m_ID == _ID)
		{
			return tex.m_spExplanation;
		}
	}

	return nullptr;
}

std::shared_ptr<KdTexture> PotionTexInfo::GetWeaponsStrengtheningName(int _ID)
{
	for (auto& tex : m_weaponsStrengtheningTexs)
	{
		if (tex.m_ID == _ID)
		{
			return tex.m_spName;
		}
	}
	return nullptr;
}

std::shared_ptr<KdTexture> PotionTexInfo::GetWeaponsStrengtheningItemIcon(int _ID)
{
	for (auto& tex : m_weaponsStrengtheningItemTexs)
	{
		if (tex.m_ID == _ID)
		{
			return tex.m_spIcon;
		}
	}

	return nullptr;
}

std::shared_ptr<KdTexture> PotionTexInfo::GetWeaponsStrengtheningItemName(int _ID)
{
	for (auto& tex : m_weaponsStrengtheningItemTexs)
	{
		if (tex.m_ID == _ID)
		{
			return tex.m_spName;
		}
	}

	return nullptr;
}

std::shared_ptr<KdTexture> PotionTexInfo::GetWeaponsStrengtheningItemExplanation(int _ID)
{
	for (auto& tex : m_weaponsStrengtheningItemTexs)
	{
		if (tex.m_ID == _ID)
		{
			return tex.m_spExplanation;
		}
	}

	return nullptr;
}

//=============================================
// その他の処理
//=============================================

void PotionTexInfo::Load()
{
	{
		std::vector<Texs> inventoryList;

		std::ifstream ifs("Asset/Data/ObjeData/Item/Potions/PotionsName.json");
		nlohmann::json jsonData = nlohmann::json::parse(ifs, nullptr, false);

		for (auto& item : jsonData["inventory"])
		{
			Texs inv;
			inv.m_ID = item["id"].get<int>();
			inv.m_name = item["name"].get<std::string>();
			inventoryList.push_back(inv);
		}

		// ID順にソート
		std::sort(inventoryList.begin(), inventoryList.end(),
			[](const Texs& _a, const Texs& _b)
			{
				return _a.m_ID < _b.m_ID;
			});

		m_potionTexs = inventoryList;

		for (auto& tex : m_potionTexs)
		{
			std::shared_ptr<KdTexture>spIcon = std::make_shared<KdTexture>();
			std::shared_ptr<KdTexture>spExplanation = std::make_shared<KdTexture>();
			std::shared_ptr<KdTexture>spName = std::make_shared<KdTexture>();

			std::string iconPath = m_potionPath + tex.m_name + m_icon;
			std::string explanationPath = m_potionPath + tex.m_name + m_explanation;
			std::string namePath = m_potionPath + tex.m_name + m_name;

			spIcon->Load(iconPath);
			spExplanation->Load(explanationPath);
			spName->Load(namePath);

			tex.m_spIcon = spIcon;
			tex.m_spExplanation = spExplanation;
			tex.m_spName = spName;
		}
	}

	{
		std::vector<Texs> inventoryList;

		std::ifstream ifs("Asset/Data/ObjeData/Item/WeaponsStrengthening/WeaponsStrengtheningName.json");
		nlohmann::json jsonData = nlohmann::json::parse(ifs, nullptr, false);

		for (auto& item : jsonData["inventory"])
		{
			Texs inv;
			inv.m_ID = item["id"].get<int>();
			inv.m_name = item["name"].get<std::string>();
			inventoryList.push_back(inv);
		}

		// ID順にソート
		std::sort(inventoryList.begin(), inventoryList.end(),
			[](const Texs& _a, const Texs& _b)
			{
				return _a.m_ID < _b.m_ID;
			});

		m_weaponsStrengtheningTexs = inventoryList;

		for (auto& tex : m_weaponsStrengtheningTexs)
		{
			std::shared_ptr<KdTexture>spIcon = std::make_shared<KdTexture>();
			std::shared_ptr<KdTexture>spExplanation = std::make_shared<KdTexture>();
			std::shared_ptr<KdTexture>spName = std::make_shared<KdTexture>();

			std::string iconPath;
			std::string explanationPath;
			std::string namePath;
			std::string playerWeapon;
			if (tex.m_ID >= 4)
			{
				switch (SceneManager::Instance().GetPlayerWeaponInfo())
				{
				case PlayerWeaponInfo::PlayerWeaponInfo_Dagger:
					playerWeapon = "/Dagger/";
					break;
				case PlayerWeaponInfo::PlayerWeaponInfo_Sword:
					playerWeapon = "/Sword/";
					break;

				default:
					break;
				}


				iconPath = m_weaponsStrengtheningPath + playerWeapon + tex.m_name + m_icon;
				explanationPath = m_weaponsStrengtheningPath + playerWeapon + tex.m_name + m_explanation;
				namePath = m_weaponsStrengtheningPath + playerWeapon + tex.m_name + m_name;
			}
			else
			{
				iconPath = m_weaponsStrengtheningPath + tex.m_name + m_icon;
				explanationPath = m_weaponsStrengtheningPath + tex.m_name + m_explanation;
				namePath = m_weaponsStrengtheningPath + tex.m_name + m_name;
			}




			spIcon->Load(iconPath);
			spExplanation->Load(explanationPath);
			spName->Load(namePath);

			tex.m_spIcon = spIcon;
			tex.m_spExplanation = spExplanation;
			tex.m_spName = spName;
		}
	}


	{
		std::vector<Texs> inventoryList;

		std::ifstream ifs("Asset/Data/ObjeData/Item/WeaponsStrengtheningItem/WeaponsStrengtheningItemName.json");
		nlohmann::json jsonData = nlohmann::json::parse(ifs, nullptr, false);

		for (auto& item : jsonData["inventory"])
		{
			Texs inv;
			inv.m_ID = item["id"].get<int>();
			inv.m_name = item["name"].get<std::string>();
			inventoryList.push_back(inv);
		}

		// ID順にソート
		std::sort(inventoryList.begin(), inventoryList.end(),
			[](const Texs& _a, const Texs& _b)
			{
				return _a.m_ID < _b.m_ID;
			});

		m_weaponsStrengtheningItemTexs = inventoryList;

		for (auto& tex : m_weaponsStrengtheningItemTexs)
		{
			std::shared_ptr<KdTexture>spIcon = std::make_shared<KdTexture>();
			std::shared_ptr<KdTexture>spExplanation = std::make_shared<KdTexture>();
			std::shared_ptr<KdTexture>spName = std::make_shared<KdTexture>();

			// アイコンの画像（Icon.png）
			std::string iconPath = m_weaponsStrengtheningItemPath + tex.m_name + m_icon;
			spIcon->Load(iconPath);

			// 説明の画像（SelectExplanationBase.png）
			std::string explanationPath = m_weaponsStrengtheningItemPath + tex.m_name + m_explanation;
			spExplanation->Load(explanationPath);

			// 名前の画像（SelectNameBase.png）
			std::string namePath = m_weaponsStrengtheningItemPath + tex.m_name + m_name;
			spName->Load(namePath);

			tex.m_spIcon = spIcon;
			tex.m_spExplanation = spExplanation;
			tex.m_spName = spName;
		}
	}
}
