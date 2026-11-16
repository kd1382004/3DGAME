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
	std::weak_ptr<PlayerBase>			wpPlayer;
	std::weak_ptr<PotionUseController>	wpPotionUseController;
	std::weak_ptr<PotionTexInfo>		wpPotionTexInfo;

	// 右側の枠に出す名前・説明（各タブが書き換えて、PlayerInventoryUI が描く）
	std::shared_ptr<KdTexture> selectNameTex;
	std::shared_ptr<KdTexture> selectExplanationTex;
	std::shared_ptr<KdTexture> notSelectNameTex;
	std::shared_ptr<KdTexture> notSelectExplanationTex;

	// 右側の枠に出す数字（所持数 / LV）
	int num = 0;

	//---------- アイテム使用・強化時の演出 ----------
	float useEffectTimer = 0.0f;			// 使ってからの経過時間（0なら演出なし）
	float useEffectTime = 0.25f;			// 演出の長さ（秒）
	int   useEffectItemID = -999;			// 演出中のアイテムID

	UseEffectType useEffectType = UseEffectType::None;




	// 0→1→0 と山なりに変化する値（演出の強さ）
	float UseEffectT() const
	{
		if (useEffectTimer <= 0.0f) { return 0.0f; }
		return sinf(DirectX::XM_PI * (1.0f - useEffectTimer / useEffectTime));
	}

	//---------- 共通の位置・画像 ----------
	Math::Vector2 back1Tex2DPos;			// 左の大きい枠の位置
	float         back1TexWidth = 0.0f;		// 左の大きい枠の幅
	Math::Vector2 back2Tex2DPos;			// 右の枠の位置

	std::shared_ptr<KdTexture> useTex;		// 使用ボタン
	Math::Vector2              useTex2DPos;

	std::shared_ptr<KdTexture> potionSelectTex;	// 選択中の枠
};

// アイテムのアイコン情報（アイテム画面と武器強化画面で使う）
struct InventoryItemIconInfo
{
	int m_ItemID = 0;
	Math::Vector2 m_2DPos;
	int m_num = 0;
	bool m_hit = false;

	std::string m_name;
	std::shared_ptr<KdTexture>m_IconTex;
	std::shared_ptr<KdTexture>m_ExplanationTex;
	std::shared_ptr<KdTexture> m_NameTex;
};

//////////////////////////////////////////////////////////////
// インベントリのタブの基底
//////////////////////////////////////////////////////////////
class InventoryTabBase
{
public:
	virtual ~InventoryTabBase() {}

	void SetContext(std::shared_ptr<InventoryUIContext> _spContext) { m_spContext = _spContext; }

	// 起動時に1回
	virtual void Init() {}

	// タブが選ばれたとき
	virtual void OnSelect() {}

	// タブが開いている間、毎フレーム
	virtual void Update() {}

	// タブが選ばれている間、毎フレーム（インベントリの開閉に関係なく）
	virtual void PreDraw() {}

	// タブが開いている間の2D描画
	virtual void DrawSprite() {}

protected:
	std::shared_ptr<InventoryUIContext> m_spContext = nullptr;
};
