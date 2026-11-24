#pragma once
class PlayerBase;
class GameScene;
class WarpGateManager;

//ワープゲートの基底クラス
class WarpGateBase :public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	WarpGateBase() {};

	//デストラクタ
	~WarpGateBase()override;

	//---- 基本の処理 ----

	//初期化
	virtual void Init()override;

	//更新
	virtual void Update()override;

	//描画前の処理
	void PreDraw() override;

	//ライトありの描画
	void DrawLit()override;

	//---- 設定 ----

	//座標をセット
	// _pos ... 座標
	void SetPos(const Math::Vector3& _pos)override
	{
		m_pos = _pos;
		m_mWorld.Translation(m_pos);
	}

	//プレイヤーをセット
	// _spPlayer ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayer) { m_wpPlayer = _spPlayer; }

	//ゲームシーンをセット
	// _spGameScene ... ゲームシーン
	void SetGameScene(std::shared_ptr<GameScene>_spGameScene) { m_wpGameScene = _spGameScene; }

	//ワープゲートの管理をセット
	// _spWarpGateManager ... ワープゲートの管理
	void SetWarpGateManager(std::shared_ptr<WarpGateManager>_spWarpGateManager) { m_wpWarpGateManager = _spWarpGateManager; }

	//消すかをセット
	// _flg ... 消すならtrue
	void SetIsExpired(bool _flg) { m_isExpired = _flg; };

protected:

	//---- メンバ変数 ----

	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	//ゲームシーン
	std::weak_ptr<GameScene>m_wpGameScene;

	//ワープゲートの管理
	std::weak_ptr<WarpGateManager>m_wpWarpGateManager;

	//3Dモデル
	std::shared_ptr<KdModelData>m_spWarpGate;

	//魔法陣アニメーション用（パーツごとに回転させるため KdModelWork で描画する）
	std::shared_ptr<KdModelWork>m_spWarpGateWork;

	//魔法陣のアニメーションの経過時間
	float m_animTime = 0.0f;

	//座標
	Math::Vector3 m_pos;

	//ワープゲートのエフェクト
	std::weak_ptr<KdEffekseerObject> m_wpWarpGateEffect;

	// 操作案内UI用テクスチャ
	std::shared_ptr<KdTexture> m_spGuideUITex;

	//魔法陣のうなり音(ゲートの位置から聞こえる。ループ)
	std::shared_ptr<KdSoundInstance3D> m_spHum3DSE;

	//---- その他の処理 ----

	//プレイヤーが乗ってるかどうか
	bool PlayerHit();

	//魔法陣のパーツを回転させる
	void UpdateMagicCircleAnime();

	//魔法陣のうなり音を鳴らし続ける
	void UpdateHumSE();

	//操作案内のUIを出す
	// _key ... 案内するキー番号
	void ShowGuideUI(int _key);
};
