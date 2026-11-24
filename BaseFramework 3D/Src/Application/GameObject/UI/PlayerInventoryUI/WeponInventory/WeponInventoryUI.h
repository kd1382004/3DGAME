#pragma once
#include"../InventoryUIContext.h"

//////////////////////////////////////////////////////////////
// インベントリ：武器強化（スキルツリー）画面
//////////////////////////////////////////////////////////////
class WeponInventoryUI :public InventoryTabBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	WeponInventoryUI() {}

	//デストラクタ
	~WeponInventoryUI()override {}

	//---- 基本の処理 ----

	//初期化
	void Init()override;
	void Update()override;			// = 旧 WeaponsStrengtheningUpdate
	void DrawSprite()override;		// = 旧 WeaponsStrengtheningDraw

	//---- その他の処理 ----

	void OnSelect()override;		// = 旧 WeaponsStrengtheningInfoInit

private:

	//---- 型 ----

	//武器強化の項目1つ分の表示情報
	struct WeaponsStrengtheningInfo
	{
		//現LV
		int m_lv = 0;

		//強化に使うアイテムの番号
		int m_strengtheningItemID = 0;

		//次のLVにするために必要な強化アイテムの数
		int m_nextLvNum = 0;

		//最大レベルか
		bool m_lvMaxFlg = false;

		//強化が可能かどうか
		bool m_strengtheningFlg = false;

		//アイコン
		std::shared_ptr<KdTexture>m_spIconTex;

		//アイコンの位置
		Math::Vector2 m_iconPos;

		//アイコンのサイズ
		Math::Vector2 m_iconSize;

		//当たってるか
		bool m_hit = false;

		//説明画像
		std::shared_ptr<KdTexture>m_spExplanationTex;


		//強化アイテムの数
		int m_num = 0;

		//強化アイテムの画像
		std::shared_ptr<KdTexture>m_spIconStrengtheningTex;

		//強化アイテムの位置
		Math::Vector2 m_iconStrengtheningPos;

		//アイコンのサイズ
		Math::Vector2 m_iconStrengtheningSize;

		//演出用のタイマー
		float m_timer = 0.0f;

		//名前の画像
		std::shared_ptr<KdTexture> m_spNameTex;


		//選ばれてるかどうか
		bool m_chooseFlg = false;

		//自分のノードID
		int m_myNodeID = 0;

		//前提ノードのMyNodeID（-1 = 前提なし）
		int m_preconditionNodeID = -1;

		//前提ノードの配列番号（-1 = 前提なし）
		int m_preconditionNodeIndex = -1;

		//線の光の強さ（0=消えてる 1=全開）毎フレーム目標に近づける
		float m_lineGlow = 0.0f;


		//オーラのアルファ
		float m_auraAlpha = 0.0f;
	};

	//武器強化の項目の番号
	enum WeaponStrengtheningID
	{
		//攻撃力
		WeaponsStrengthening_Attack,

		//スタミナ
		WeaponsStrengthening_Stamina,

		//Chargeタイム
		WeaponsStrengthening_ChargeTime,

		//防御無視
		WeaponsStrengthening_DefenseIgnore,

		//武器特有(Lv1になったら他の特有の強化を許す)
		WeaponsStrengthening_Characteristic,

		//武器特有1
		WeaponsStrengthening_Characteristic1,

		//武器特有2
		WeaponsStrengthening_Characteristic2,

		//武器特有3
		WeaponsStrengthening_Characteristic3,

		//武器アイコン
		WeaponsStrengthening_Icon,

		WeaponsStrengtheningSize
	};

	//---- メンバ変数 ----

	//「/」の画像
	std::shared_ptr<KdTexture>m_spNumSlashTex;

	//ノードをつなぐ線の画像
	std::shared_ptr<KdTexture>m_spLineTex;

	//「前提ノードを解放してください」の説明の画像
	std::shared_ptr<KdTexture>m_spPreconditionNodeExplanationTex;

	//前提ノードの名前の画像
	std::shared_ptr<KdTexture>m_spPreconditionNodeNameTex;

	//前提ノードの枠の画像
	std::shared_ptr<KdTexture>m_spPreconditionNodeTex;

	//前提ノードのアイコンの画像
	std::shared_ptr<KdTexture>m_spPreconditionNodeIconTex;

	//強化ボタンの画像
	std::shared_ptr<KdTexture>m_spWeaponsStrengtheningButtonTex;

	//「LV」の画像
	std::shared_ptr<KdTexture>m_spLVTex;

	//光っている線の画像
	std::shared_ptr<KdTexture>m_spLineGlowTex;

	//強化ボタンの位置
	Math::Vector2 m_weaponsStrengtheningButtonTex2DPos;

	//「LV」の位置
	Math::Vector2 m_LVTex2DPos;

	//選んでいるアイコンの1フレームあたりの拡大量
	float m_selectAdd = 0.1;

	//拡大量の初期値
	float m_selectDefaultAdd = 0.1;

	//拡大量の増減
	float m_selectAddNum = 0.05;

	//今の拡大の合計
	float m_selectAddTotal = 0;

	//拡大の合計の最大
	float m_selectAddTotalMax = 50;

	//最大レベルのときのボタンの画像
	std::shared_ptr<KdTexture>m_spLvMaxButtonTex;

	// 選んでいるノードの前提ノードが、まだ解放されていないか
	// true  … 強化できない（強化ボタンの代わりに「前提ノードを解放してください」を表示）
	// false … 前提なし、または解放済み（通常の強化ボタンを表示）
	// ※ WeaponsStrengtheningButtonHit() で毎フレーム更新
	bool m_preconditionNodeFlg = false;

	//選択中の強化の配列番号（= 種類）
	int m_selectIndex = 0;

	//選んでいる強化アイコンの大きさ
	Math::Vector2 m_selectWeaponsStrengtheningIconSize;

	//強化の情報(種類ごと)
	WeaponsStrengtheningInfo m_weaponsStrengtheningInfo[WeaponsStrengtheningSize];

	//強化アイテムのアイコン情報
	InventoryItemIconInfo m_weaponsStrengtheningIconInfo;

	//次のLVにするために必要な強化アイテムの数
	int m_selectWeaponsStrengtheningNextLVNum = 0;

	// 線の光がフェードする速さ
	const float m_lineGlowFadeSpeed = 10.0f;

	//---- その他の処理 ----

	//強化アイコンの位置を決める
	void WeaponsStrengtheningInfoInitPos();

	//あるものを入れてく関数
	void WeaponsStrengtheningTexInfo();

	//アイコンとの当り判定
	void WeaponsStrengtheningIconHit();

	//武器強化ボタンとのあたり判定
	void WeaponsStrengtheningButtonHit();

	//IDのみ選ばれてそれ以外は選ばれてないに
	// _index ... 選ぶ強化の配列番号
	void WeaponChooseFlg(int _index);
};
