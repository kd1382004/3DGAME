#pragma once

#include"../Character/CharacterBase.h"
class MapManager;

struct Node; // ノード構造体の前方宣言を追加
class TitelMove :public CharacterBase
{
public:
	TitelMove();
	~TitelMove() override {};
	void Update() override;
	void PostUpdate() override;
	void SetPos(const Math::Vector3& pos)
	{
		m_pos = pos;
		m_mWorld.Translation(pos);
	}

	// MapManagerの登録
	void SetMapManager(std::shared_ptr<MapManager> spMapManager)
	{
		m_wpMapManager = spMapManager;
	}

	// 目標地点の設定・取得
	void SetNextPos(const Math::Vector3& nextPos) { m_nextPos = nextPos; }
	const Math::Vector3& GetNextPos() const { return m_nextPos; }
	// 移動フラグ・移動速度のアクセサ
	void SetSpeed(float speed) { m_speed = speed; }
	float GetSpeed() const { return m_speed; }
	bool IsMoving() const { return m_moveFlg; }
	void SetMoveFlg(bool flg) { m_moveFlg = flg; }

	void SetNodeNearDistance(float dist) { m_nodeNearDistance = dist; }
	float GetNodeNearDistance() const { return m_nodeNearDistance; }

	const Math::Vector3& GetMoveVec() const { return m_moveVec; }

private:

	std::weak_ptr<MapManager> m_wpMapManager;

	Math::Vector3 m_pos;
	Math::Vector3 m_nextPos;
	Math::Vector3 m_moveVec = Math::Vector3::Zero; // 進行方向ベクトル
	float m_speed = 10.0f;
	float m_nodeNearDistance = 1.0f; // ノード到達判定の半径
	// ノード経路情報
	std::vector<Node*> m_path;
	int m_pathIndex = 0;
	// 動いているかどうか
	bool m_moveFlg = false;
	// 待機時間
	float m_watiMax = 0.0f;
	float m_nowWati = 0.0f;
};
