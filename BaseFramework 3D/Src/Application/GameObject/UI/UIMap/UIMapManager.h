#pragma once

#include"../UIBase.h"

class UIMap_Map;
class UIMap_Player;
class UIMap_EnemyManage;
class UIMap_TreasureChest;
//ミニマップの管理
class UIMapManager:public UIBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	UIMapManager() {};

	//デストラクタ
	~UIMapManager()override {};

	//---- 基本の処理 ----

	//初期化
	virtual void Init()override;

	//更新前の処理
	virtual void PreUpdate()override;

	//描画前の処理
	virtual void PreDraw()override;

	//2Dの描画
	virtual void DrawSprite()override;

	//---- 取得 ----

	//マップのUIを返す
	//戻り値 ... マップのUI
	std::shared_ptr<UIMap_Map> GetUIMap_Map() {return m_spUIMap_Map;}

	//プレイヤーのUIを返す
	//戻り値 ... プレイヤーのUI
	std::shared_ptr<UIMap_Player> GetUIMap_Player() {return m_spUIMap_Player;}

	//敵のUIを返す
	//戻り値 ... 敵のUI
	std::shared_ptr<UIMap_EnemyManage> GetUIMap_Enemy() {return m_spUIMap_Enemy;}

	//---- 設定 ----

	//マップの基準位置(ワールド)をセット(子のUIにも渡す)
	// _base3DPos ... 基準位置(ワールド)
	void SetBase3DPos(Math::Vector3 _base3DPos);

	//1タイルの大きさをセット(子のUIにも渡す)
	// _size ... 1タイルの大きさ
	void SetTileSize(float _size);

	//宝箱のアイコンを追加
	// _spTreasureChest ... 宝箱のアイコン
	// _3DPos ... 宝箱の座標(ワールド)
	void AddUIMap_TreasureChest(std::shared_ptr<UIMap_TreasureChest> _spTreasureChest, Math::Vector3 _3DPos);

	//ミニマップを全部見えるようにするか
	// _enable ... 全部見えるようにするか
	void SetMinimapRevealAll(bool _enable);

	//---- その他の処理 ----

	//宝箱のアイコンを全部消す
	void ResetTreasureChest() { m_UIMap_TreasureChest.clear(); }

private:

	//---- メンバ変数 ----

	//マップのUI
	std::shared_ptr<UIMap_Map>m_spUIMap_Map;

	//プレイヤーのUI
	std::shared_ptr<UIMap_Player>m_spUIMap_Player;

	//敵のUI
	std::shared_ptr<UIMap_EnemyManage>m_spUIMap_Enemy;

	//宝箱のアイコンの一覧
	std::list<std::shared_ptr<UIMap_TreasureChest>>	m_UIMap_TreasureChest;

	//ミニマップの基準位置(画面)
	Math::Vector2 m_basePos;

	//マップの基準位置(ワールド)
	Math::Vector3 m_base3DPos;

	//1タイルの大きさ
	float m_tileSize = 10.0f;
};
