#include "PotionTexInfo.h"

std::shared_ptr<KdTexture> PotionTexInfo::GetPotionIcon(int _ID)
{
	for (auto& tex : m_PotionTexs)
	{
		if (tex.ID == _ID)
		{
			return tex.m_Icon;
		}
	}

	return nullptr;
}

std::shared_ptr<KdTexture> PotionTexInfo::GetPotionExplanation(int _ID)
{
	for (auto& tex : m_PotionTexs)
	{
		if (tex.ID == _ID)
		{
			return tex.m_Explanation;
		}
	}

	return nullptr;
}

std::shared_ptr<KdTexture> PotionTexInfo::GetPotionName(int _ID)
{
	for (auto& tex : m_PotionTexs)
	{
		if (tex.ID == _ID)
		{
			return tex.m_Name;
		}
	}
	return nullptr;
}

std::shared_ptr<KdTexture> PotionTexInfo::GetWeaponsStrengtheningIcon(int _ID)
{
	for (auto& tex : m_WeaponsStrengtheningTexs)
	{
		if (tex.ID == _ID)
		{
			return tex.m_Icon;
		}
	}

	return nullptr;
}

std::shared_ptr<KdTexture> PotionTexInfo::GetWeaponsStrengtheningExplanation(int _ID)
{
	for (auto& tex : m_WeaponsStrengtheningTexs)
	{
		if (tex.ID == _ID)
		{
			return tex.m_Explanation;
		}
	}

	return nullptr;
}

std::shared_ptr<KdTexture> PotionTexInfo::GetWeaponsStrengtheningName(int _ID)
{
	for (auto& tex : m_WeaponsStrengtheningTexs)
	{
		if (tex.ID == _ID)
		{
			return tex.m_Name;
		}
	}
	return nullptr;
}

void PotionTexInfo::Load()
{
	{
		std::vector<Texs> inventoryList;

		std::ifstream ifs("Asset/Data/ObjeData/Item/Potions/PotionsName.json");
		nlohmann::json jsonData = nlohmann::json::parse(ifs, nullptr, false);

		for (auto& item : jsonData["inventory"])
		{
			Texs inv;
			inv.ID = item["id"].get<int>();
			inv.m_name = item["name"].get<std::string>();
			inventoryList.push_back(inv);
		}

		// ID順にソート
		std::sort(inventoryList.begin(), inventoryList.end(),
			[](const Texs& a, const Texs& b)
			{
				return a.ID < b.ID;
			});

		m_PotionTexs = inventoryList;

		for (auto& tex : m_PotionTexs)
		{
			std::shared_ptr<KdTexture>icon = std::make_shared<KdTexture>();
			std::shared_ptr<KdTexture>explanation = std::make_shared<KdTexture>();
			std::shared_ptr<KdTexture>name = std::make_shared<KdTexture>();

			std::string iconPath = m_potionPath + tex.m_name + m_Icom;
			std::string explanationPath = m_potionPath + tex.m_name + m_Explanation;
			std::string namePath = m_potionPath + tex.m_name + m_Name;

			icon->Load(iconPath);
			explanation->Load(explanationPath);
			name->Load(namePath);

			tex.m_Icon = icon;
			tex.m_Explanation = explanation;
			tex.m_Name = name;
		}
	}

	{
		std::vector<Texs> inventoryList;

		std::ifstream ifs("Asset/Data/ObjeData/Item/WeaponsStrengthening/WeaponsStrengtheningName.json");
		nlohmann::json jsonData = nlohmann::json::parse(ifs, nullptr, false);

		for (auto& item : jsonData["inventory"])
		{
			Texs inv;
			inv.ID = item["id"].get<int>();
			inv.m_name = item["name"].get<std::string>();
			inventoryList.push_back(inv);
		}

		// ID順にソート
		std::sort(inventoryList.begin(), inventoryList.end(),
			[](const Texs& a, const Texs& b)
			{
				return a.ID < b.ID;
			});

		m_WeaponsStrengtheningTexs = inventoryList;

		for (auto& tex : m_WeaponsStrengtheningTexs)
		{
			std::shared_ptr<KdTexture>icon = std::make_shared<KdTexture>();
			std::shared_ptr<KdTexture>explanation = std::make_shared<KdTexture>();
			std::shared_ptr<KdTexture>name = std::make_shared<KdTexture>();

			std::string iconPath = m_weaponsStrengtheningPath + tex.m_name + m_Icom;
			std::string explanationPath = m_weaponsStrengtheningPath + tex.m_name + m_Explanation;
			std::string namePath = m_weaponsStrengtheningPath + tex.m_name + m_Name;

			icon->Load(iconPath);
			explanation->Load(explanationPath);
			name->Load(namePath);

			tex.m_Icon = icon;
			tex.m_Explanation = explanation;
			tex.m_Name = name;
		}
	}
}
