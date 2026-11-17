#pragma once
#include"../UIBase.h"
#include"InventoryUIContext.h"

class GameScene;
class PotionUseController;
class PlayerBase;
class PotionTexInfo;

class PotionInventoryUI;
class WeponInventoryUI;
class PlayerStatusInventoryUI;

class PlayerInventoryUI :public UIBase
{
public:

	//コンストラクタ
	PlayerInventoryUI() {};
	//デストラクタ
	~PlayerInventoryUI()override {};

	//初期化
	void Init()override;

	//更新前の処理
	void PreUpdate()override;
	//更新
	void Update()override;

	//描画前の処理
	void PreDraw()override;
	//2Dの描画
	void DrawSprite()override;
	//ゲームシーンをセット
	// _spGameScene ... ゲームシーン
	void SetGameScene(std::shared_ptr<GameScene> _spGameScene) { m_wpGameScene = _spGameScene; }


	//ポーションを使う指示を出すクラスをセット
	// _spPotionUseController ... ポーションを使う指示を出すクラス
	void SetPotionUseController(std::shared_ptr<PotionUseController> _spPotionUseController) { m_spContext->m_wpPotionUseController = _spPotionUseController; }
	//プレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayerBase(std::shared_ptr<PlayerBase> _spPlayerBase) { m_spContext->m_wpPlayer = _spPlayerBase; }
	//ポーションの画像一覧をセット
	// _spPotionTexInfo ... ポーションの画像一覧
	void SetPotionTexInfo(std::shared_ptr<PotionTexInfo> _spPotionTexInfo) { m_spContext->m_wpPotionTexInfo = _spPotionTexInfo; }
private:

	// 各タブで共有する情報（選択中の名前・説明、数字、演出、プレイヤーなど）
	std::shared_ptr<InventoryUIContext> m_spContext = std::make_shared<InventoryUIContext>();

	//選んだものの名前の位置
	Math::Vector2 m_selectNameTexPos;
	//選んだものの説明の位置
	Math::Vector2 m_selectExplanationTexPos;

	//ゲームシーン
	std::weak_ptr<GameScene> m_wpGameScene;

	//インベントリが開いてるかどうか
	bool m_playerInventoryUIFlg = false;

	//インベントリの開閉を管理
	void PlayerInventoryOpen();


	//左の大きい枠の画像
	std::shared_ptr<KdTexture>m_spBack1Tex;
	//左の大きい枠の位置
	Math::Vector2 m_back1Tex2DPos;


	//右の枠の画像(今出しているもの)
	std::shared_ptr<KdTexture>m_spBack2Tex;
	//右の枠の画像(選んでいるとき)
	std::shared_ptr<KdTexture>m_spChangeBack2Tex;
	//右の枠の画像(何も選んでいないとき)
	std::shared_ptr<KdTexture>m_spNotSelect;

	//右の枠の位置
	Math::Vector2 m_back2Tex2DPos;
	//右の枠の大きさ
	Math::Vector2 m_back2Tex2DSiz;

	//タブの帯の画像
	std::shared_ptr<KdTexture>m_spInventoryTypeTex;
	//タブの帯の位置
	Math::Vector2 m_inventoryType2DPos;
	//タブの帯の大きさ
	Math::Vector2 m_inventoryTypeSiz;

	/////////////////////////////
	//どのインベントリを開いてるか

	enum InventoryType
	{
		PotionInventory,
		WeaponInventory,
		PlayerStatus,
		InventoryTypeSiz,
	};

	//今開いているタブ
	InventoryType m_nowInventoryType = PotionInventory;


	//どのInventoryにするか
	void InventoryTypeChangeUpdate();

	struct InventoryTypeChange
	{
		//タブの種類
		InventoryType m_ID;
		//ボタンの位置
		Math::Vector2 m_2DPos;
		//マウスが乗っているか
		bool m_hit = false;
		//アイコンの画像
		std::shared_ptr<KdTexture>m_spIconTex;
		//アイコンの大きさ
		Math::Vector2 m_iconTexSiz;
	};

	//タブの切り替えボタン
	InventoryTypeChange m_inventoryTypeChange[InventoryTypeSiz];

	/////////////////////////////
	//各タブ
	std::shared_ptr<PotionInventoryUI>			m_spPotionInventoryUI;
	//武器強化のタブ
	std::shared_ptr<WeponInventoryUI>			m_spWeaponInventoryUI;
	//ステータスのタブ
	std::shared_ptr<PlayerStatusInventoryUI>	m_spPlayerStatusInventoryUI;

	// InventoryType に対応するタブ
	// _type ... タブの種類
	InventoryTabBase* GetTab(InventoryType _type);

	/////////////////////////////
	//所持金(上の木の帯の右端に出す。どのタブでも見える)
	//値はプレイヤーのインベントリ(PlayerInventory)が持っている。ここでは描くだけ
	std::shared_ptr<KdTexture> m_spCoinTex;	// 金貨のアイコン
	std::shared_ptr<KdTexture> m_spGoldTex;	// 「G」

	// 所持金を描く
	void DrawMoney();
};
