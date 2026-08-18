#include "PotionTexInfo.h"

std::shared_ptr<KdTexture> PotionTexInfo::GetIcon(int _ID)
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

std::shared_ptr<KdTexture> PotionTexInfo::GetExplanation(int _ID)
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


void PotionTexInfo::Load()
{
	std::vector<PotionTexs> inventoryList;

	std::ifstream ifs("Asset/Data/ObjeData/Item/Potions/PotionsName.json");
	nlohmann::json jsonData = nlohmann::json::parse(ifs, nullptr, false);

	for (auto& item : jsonData["inventory"])
	{
		PotionTexs inv;
		inv.ID = item["id"].get<int>();
		inv.m_name = item["name"].get<std::string>();
		inventoryList.push_back(inv);
	}

	// ID順にソート
	std::sort(inventoryList.begin(), inventoryList.end(),
		[](const PotionTexs& a, const PotionTexs& b)
		{
			return a.ID < b.ID;
		});

	m_PotionTexs = inventoryList;

	for (auto& tex : m_PotionTexs)
	{
		std::shared_ptr<KdTexture>icon = std::make_shared<KdTexture>();
		std::shared_ptr<KdTexture>explanation = std::make_shared<KdTexture>();

		std::string iconPath = m_path + tex.m_name + m_Icom;
		std::string explanationPath = m_path + tex.m_name + m_Explanation;

		icon->Load(iconPath);
		explanation->Load(explanationPath);

		tex.m_Icon = icon;
		tex.m_Explanation = explanation;
	}
}
