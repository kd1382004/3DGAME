#pragma once

class PlayerBase;
class LootTableManager;

//宝箱から出たアイテムの動きの段階
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
	//コンストラクタ
	TreasureDropItem() {}
	 //デストラクタ
	 ~TreasureDropItem()override {};

	 //初期化
	 void Init()override;
	 //更新前の処理
	 void PreUpdate()override;
	 //更新
	 void Update()override;
	 //エフェクトの描画
	 void DrawEffect()override;
	
	 //ドロップテーブルの管理をセット
	 // _spLootTableManager ... ドロップテーブルの管理
	 void SetLootTableManager(std::shared_ptr<LootTableManager> _spLootTableManager) { m_wpLootTableManager = _spLootTableManager; };
	 //プレイヤーをセット
	 // _spPlayer ... プレイヤー
	 void SetPlayer(std::shared_ptr<PlayerBase> _spPlayer) { m_wpPlayer = _spPlayer; };

	//座標をセット
	// _pos ... 座標
	void SetPos(const Math::Vector3& _pos) override
	{
		m_mWorld.Translation(_pos); 
		m_pos = _pos;
	}

	//出すアイテムのモデルのパスをセット
	// _path ... モデルのパス
	void SetPath(std::string _path) { m_path = _path; }
private:
	//アイテムのモデル
	std::shared_ptr<KdModelData>m_spDropItemModel = nullptr;
	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;
	//ドロップテーブルの管理
	std::weak_ptr<LootTableManager>m_wpLootTableManager;


	//プレイヤーの座標
	Math::Vector3 m_playerPos;
	//座標
	Math::Vector3 m_pos;

	//拾える距離
	float m_getRadius = 0.1;



	//モデルのパス
	std::string m_path;

	//1秒あたりの進む距離
	float m_speed=10;
	//最高速度
	float m_maxSpeed = 1000;

	//今の加速
	float m_accel=0;
	//1秒あたりに増やす加速
	float m_addAccel=100;



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

	//今の状態
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
