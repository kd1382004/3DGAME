#pragma once

#include"../Character/CharacterBase.h"
class MapManager;

struct Node; // ノード構造体の前方宣言を追加
//タイトル画面で歩き回るキャラ
class TitelMove :public CharacterBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	TitelMove();

	//デストラクタ
	~TitelMove() override {};

	//---- 基本の処理 ----

	//更新(経路に沿って動く)
	void Update() override;

	//更新後の処理
	void PostUpdate() override;

	//---- 取得 ----

	//目標地点を取得
	//戻り値 ... 目標地点
	const Math::Vector3& GetNextPos() const { return m_nextPos; }

	//移動速度を取得
	//戻り値 ... 移動速度
	float GetSpeed() const { return m_speed; }

	//動いているか
	//戻り値 ... 動いているならtrue
	bool IsMoving() const { return m_moveFlg; }

	//ノードに着いたとみなす距離を取得
	//戻り値 ... 距離
	float GetNodeNearDistance() const { return m_nodeNearDistance; }

	//進む向きを取得
	//戻り値 ... 進行方向ベクトル
	const Math::Vector3& GetMoveVec() const { return m_moveVec; }

	//---- 設定 ----

	//座標をセット
	// _pos ... 座標
	void SetPos(const Math::Vector3& _pos)
	{
		m_pos = _pos;
		m_mWorld.Translation(_pos);
	}

	// MapManagerの登録
	// _spMapManager ... マップの管理
	void SetMapManager(std::shared_ptr<MapManager> _spMapManager)
	{
		m_wpMapManager = _spMapManager;
	}

	// 目標地点の設定・取得
	// _nextPos ... 目標地点
	void SetNextPos(const Math::Vector3& _nextPos) { m_nextPos = _nextPos; }

	// 移動フラグ・移動速度のアクセサ
	// _speed ... 移動速度
	void SetSpeed(float _speed) { m_speed = _speed; }

	//動くかをセット
	// _flg ... 動くならtrue
	void SetMoveFlg(bool _flg) { m_moveFlg = _flg; }

	//ノードに着いたとみなす距離をセット
	// _dist ... 距離
	void SetNodeNearDistance(float _dist) { m_nodeNearDistance = _dist; }

private:

	//---- メンバ変数 ----

	//マップの管理
	std::weak_ptr<MapManager> m_wpMapManager;

	//座標
	Math::Vector3 m_pos;

	//目標地点
	Math::Vector3 m_nextPos;
	Math::Vector3 m_moveVec = Math::Vector3::Zero; // 進行方向ベクトル
	float m_speed = 10.0f; // 移動速度
	float m_nodeNearDistance = 1.0f; // ノード到達判定の半径

	// ノード経路情報
	std::vector<Node*> m_path;

	//今向かっている経路の番号
	int m_pathIndex = 0;

	// 動いているかどうか
	bool m_moveFlg = false;

	// 待機時間
	float m_waitMax = 0.0f;

	//今待っている時間
	float m_nowWait = 0.0f;
};
