#pragma once

class PlayerBase;

//敵から落ちたコインの動きの段階
enum class DropCoinState
{
	Pop,      // 倒れた位置から飛び散る
	Wait,     // 地面に落ちて少し待つ
	Attract,  // プレイヤーに吸い寄せられる
	Get,      // 取得（所持金に加算して消える）
};

class DropCoin :public KdGameObject
{
public:
	//コンストラクタ
	DropCoin() {}
	//デストラクタ
	~DropCoin()override {};

	//初期化
	void Init()override;
	//更新前の処理
	void PreUpdate()override;
	//更新
	void Update()override;
	//モデルの描画
	void DrawLit()override;
	//エフェクトの描画
	void DrawEffect()override;

	void DrawBright()override;

	//プレイヤーをセット
	// _spPlayer ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayer) { m_wpPlayer = _spPlayer; }

	//このコイン1枚が持つ金額をセット
	// _value ... 金額
	void SetValue(long _value) { m_value = _value; }

	//座標をセット
	// _pos ... 座標
	void SetPos(const Math::Vector3& _pos) override
	{
		m_mWorld.Translation(_pos);
		m_pos = _pos;
	}

private:
	//コインのモデル
	std::shared_ptr<KdModelData>m_spCoinModel = nullptr;
	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	//このコイン1枚が持つ金額
	long m_value = 0;

	//座標
	Math::Vector3 m_pos;
	//プレイヤーの座標
	Math::Vector3 m_playerPos;

	//今の状態
	DropCoinState m_dropCoinState = DropCoinState::Pop;

	//-------- 飛び散り(Pop) --------
	bool  m_popInit = false;              // 飛び散りの初期化をしたか
	Math::Vector3 m_vel;                 // 速度（1秒あたり）
	float m_popUpSpeed = 5.0f;            // 上に飛び出す速さ
	float m_popSideSpeed = 5.0f;          // 横に散る速さの最大
	float m_gravity = 15.0f;              // 重力
	float m_groundY = 0.0f;               // 落ちて止まる地面の高さ

	//-------- 待機(Wait) --------
	float m_waitTimer = 0.0f;             // 地面で待っている時間
	float m_waitTime = 0.5f;              // 何秒待ったら吸い寄せ開始か

	//-------- 吸い寄せ(Attract) --------
	float m_attractTime = 0.5f;           // 距離に関係なく何秒で届かせるか
	float m_attractTimer = 0.0f;          // 吸い寄せを始めてからの時間
	Math::Vector3 m_attractStartPos;      // 吸い寄せを始めた位置
	float m_targetHeight = 1.0f;          // プレイヤーの足元ではなく胸あたりを狙う
	float m_getRadius = 0.2f;             // 取得したとみなす距離

	//-------- 見た目 --------
	float m_rotY = 0.0f;                  // くるくる回す角度
	float m_rotSpeed = 360.0f;            // 1秒に何度回るか
	float m_scale = 0.3f;                 // コインの大きさ

	//飛び散る
	void PopUpdate();

	//地面で待つ
	void WaitUpdate();

	//プレイヤーに吸い寄せられる
	void AttractUpdate();

	//取得（プレイヤーの所持金に加算して消える）
	void GetCoin();
};
