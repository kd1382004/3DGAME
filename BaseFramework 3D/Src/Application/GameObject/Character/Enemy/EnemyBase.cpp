#include "EnemyBase.h"
#include"../../Camera/CameraBase.h"

//プレイヤー
#include"../Player/PlayerBase.h"

void EnemyBase::Init()
{
}

void EnemyBase::PreUpdate()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (spPlayer)
	{
		m_playerPos = spPlayer->GetPos();
	}
}

void EnemyBase::Update()
{
	


}

void EnemyBase::PostUpdate()
{
	CharacterBase::PostUpdate();
}

void EnemyBase::PreDraw()
{
	//カメラに映ってるかどうか
	std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		m_isInView = CheckInScreen(spCamera->GetBoundingFrustum(), m_frustumBox);
	}
}

void EnemyBase::SearchPlayer()
{
	Math::Vector3 dir = m_moveDir;             
	Math::Vector3 toPlayer = (m_playerPos - m_pos);
	toPlayer.Normalize();

	float dot = dir.Dot(toPlayer);

	float cosView = cosf(m_viewAngle * 0.5f);

	if (dot > cosView)
	{
		// 視界に入っている
		float dist = Math::Vector3::Distance(m_pos, m_playerPos);

		if (dist < m_viewDistance)
		{
			m_playerChaseFlg = true;
		}
	}

	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugLine(m_pos, dir, m_viewDistance);
	}

}

void EnemyBase::PlayerChase()
{
	m_moveDir = m_playerPos - m_pos;
	m_moveDir.y = 0;
	m_moveDir.Normalize();


	float dist = (m_playerPos - m_pos).Length();

	if (dist > m_loseSightDistance)
	{
		
		if (m_stayTime > 0)
		{
			m_stayTime -= DeltaTime::Instance().GetGameDeltaTime();
			return;
		}
		else
		{
			m_playerChaseFlg = false;
		}
	}
	else
	{
		m_stayTime = m_lostSightWaitTime;

		//Chase範囲内なら座標更新
		m_pos += m_moveDir * m_status.moveSpeed.nowSpeed * DeltaTime::Instance().GetGameDeltaTime();
	}



	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugLine(m_pos, m_moveDir, m_loseSightDistance);
	}

}
