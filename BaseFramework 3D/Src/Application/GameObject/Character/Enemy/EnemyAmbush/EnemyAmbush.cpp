#include "EnemyAmbush.h"
#include"../../../Terrains/Map/MapManager.h"

void EnemyAmbush::Init()
{
	wanderRadius = 15;

	m_isMovingToTarget = false;
}

void EnemyAmbush::Update()
{
	m_status.moveSpeed.nowSpeed =5;

	if (!m_playerChaseFlg)
	{

		if (m_returnSpawnPosFlg)
		{
			//スポーン地点に戻る
			ReturnSpawnPos();
		}
		else
		{
			//徘徊
			Wander();
		}




		//プレイヤーが視界にいるかどうか
		SearchPlayer();
	}
	else
	{
		PlayerChase();
	}

	

	AngeleUpdate();



	//座標行列を作る
	Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);
	//回転行列
	Math::Matrix rMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
	//行列の合成(S * R * T)
	m_mWorld = rMat * tMat;

	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugSphere(GetPos(), 1.0f);
		m_pDebugWire->AddDebugSphere(m_spawnPos, wanderRadius, { 1,1,1,1 });
	}



}

void EnemyAmbush::Wander()
{
	if (!m_isMovingToTarget)
	{
		if (m_stayTime > 0) 
		{
			m_stayTime -= DeltaTime::Instance().GetGameDeltaTime();
			return; 
		}

		//目的地に向かっていない場合、ランダムな方向にランダムな距離だけ進む目的地を決める

		//ランダム方向
		Math::Vector3 nextDir = Math::Vector3(KdRandom::GetFloat(-1, 1), 0, KdRandom::GetFloat(-1, 1));
		nextDir.Normalize();

		//スポーン位置から見てどれだけ進むか
		Math::Vector3 distance = Math::Vector3(KdRandom::GetFloat(0, wanderRadius), 0, KdRandom::GetFloat(0, wanderRadius));

		//結果のターゲット位置
		m_targetPos = m_spawnPos + (nextDir * distance);

		m_isMovingToTarget = true;
		m_moveTimeoutTimer = m_moveTimeoutMax;
	}
	else
	{
		//目的地に向かって移動する
		m_moveVec = m_targetPos - m_pos;
		m_moveVec.Normalize();
		m_pos += m_moveVec * m_status.moveSpeed.nowSpeed * DeltaTime::Instance().GetGameDeltaTime();


		//目的地に到達したかどうかor壁に当たってるかどうか
		if ((m_targetPos - m_pos).LengthSquared()< 0.1f)
		{
			m_isMovingToTarget = false;
			m_stayTime = m_arrivalWaitTime;
		}

		m_moveTimeoutTimer -= DeltaTime::Instance().GetGameDeltaTime();

		if (m_moveTimeoutTimer <= 0.0f)
		{
			m_isMovingToTarget = false;
			m_stayTime = m_arrivalWaitTime;
		}
	}
}


