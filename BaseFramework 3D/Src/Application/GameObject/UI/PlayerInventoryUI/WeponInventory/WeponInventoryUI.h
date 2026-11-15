#pragma once
#include"../InventoryUIContext.h"

//////////////////////////////////////////////////////////////
// インベントリ：武器強化（スキルツリー）画面
//////////////////////////////////////////////////////////////
class WeponInventoryUI :public InventoryTabBase
{
public:
	WeponInventoryUI() {}
	~WeponInventoryUI()override {}

	void Init()override;
	void OnSelect()override;		// = 旧 WeaponsStrengtheningInfoInit
	void Update()override;			// = 旧 WeaponsStrengtheningUpdate
	void DrawSprite()override;		// = 旧 WeaponsStrengtheningDraw

private:
	void WeaponsStrengtheningInfoInitPos();

	//あるものを入れてく関数
	void WeaponsStrengtheningTexInfo();

	//アイコンとの当り判定
	void WeaponsStrengtheningIconHit();

	//武器強化ボタンとのあたり判定
	void WeaponsStrengtheningButtonHit();

	//IDのみ選ばれてそれ以外は選ばれてないに
	void WeaponChooseFlg(int _index);

	std::shared_ptr<KdTexture>m_numSlashTex;
	std::shared_ptr<KdTexture>m_LineTex;
	std::shared_ptr<KdTexture>m_PreconditionNodeExplanationTex;
	std::shared_ptr<KdTexture>m_PreconditionNodeNameTex;
	std::shared_ptr<KdTexture>m_PreconditionNodeTex;
	std::shared_ptr<KdTexture>m_PreconditionNodeIconTex;
	std::shared_ptr<KdTexture>m_weaponsStrengtheningButtonTex;
	std::shared_ptr<KdTexture>m_LVTex;
	std::shared_ptr<KdTexture>m_LineGlowTex;
	Math::Vector2 m_weaponsStrengtheningButtonTex2DPos;
	Math::Vector2 m_LVTex2DPos;
	float m_selectAdd = 0.1;
	float m_selectDefoAdd = 0.1;
	float m_selectAddNum = 0.05;
	float m_selectAddTotal = 0;
	float m_selectAddTotalMax = 50;

	std::shared_ptr<KdTexture>m_LvMaxButtonTex;


	// 選んでいるノードの前提ノードが、まだ解放されていないか
	// true  … 強化できない（強化ボタンの代わりに「前提ノードを解放してください」を表示）
	// false … 前提なし、または解放済み（通常の強化ボタンを表示）
	// ※ WeaponsStrengtheningButtonHit() で毎フレーム更新
	bool m_PreconditionNodeFlg = false;

	struct WeaponsStrengtheningInfo
	{
		//現LV
		int Lv;

		int strengtheningItem_ID = 0;

		//次のLVにするために必要な強化アイテムの数
		int nextLvNum;

		bool LvMaxFlg = false;

		//強化が可能かどうか
		bool strengtheningFlg = false;

		//アイコン
		std::shared_ptr<KdTexture>iconTex;

		//アイコンの位置
		Math::Vector2 iconPos;

		//アイコンのサイズ
		Math::Vector2 iconSiz;

		//当たってるか
		bool m_hit = false;

		//説明画像
		std::shared_ptr<KdTexture>m_ExplanationTex;


		//強化アイテムの数
		int num;

		//強化アイテムの画像
		std::shared_ptr<KdTexture>iconStrengtheningTex;

		//強化アイテムの位置
		Math::Vector2 iconStrengtheningPos;

		//アイコンのサイズ
		Math::Vector2 iconStrengtheningSiz;

		float timer = 0.0f;

		std::shared_ptr<KdTexture> m_NameTex;


		//選ばれてるかどうか
		bool ChooseFlg = false;

		//自分のノードID
		int MyNodeID = 0;

		//前提ノードのMyNodeID（-1 = 前提なし）
		int PreconditionNodeID = -1;

		//前提ノードの配列番号（-1 = 前提なし）
		int PreconditionNodeID_index = -1;

		//線の光の強さ（0=消えてる 1=全開）毎フレーム目標に近づける
		float lineGlow = 0.0f;


		float auraAlpha = 0.0f;
	};

	enum WeponStrengtheningID
	{
		//攻撃力
		WeaponsStrengthening_Attck,

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

	//選択中の強化の配列番号（= 種類）
	int m_selectIndex = 0;
	Math::Vector2 m_selectWeaponsStrengtheningIconSiz;

	WeaponsStrengtheningInfo m_weaponsStrengtheningInfo[WeaponsStrengtheningSiz];

	InventoryItemIconInfo m_weaponsStrengtheningIconInfo;

	//次のLVにするために必要な強化アイテムの数
	int m_selectWeaponsStrengtheningnextLVNum = 0;

	// 線の光がフェードする速さ
	const float m_lineGlowFadeSpeed = 10.0f;
};
