#pragma once

class PlayerBase;
class PotionUseController;
class PotionTexInfo;


// アイテム画面の種類（切り替えボタン）
// 換金品などを増やすときは Num の前に足す
enum ItemCategory
{
	ItemCategory_Consumable,	// 消耗品（ポーション）
	ItemCategory_Material,		// 素材（武器強化アイテム）

	ItemCategory_Num			// 種類の数（配列のサイズ用）
};

//アイテムを使ったときのアイコンの演出の種類
enum UseEffectType
{
	None,		// 演出なし
	Bounce,		// 上に跳ねる（成功）
	Shake,		// 横に震える（失敗）
};

//////////////////////////////////////////////////////////////
// インベントリの各タブ（アイテム / 武器強化 / ステータス）で共有する情報
// PlayerInventoryUI が持ち、各タブに渡す
//////////////////////////////////////////////////////////////
struct InventoryUIContext
{
	//プレイヤー
	std::weak_ptr<PlayerBase>			m_wpPlayer;
	//ポーションを使う指示を出すクラス
	std::weak_ptr<PotionUseController>	m_wpPotionUseController;
	//ポーションの画像一覧
	std::weak_ptr<PotionTexInfo>		m_wpPotionTexInfo;

	// 右側の枠に出す名前・説明（各タブが書き換えて、PlayerInventoryUI が描く）
	std::shared_ptr<KdTexture> m_spSelectNameTex;
	//右側の枠に出す説明
	std::shared_ptr<KdTexture> m_spSelectExplanationTex;
	//何も選んでいないときの名前
	std::shared_ptr<KdTexture> m_spNotSelectNameTex;
	//何も選んでいないときの説明
	std::shared_ptr<KdTexture> m_spNotSelectExplanationTex;

	// 右側の枠に出す数字（所持数 / LV）
	int m_num = 0;

	//---------- アイテム使用・強化時の演出 ----------
	float m_useEffectTimer = 0.0f;			// 使ってからの経過時間（0なら演出なし）
	float m_useEffectTime = 0.25f;			// 演出の長さ（秒）
	int   m_useEffectItemID = -999;			// 演出中のアイテムID

	//演出の種類
	UseEffectType m_useEffectType = UseEffectType::None;




	// 0→1→0 と山なりに変化する値（演出の強さ）
	float UseEffectT() const
	{
		if (m_useEffectTimer <= 0.0f) { return 0.0f; }
		return sinf(DirectX::XM_PI * (1.0f - m_useEffectTimer / m_useEffectTime));
	}

	//---------- 共通の位置・画像 ----------
	Math::Vector2 m_back1Tex2DPos;			// 左の大きい枠の位置
	float         m_back1TexWidth = 0.0f;		// 左の大きい枠の幅
	Math::Vector2 m_back2Tex2DPos;			// 右の枠の位置

	std::shared_ptr<KdTexture> m_spUseTex;		// 使用ボタン
	Math::Vector2              m_useTex2DPos;

	std::shared_ptr<KdTexture> m_spPotionSelectTex;	// 選択中の枠
};

// アイテムのアイコン情報（アイテム画面と武器強化画面で使う）
struct InventoryItemIconInfo
{
	//アイテムの番号
	int m_itemID = 0;
	//表示位置
	Math::Vector2 m_2DPos;
	//所持数
	int m_num = 0;
	//マウスが乗っているか
	bool m_hit = false;

	//アイテムの名前
	std::string m_name;
	//アイコンの画像
	std::shared_ptr<KdTexture>m_spIconTex;
	//説明の画像
	std::shared_ptr<KdTexture>m_spExplanationTex;
	//名前の画像
	std::shared_ptr<KdTexture> m_spNameTex;
};

//////////////////////////////////////////////////////////////
// インベントリのタブの基底
//////////////////////////////////////////////////////////////
class InventoryTabBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//デストラクタ
	virtual ~InventoryTabBase() {}

	//---- 基本の処理 ----

	// 起動時に1回
	virtual void Init() {}

	// タブが開いている間、毎フレーム
	virtual void Update() {}

	// タブが選ばれている間、毎フレーム（インベントリの開閉に関係なく）
	virtual void PreDraw() {}

	// タブが開いている間の2D描画
	virtual void DrawSprite() {}

	//---- 設定 ----

	//各タブで共有する情報をセット
	// _spContext ... 共有する情報
	void SetContext(std::shared_ptr<InventoryUIContext> _spContext) { m_spContext = _spContext; }

	//---- その他の処理 ----

	// タブが選ばれたとき
	virtual void OnSelect() {}

protected:

	//---- メンバ変数 ----

	//各タブで共有する情報
	std::shared_ptr<InventoryUIContext> m_spContext = nullptr;
};
