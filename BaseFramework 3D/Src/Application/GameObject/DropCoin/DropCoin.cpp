//---- 自分のヘッダー ----
#include "DropCoin.h"												//敵が落とすコイン

//---- プレイヤー ----
#include"../Character/Player/PlayerBase.h"							//プレイヤー
#include"../Character/Player/PlayerInventory/PlayerInventory.h"		//プレイヤーの所持品

//---- 便利機能 ----
#include"../../Info/VOL/VOLInfo.h"									//音量設定
#include"../../Info/DeltaTime/DeltaTime.h"							//デルタタイム

void DropCoin::Init()
{
	if (!m_spCoinModel)
	{
		m_spCoinModel = std::make_shared<KdModelData>();
		m_spCoinModel->Load("Asset/Models/DropCoin/Coin.gltf");
	}

	m_dropCoinState = DropCoinState::Pop;
}

void DropCoin::PreUpdate()
{

}

void DropCoin::Update()
{
	const float dt = DeltaTime::Instance().GetGameDeltaTime();

	// くるくる回す
	m_rotY += m_rotSpeed * dt;
	if (m_rotY >= 360.0f) { m_rotY -= 360.0f; }

	switch (m_dropCoinState)
	{
	case DropCoinState::Pop:
		PopUpdate();
		break;
	case DropCoinState::Wait:
		WaitUpdate();
		break;
	case DropCoinState::Attract:
		AttractUpdate();
		break;
	case DropCoinState::Get:
		GetCoin();
		break;
	default:
		break;
	}

	Math::Matrix sMat = Math::Matrix::CreateScale(m_scale);
	Math::Matrix rMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_rotY));
	Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = sMat * rMat * tMat;
}

void DropCoin::DrawLit()
{
	//if (m_spCoinModel)
	//{
	//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spCoinModel, m_mWorld);
	//}
}

void DropCoin::DrawEffect()
{

}

void DropCoin::DrawBright()
{
	if (m_spCoinModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spCoinModel, m_mWorld);
	}
}

void DropCoin::PopUpdate()
{
	float dt = DeltaTime::Instance().GetGameDeltaTime();

	if (!m_popInit)
	{
		m_popInit = true;
		m_groundY = m_pos.y;

		float deg = KdRandom::GetFloat(0, 360);
		float rad = DirectX::XMConvertToRadians(deg);
		float side = m_popSideSpeed * KdRandom::GetFloat(0.3f, 1.0f);

		m_vel.x = cos(rad) * side;
		m_vel.y = KdRandom::GetFloat(1, m_popUpSpeed);
		m_vel.z = sin(rad) * side;
	}

	m_vel.y -= m_gravity * dt;
	m_pos += m_vel * dt;



	if (m_pos.y <= m_groundY)
	{
		m_pos.y = m_groundY;
		m_dropCoinState = DropCoinState::Wait;
	}
}

void DropCoin::WaitUpdate()
{
	float dt = DeltaTime::Instance().GetGameDeltaTime();

	m_waitTimer += dt;


	if (m_waitTimer >= m_waitTime)
	{
		m_dropCoinState = DropCoinState::Attract;
		m_attractStartPos = m_pos;
	}
}

void DropCoin::AttractUpdate()
{
	std::shared_ptr<PlayerBase> spPlayer = m_wpPlayer.lock();
	if (!spPlayer)
	{
		// プレイヤーがいなければ消すだけ
		m_isExpired = true;
		return;
	}

	float dt = DeltaTime::Instance().GetGameDeltaTime();
	m_attractTimer += dt;

	// 足元ではなく胸あたりを狙う
	m_playerPos = spPlayer->GetPos();
	m_playerPos.y += m_targetHeight;

	// 経過時間の割合（0～1）。距離に関係なく m_attractTime 秒で届く
	float t = std::min(m_attractTimer / m_attractTime, 1.0f);
	// 2乗して、最初はゆっくり・だんだん速く
	t = t * t;

	// スタート位置とプレイヤーの間のどこにいるかを割合で決める
	m_pos = Math::Vector3::Lerp(m_attractStartPos, m_playerPos, t);

	// 届いたら取得
	if (t >= 1.0f || (m_playerPos - m_pos).Length() < m_getRadius)
	{
		m_pos = m_playerPos;
		m_dropCoinState = DropCoinState::Get;
	}
}

void DropCoin::GetCoin()
{
	std::shared_ptr<PlayerBase> spPlayer = m_wpPlayer.lock();
	if (spPlayer && spPlayer->GetPlayerInventory())
	{
		// 所持金に加算
		spPlayer->GetPlayerInventory()->AddMoney(m_value);

		// 拾った音
		std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Item/Get.wav", false);
		if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
	}

	m_isExpired = true;
}
