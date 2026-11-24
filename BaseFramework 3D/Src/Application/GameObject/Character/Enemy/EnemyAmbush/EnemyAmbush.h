#pragma once
#include"../EnemyBase.h"

//敵の待ち伏せ出現
class EnemyAmbush :public EnemyBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	EnemyAmbush() {};

	//デストラクタ
	~EnemyAmbush()override {};

	//---- 基本の処理 ----

	//初期化
	virtual void Init()override;

	//更新
	virtual void Update()override;

	//---- その他の処理 ----

	//徘徊
	void  Wander();

private:

	//---- メンバ変数 ----

	//目的地に向かってるかどうか
	bool m_isMovingToTarget = false;

	//目的地
	Math::Vector3 m_targetPos;

	//目的地に向かい始めてからの時間
	float m_moveTimeoutTimer = 0;

	//目的地に着けなかったとみなす時間(秒)
	float m_moveTimeoutMax = 3.0f;
};
