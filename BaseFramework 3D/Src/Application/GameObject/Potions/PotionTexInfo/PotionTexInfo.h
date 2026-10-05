#pragma once

struct Texs
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


	std::shared_ptr<KdTexture> GetPotionIcon(int _ID);
	std::shared_ptr<KdTexture> GetPotionExplanation(int _ID);

	std::shared_ptr<KdTexture> GetWeaponsStrengtheningIcon(int _ID);
	std::shared_ptr<KdTexture> GetWeaponsStrengtheningExplanation(int _ID);
private:

	void Load();

	std::vector<Texs> m_PotionTexs;
	std::vector<Texs> m_WeaponsStrengtheningTexs;

	std::string m_potionPath = "Asset/Textures/GameUI/Item/Potion/";
	std::string m_weaponsStrengtheningPath = "Asset/Textures/GameUI/Item/WeaponsStrengthening/";
	std::string m_Icom = "/Icon.png";
	std::string m_Explanation = "/Explanation.png";
};
