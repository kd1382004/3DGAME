#pragma once

#pragma once


class TreasureChest;
class PlayerBase;

class TreasureChestManager :public KdGameObject
{
public:
	TreasureChestManager() {};
	~TreasureChestManager() override {};

	void Init()override;
	void Update()override;
	void GenerateDepthMapFromLight()override;
	void DrawLit()override;

	void GenerateTreasureChest(std::list<Math::Vector3> _treasureChestPosList);


	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayer) { m_wpPlayer = _spPlayer; };
private:
	std::weak_ptr<PlayerBase>m_wpPlayer;


	std::list<std::shared_ptr<TreasureChest>> m_treasureChestList;
	};
