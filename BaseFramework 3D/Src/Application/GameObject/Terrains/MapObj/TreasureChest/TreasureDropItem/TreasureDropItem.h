#pragma once

class PlayerBase;
class LootTableManager;

class TreasureDropItem :public KdGameObject
{
public:
	TreasureDropItem() {}
	 ~TreasureDropItem()override {};

	 void Init()override;
	 void PreUpdate()override;
	 void Update()override;
	 void DrawEffect()override;
	
	 void SetLootTableManager(std::shared_ptr<LootTableManager> _spLootTableManager) { m_wpLootTableManager = _spLootTableManager; };
	 void SetPlayer(std::shared_ptr<PlayerBase> _spPlayer) { m_wpPlayer = _spPlayer; };

	void SetPos(const Math::Vector3& pos) override
	{
		m_mWorld.Translation(pos); 
		m_pos = pos;
	}

	void SetPath(std::string path) { m_path = path; }
private:
	std::shared_ptr<KdModelData>m_dropItemModel = nullptr;
	std::weak_ptr<PlayerBase>m_wpPlayer;
	std::weak_ptr<LootTableManager>m_wpLootTableManager;


	Math::Vector3 m_playerPos;
	Math::Vector3 m_pos;

	float m_getRadius = 0.1;

	void GetItem();

	std::string m_path;

	//1秒あたりの進む距離
	float m_spped=5;
	float m_maxSpeed = 100;

	float m_accel=0;


	//プレイヤーを追いかける処理
	void PlayerChase();
};
