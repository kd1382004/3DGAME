#pragma once

#include"../MapObjBase.h"

class PlayerBase;
class CameraBase;
class UIManager;
class UIMap_TreasureChest;
class LootTableManager;
class UIBase2;

//宝箱
class TreasureChest:public MapObjBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	TreasureChest(){};

	//デストラクタ
	~TreasureChest() override;

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新
	void Update()override;

	//更新後の処理
	void PostUpdate()override;

	//影の描画
	void GenerateDepthMapFromLight()override;

	//描画前の処理(視錐台カリングなど)
	void PreDraw()override;

	//ライトありの描画
	void DrawLit()override;

	//---- 設定 ----

	//プレイヤーをセット
	// _spPlayer ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayer) { m_wpPlayer = _spPlayer; };

	//カメラをセット
	// _spCamera ... カメラ
	void SetCamera(std::shared_ptr<CameraBase> _spCamera) { m_wpCamera = _spCamera; };

	//UIの管理をセット
	// _spUIManager ... UIの管理
	void SetUIManager(std::shared_ptr<UIManager> _spUIManager);

	//ドロップテーブルの管理をセット
	// _spLootTableManager ... ドロップテーブルの管理
	void SetLootTableManager(std::shared_ptr<LootTableManager> _spLootTableManager) { m_wpLootTableManager = _spLootTableManager; };

	//宝箱の中身の番号をセット
	// _num ... 中身の番号(テーブルの番号)
	void SetTreasureChestTableNum(int _num) { m_treasureChestTableNum = _num; }

	//UIBase2に入れるテクスチャ
	// _spTex ... テクスチャ
	void SetUITex(std::shared_ptr<KdTexture>_spTex) { m_spUiTex = _spTex; }

private:

	//---- メンバ変数 ----

	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	//カメラ
	std::weak_ptr<CameraBase>m_wpCamera;

	//UIの管理
	std::weak_ptr<UIManager>m_wpUIManager;

	//ミニマップの宝箱のアイコン
	std::weak_ptr<UIMap_TreasureChest>m_wpUIMap_TreasureChest;

	//ドロップテーブルの管理
	std::weak_ptr<LootTableManager>m_wpLootTableManager;

	//開けるときのUI
	std::weak_ptr<UIBase2>m_wpUIBase2;

	//宝箱のモデル
	std::shared_ptr<KdModelWork>m_spTreasureChestModel = nullptr;

	//宝箱のアニメーション
	std::shared_ptr<KdAnimator>	m_spTreasureChestAnimator = nullptr;

	//空いてるかどうか
	bool m_isOpen = false;

	//空き切ったかどうか
	bool m_open = false;

	//視錐台用のBoxInfo
	KdCollider::BoxInfo m_frustumBox;

	//カメラに映っているか
	bool m_isInView = false;

	//宝箱の中身
	int m_treasureChestTableNum = 2;

	//ドロップテーブルの名前の頭
	std::string m_treasureChestGetItemLootPath = "slot";

	//UIBase2に入れるテクスチャ
	std::shared_ptr<KdTexture>m_spUiTex;

	//オープン可能範囲
	float m_openSize = 3;

	//音
	std::shared_ptr<KdSoundInstance3D> m_spTreasureChestSE;

	//開くアニメーションがどれだけ進んだらアイテムを出すか(0〜1)
	const float m_dropItemInitPercent = 0.6;

	//アイテムを出したか
	bool m_dropItemInitFlg = false;
};
