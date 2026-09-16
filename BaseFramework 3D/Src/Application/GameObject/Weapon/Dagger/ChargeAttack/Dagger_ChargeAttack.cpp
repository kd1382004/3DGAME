#include "Dagger_ChargeAttack.h"

void Dagger_ChargeAttack::Init()
{
	if (!m_Poly)
	{
		m_Poly = std::make_shared<KdSquarePolygon>();
		m_Poly->SetMaterial("Asset/Textures/Weapon/Daerre/ChargeAttack/jet.png");
		m_Poly->SetScale({ 2,1 });
		m_Poly->SetPivot(KdSquarePolygon::PivotType::Center_Bottom);
	}
}

void Dagger_ChargeAttack::Update()
{
	if (m_shockwaveStatus.maxHitNum > 0 && m_shockwaveStatus.DistanceM <= m_shockwaveStatus.maxDistanceM)
	{
		//どの方向に進むか求める
		float yawRad = DirectX::XMConvertToRadians(m_angle);

		Math::Vector3 dir;
		dir.x = sinf(yawRad);
		dir.y = 0.0f;
		dir.z = cosf(yawRad);
		dir.Normalize();

		//今フレーム進む距離を求める
		Math::Vector3 distance= dir * m_shockwaveStatus.speed * DeltaTime::Instance().GetGameDeltaTime();

		//進ます
		m_pos += distance;

		//今フレーム進んだ距離を足す
		m_shockwaveStatus.DistanceM+=distance.Length();
	}
	else
	{
		m_isExpired = true;
	}


	Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix r1Mat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(90));
	Math::Matrix r2Mat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
	m_mWorld = r1Mat * r2Mat * tMat;
}

void Dagger_ChargeAttack::DrawEffect()
{
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_Poly, m_mWorld);
}

bool Dagger_ChargeAttack::IsAlreadyHit(const std::shared_ptr<CharacterBase>& _chara)
{
	for (auto& wp : m_hitCharactersList)
	{
		if (auto sp = wp.lock())
		{
			if (sp == _chara) return true;
		}
	}
	return false;
}
