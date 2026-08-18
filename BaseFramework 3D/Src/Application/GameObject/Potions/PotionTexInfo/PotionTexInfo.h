#pragma once

struct PotionTexs
{
	int ID;
	std::string m_name;
	std::shared_ptr<KdTexture>m_Icon;
	std::shared_ptr<KdTexture>m_Explanation;
};


class PotionTexInfo
{
public:
	PotionTexInfo() { Load(); };
	~PotionTexInfo() {};


	std::shared_ptr<KdTexture> GetIcon(int _ID);
	std::shared_ptr<KdTexture> GetExplanation(int _ID);
private:

	void Load();

	std::vector<PotionTexs> m_PotionTexs;

	std::string m_path = "Asset/Textures/GameUI/Item/Potion/";
	std::string m_Icom = "/Icon.png";
	std::string m_Explanation = "/Explanation.png";
};
