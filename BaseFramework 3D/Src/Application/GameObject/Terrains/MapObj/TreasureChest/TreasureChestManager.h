#pragma once

#pragma once


class TreasureChest;
class PlayerBase;
class CameraBase;
class UIManager;
class UIMap_TreasureChest;
class LootTableManager;

//宝箱の管理
class TreasureChestManager :public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	TreasureChestManager() {};

	//デストラクタ
	~TreasureChestManager() override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新前の処理
	void PreUpdate()override;

	//更新
	void Update()override;

	//更新後の処理
	void PostUpdate()override;

	//影の描画
	void GenerateDepthMapFromLight()override;

	//ライトありの描画
	void DrawLit()override;

	//描画前の処理(視錐台カリングなど)
	void PreDraw()override;

	//---- 設定 ----

	//プレイヤーをセット
	// _spPlayer ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayer) { m_wpPlayer = _spPlayer; };

	//カメラをセット
	// _spCamera ... カメラ
	void SetCamera(std::shared_ptr<CameraBase> _spCamera) { m_wpCamera = _spCamera; };

	//UIの管理をセット
	// _spUIManager ... UIの管理
	void SetUIManager(std::shared_ptr<UIManager> _spUIManager) { m_wpUIManager = _spUIManager; };

	//---- その他の処理 ----

	//宝箱を生成する
	// _treasureChestPosList ... 宝箱を置く座標の一覧
	void GenerateTreasureChest(std::list<Math::Vector3> _treasureChestPosList);

	//宝箱とあたり判定したい対象
	// _spObj ... 当たり判定させる対象
	void TreasureChestHit(const std::shared_ptr<KdGameObject>& _spObj);

	//宝箱を全部消す
	void TreasureChestReset() { m_treasureChestList.clear(); }

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
	std::shared_ptr<LootTableManager>m_spLootTableManager;

	//宝箱の一覧
	std::list<std::shared_ptr<TreasureChest>> m_treasureChestList;
};
