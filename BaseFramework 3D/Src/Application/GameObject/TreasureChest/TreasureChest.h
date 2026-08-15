#pragma once

class PlayerBase;

class TreasureChest:public KdGameObject
{
public:
	TreasureChest(){};
	~TreasureChest() override{};

	void Init()override;
	void Update()override;
	void GenerateDepthMapFromLight()override;
	void DrawLit()override;

	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayer) { m_wpPlayer = _spPlayer; };
private:
	std::weak_ptr<PlayerBase>m_wpPlayer;


	std::shared_ptr<KdModelWork>m_treasureChestModel = nullptr;
	std::shared_ptr<KdAnimator>	m_treasureChestAnimetor = nullptr;

	//空いてるかどうか
	bool m_IsOpen = false;
};
