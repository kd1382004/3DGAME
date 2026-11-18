#pragma once

class UIBase;
class PlayerBase;
class UIMapManager;
class PlayerInventoryUI;
class GameScene;
class PotionUseController;
class PotionTexInfo;
class UIBase2;

class UIManager :public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	UIManager() {};

	//デストラクタ
	~UIManager()override {};

	//---- 基本の処理 ----

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

	//---- 取得 ----

	//マップUIの管理を返す
	//戻り値 ... マップUIの管理(無ければnullptr)
	std::shared_ptr<UIMapManager> GetUIMapManager() { return  m_wpUIMapManager.lock(); }

	//ポーションの画像一覧を返す
	//戻り値 ... ポーションの画像一覧
	std::shared_ptr<PotionTexInfo> GetPotionTexInfo() { return  m_spPotionTexInfo; }

	//キー表示のUIを返す
	//戻り値 ... キー表示のUI
	std::shared_ptr<UIBase2> GetUIBase2() { return  m_spUIBase2; }

	//宝箱用のテクスチャを返す
	//戻り値 ... 宝箱用のテクスチャ
	std::shared_ptr<KdTexture> GetTreasureChestUiTex() { return m_spTreasureChestUiTex; }

	//---- 設定 ----

	//プレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayerBase);

	//ゲームシーンをセット
	// _spGameScene ... ゲームシーン
	void SetGameScene(std::shared_ptr<GameScene> _spGameScene);

	//UIを追加
	// _spUI ... 追加するUI
	void AddUIObj(std::shared_ptr<UIBase> _spUI) { m_spUIList.push_back(_spUI); }

	//ポーションを使う指示を出すクラスをセット
	// _spPotionUseController ... ポーションを使う指示を出すクラス
	void SetPotionUseController(std::shared_ptr<PotionUseController> _spPotionUseController);

	//マップUIマネージャー
	void AddUIMapManager();

	//プレイヤーのインベントリ
	void AddPlayerInventoryUI();

	//ポーションの画像一覧
	void SetPotionTexInfo();

	//ミニマップを全部見えるようにするか
	// _enable ... 全部見えるようにするか
	void SetMinimapRevealAll(bool _enable);

private:

	//---- メンバ変数 ----

	//プレイヤー
	std::weak_ptr<PlayerBase> m_wpPlayerBase;

	//マップUIの管理
	std::weak_ptr<UIMapManager> m_wpUIMapManager;

	//ゲームシーン
	std::weak_ptr<GameScene> m_wpGameScene;

	//プレイヤーのインベントリUI
	std::shared_ptr<PlayerInventoryUI> m_spPlayerInventoryUI;

	//ポーションの画像一覧
	std::shared_ptr<PotionTexInfo> m_spPotionTexInfo;

	//キー表示のUI
	std::shared_ptr<UIBase2> m_spUIBase2;

	//UIの一覧
	std::list<std::shared_ptr<UIBase>> m_spUIList;

	//前のフレームのUIの数(増えたら並べ替える)
	int m_uiListOldSiz = 0;

	//宝箱用テクスチャ
	std::shared_ptr<KdTexture>m_spTreasureChestUiTex;
};
