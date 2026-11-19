#pragma once
#include"../ResultDisplayBase.h"

class WeaponBase;
struct Inventory;

class ResultDisplay_WeponInventory :public ResultDisplayBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	ResultDisplay_WeponInventory() {}

	//デストラクタ
	~ResultDisplay_WeponInventory() {}

	//---- 基本の処理 ----

	//初期化
	void Init();

	//更新
	void Update();

	//描画前の処理
	void PreDraw();

	//2Dの描画
	void DrawSprite();

	//---- 設定 ----

	//ゲーム終了時の武器をセット
	// _spWeapon ... 武器
	void SetSpWeaponBase(std::shared_ptr<WeaponBase>_spWeapon) { m_spWeapon = _spWeapon; }

	//---- その他の処理 ----

	//武器
	// _weaponsStrengtheningInventory ... 持っている武器強化素材の一覧
	void WeaponsStrengtheningInfoInit(std::vector<Inventory> _weaponsStrengtheningInventory);

private:

	//---- 型 ----

	//リザルトに並べる強化アイテムのアイコン1つ分の情報
	struct ItemIconInfo
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

	//リザルトに出す武器強化の項目1つ分の表示情報
	struct WeaponsStrengtheningInfo
	{
		//現LV
		int m_lv = 0;

		//強化の番号
		int m_ID = 0;

		//次のLVにするために必要な強化アイテムの数
		int m_nextLvNum = 0;

		//強化が可能かどうか
		bool m_strengtheningFlg = false;

		//LVがMAXかどうか
		bool m_lvMaxFlg = false;

		//アイコン
		std::shared_ptr<KdTexture>m_spIconTex;

		//アイコンの位置
		Math::Vector2 m_iconPos;

		//アイコンのサイズ
		Math::Vector2 m_iconSiz;

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
		Math::Vector2 m_iconStrengtheningSiz;

		//演出用のタイマー
		float m_timer = 0.0f;

		//名前の画像
		std::shared_ptr<KdTexture> m_spNameTex;

		//前提ノードのMyNodeID（-1 = 前提なし）
		int m_preconditionNodeID = -1;

		//前提ノードの配列番号（-1 = 前提なし）
		int m_preconditionNodeIndex = -1;

		//選ばれているか
		bool m_chooseFlg = false;
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

		WeaponsStrengtheningSiz
	};

	//---- メンバ変数 ----

	//ゲーム終了時の武器
	std::shared_ptr<WeaponBase>m_spWeapon;

	//「前提ノードを解放してください」の説明の画像
	std::shared_ptr<KdTexture>m_spPreconditionNodeExplanationTex;

	//前提ノードの名前の画像
	std::shared_ptr<KdTexture>m_spPreconditionNodeNameTex;

	//前提ノードの枠の画像
	std::shared_ptr<KdTexture>m_spPreconditionNodeTex;

	//前提ノードのアイコンの画像
	std::shared_ptr<KdTexture>m_spPreconditionNodeIconTex;

	//選んでいるものの説明の画像
	std::shared_ptr<KdTexture>m_spSelectExplanationTex;

	//選んでいるものの名前の画像
	std::shared_ptr<KdTexture>m_spSelectNameTex;

	//強化ボタンの位置
	Math::Vector2 m_weaponsStrengtheningButtonTex2DPos;

	//選んでいる枠の画像
	std::shared_ptr<KdTexture>m_spSelectTex;

	//「LV」の画像
	std::shared_ptr<KdTexture>m_spLVTex;

	//「LV」の位置
	Math::Vector2 m_LVTex2DPos;

	//選んでいる枠の位置
	Math::Vector2 m_selectTex2DPos;

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

	//選んでいるノードの前提ノードがまだ解放されていないか
	bool m_preconditionNodeFlg = false;

	//選んでいる強化の番号
	int m_selectWeaponsStrengtheningID = 0;

	//選んでいる強化アイコンの大きさ
	Math::Vector2 m_selectWeaponsStrengtheningIconSiz;

	//強化の情報(種類ごと)
	WeaponsStrengtheningInfo m_weaponsStrengtheningInfo[WeaponsStrengtheningSiz];

	//強化アイテムのアイコン一覧
	std::vector<ItemIconInfo> m_weaponsStrengtheningIconInfoList;

	//次のLVにするために必要な強化アイテムの数
	int m_selectWeaponsStrengtheningNextLVNum = 0;

	//背景の画像
	std::shared_ptr<KdTexture>m_spWeaponsStrengtheningBackTex;

	//背景2の画像
	std::shared_ptr<KdTexture>m_spWeaponsStrengtheningBack2Tex;

	//背景の位置
	Math::Vector2 m_weaponsStrengtheningTexPos;

	//タイトルの画像
	std::shared_ptr<KdTexture>m_spWeaponsStrengtheningTitleTex;

	//タイトルの位置
	Math::Vector2 m_weaponsStrengtheningTitleTexPos;

	//ノードをつなぐ線の画像
	std::shared_ptr<KdTexture>m_spLineTex;

	//---- その他の処理 ----

	//アイコンとの当り判定
	void WeaponsStrengtheningIconHit();

	//指定した強化だけ選んで、それ以外は選ばれていないようにする
	// _ID ... 選ぶ強化の番号
	void WeaponChooseFlg(int _ID);

	//選択中の強化に使うアイテムが m_weaponsStrengtheningIconInfoList の何番目か（なければ -1）
	int FindSelectItemIconIndex() const;
};
