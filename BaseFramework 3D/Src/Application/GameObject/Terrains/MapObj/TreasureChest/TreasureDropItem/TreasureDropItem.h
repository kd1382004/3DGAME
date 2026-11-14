#pragma once

class PlayerBase;
class LootTableManager;

enum class DropState
{
	Pop,      // 宝箱から上に飛び出す
	Float,    // 少し浮いて止まる
	Attract,  // プレイヤーに吸い寄せられる
	Get,      // 取得（UI に通知して消える）
};

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



	std::string m_path;

	//1秒あたりの進む距離
	float m_spped=10;
	float m_maxSpeed = 1000;

	float m_accel=0;
	float m_AddAccel=100;



	//-------- 飛び出し(Pop) --------
	bool  m_popInit = false;              // 飛び出しの初期化をしたか
	Math::Vector3 m_vel;                  // 速度（1秒あたり）
	float m_popUpSpeed = 6.0f;            // 上に飛び出す速さ
	float m_popSideSpeed = 1.5f;          // 横に散る速さの最大
	float m_gravity = 18.0f;              // 重力
	float m_floatHeight = 1.6f;           // 宝箱の位置からどの高さで止まるか
	float m_startY = 0.0f;                // 出てきた高さ

	//-------- 浮遊(Float) --------
	float m_floatTimer = 0.0f;            // 浮いている時間
	float m_floatTime = 0.4f;             // 何秒浮いたら吸い寄せ開始か
	float m_floatBaseY = 0.0f;            // 浮遊の中心の高さ
	float m_bobHeight = 0.08f;            // 上下にふわふわする幅
	float m_bobSpeed = 6.0f;              // ふわふわの速さ

	//-------- 見た目 --------
	float m_rotY = 0.0f;                  // くるくる回す角度
	float m_rotSpeed = 180.0f;            // 1秒に何度回るか
	float m_time = 0.0f;                  // 光の明滅用

	//-------- 吸い寄せ --------
	float m_targetHeight = 1.0f;          // プレイヤーの足元ではなく胸あたりを狙う

	DropState m_dropStateMode;


	//宝箱から飛び出す
	void PopUpdate();
	
	//浮いて止まる
	void FloatUpdate();
	
	//プレイヤーを追いかける処理
	void AttractUpdate();

	//取得
	void GetItem();
};
