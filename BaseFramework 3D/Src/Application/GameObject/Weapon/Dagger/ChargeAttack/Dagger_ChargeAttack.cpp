#include "Dagger_ChargeAttack.h"

#include"../../../Character/CharacterBase.h"

#include"../../../Terrains/Map/MapBase.h"
void Dagger_ChargeAttack::Init()
{
	if (!m_Poly)
	{
		m_Poly = std::make_shared<KdSquarePolygon>();
		m_Poly->SetMaterial("Asset/Textures/Weapon/Daerre/ChargeAttack/jet.png");
		m_Poly->SetScale(m_polyScale);
		m_Poly->SetPivot(KdSquarePolygon::PivotType::Center_Bottom);
	}


	if (!m_pDebugWire)
	{
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	}
}

void Dagger_ChargeAttack::Update()
{
	// ----------------------------------------------------
	// 1. 移動処理
	// ----------------------------------------------------
	bool isReachMaxDistance = false;
	if (m_shockwaveStatus.maxHitNum > 0 && m_shockwaveStatus.DistanceM < m_shockwaveStatus.maxDistanceM)
	{
		float yawRad = DirectX::XMConvertToRadians(m_angle);
		Math::Vector3 dir(sinf(yawRad), 0.0f, cosf(yawRad));
		dir.Normalize();
		Math::Vector3 distance = dir * m_shockwaveStatus.speed * DeltaTime::Instance().GetGameDeltaTime();
		// 最大距離を超える場合は距離ぴったりにクランプ
		if (m_shockwaveStatus.DistanceM + distance.Length() >= m_shockwaveStatus.maxDistanceM)
		{
			float remainDist = m_shockwaveStatus.maxDistanceM - m_shockwaveStatus.DistanceM;
			distance = dir * remainDist;
			isReachMaxDistance = true;
		}
		m_pos += distance;
		m_shockwaveStatus.DistanceM += distance.Length();
	}
	else
	{
		m_isExpired = true;
		return;
	}
	// ----------------------------------------------------
	// 2. ワールド行列の計算
	// ----------------------------------------------------
	Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
	m_mWorld = rMat * tMat;
	Math::Vector3 currPos = m_pos;

	// ----------------------------------------------------
	// 3. m_Poly のスケール変数 (m_polyScale) に連動したローカル AABB の自動計算
	// ----------------------------------------------------

	float moveDist = (currPos - m_prevTipPos).Length(); // 1フレームでの移動量
	float extentX = m_polyScale.x * 0.5f;   // 横幅の半分
	float extentY = m_polyThickness * 0.5f; // 高さの半分
	float extentZ = m_polyScale.y * 0.5f;   // ポリゴン本来の長さの半分

	// 移動量(moveDist)の半分を進行方向に伸ばしてすり抜けを防止
	float extendedExtentZ = extentZ + moveDist * 0.5f;
	float extendedCenterZ = extentZ - moveDist * 0.5f;
	DirectX::BoundingOrientedBox localOBB(
		Math::Vector3(0.0f, 0.0f, extendedCenterZ),
		Math::Vector3(extentX, extentY, extendedExtentZ),
		DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	// ★ワールド行列(回転 m_angle)をかけて進行方向に傾いた OBB に変換！
	DirectX::BoundingOrientedBox sweptOBB;
	localOBB.Transform(sweptOBB, m_mWorld);
	// OBB (isOriented = true) として BoxInfo を作成
	KdCollider::BoxInfo box(KdCollider::TypeGround | KdCollider::TypeDamage | KdCollider::TypeBump, sweptOBB);

	// ----------------------------------------------------
	// 4. 当たり判定処理
	// ----------------------------------------------------
	for (auto& wpGameObj : m_attackHitCharacterList)
	{
		auto spGameObj = wpGameObj.lock();
		if (!spGameObj) { continue; }
		if (IsAlreadyHit(spGameObj)) { continue; }

		std::list<KdCollider::CollisionResult> results;
		if (spGameObj->Intersects(box, &results))
		{
			m_hitCharactersList.push_back(spGameObj);

			float yawRad = DirectX::XMConvertToRadians(m_angle);
			Math::Vector3 dir(sinf(yawRad), 0.0f, cosf(yawRad));

			spGameObj->OnAttackHit(
				m_shockwaveStatus.maxDamage,
				5.0f,
				dir,
				0.05f,
				false,
				1.0f
			);

			float hitStopTime = 0.03f;
			DeltaTime::Instance().HitStop(hitStopTime);

			m_shockwaveStatus.maxHitNum--;
			if (m_shockwaveStatus.maxHitNum <= 0)
			{
				m_isExpired = true;
				break;
			}
		}
	}

	if (!m_isFirstFrame)
	{
		Math::Vector3 moveVec = m_pos - m_prevTipPos;
		// 移動量が 0 より大きい時（レイの方向ベクトルが存在する時）のみ判定
		if (moveVec.LengthSquared() > 0.0001f)
		{
			KdCollider::RayInfo mapRay(
				KdCollider::TypeGround | KdCollider::TypeBump,
				m_prevTipPos,
				m_pos
			);

			for (auto& wpMapObj : m_MapObjList)
			{
				auto spMapObj = wpMapObj.lock();
				if (!spMapObj) { continue; }
				if (spMapObj->Intersects(mapRay, nullptr))
				{
					m_isExpired = true;
					break;
				}
			}
		}
	}

	// ----------------------------------------------------
	// 5. デバッグワイヤー描画（ポリゴンの赤枠を表示）
	// ----------------------------------------------------
	if (m_pDebugWire)
	{
		// OBB の回転(Quaternion)と位置(Center)から行列を作成して描画
		Math::Matrix obbMat = Math::Matrix::CreateFromQuaternion(sweptOBB.Orientation) * Math::Matrix::CreateTranslation(sweptOBB.Center);
		m_pDebugWire->AddDebugBox(obbMat, sweptOBB.Extents, Math::Vector3::Zero, true, kRedColor);
	}

	// ----------------------------------------------------
	// 6.寿命チェック
	// ----------------------------------------------------
	if (isReachMaxDistance)
	{
		m_isExpired = true;
	}



	//残像生成
	GhostTrailUpdate();

	// ----------------------------------------------------
	tMat = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix r1Mat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(90));
	Math::Matrix r2Mat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
	m_mWorld = r1Mat * r2Mat * tMat;

	if (m_isFirstFrame)
	{
		m_isFirstFrame = false;
	}


	m_prevWorldMat = m_mWorld;
	m_prevTipPos = m_pos;


}


void Dagger_ChargeAttack::DrawEffect()
{
	// 残像を描画
	for (auto& t : m_trails)
	{
		Math::Matrix mat = m_mWorld;
		mat.Translation(t.pos);

		Math::Color color = { 1,1,1,t.alpha };
		KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_Poly, mat, color);
	}

	// 本体の斬撃を描画
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

void Dagger_ChargeAttack::GhostTrailUpdate()
{

	float deltaTime = DeltaTime::Instance().GetGameDeltaTime();;

	// 残像生成タイミング
	m_trailTimer += deltaTime;
	if (m_trailTimer >= m_trailSpawnInterval)
	{
		m_trailTimer = 0.0f;

		GhostTrail trail;
		trail.pos = m_pos;
		trail.alpha = 1.0f;      // 最初は不透明
		trail.lifeTime = 0.2f;   // 0.2秒で消える

		m_trails.push_back(trail);
	}

	// 残像の寿命処理
	for (auto& t : m_trails)
	{
		t.lifeTime -= deltaTime;
		t.alpha = t.lifeTime * 5.0f; // 徐々に透明に
	}

	// 寿命が尽きた残像を削除
	m_trails.erase(
		std::remove_if(m_trails.begin(), m_trails.end(),
			[](const GhostTrail& t) { return t.lifeTime <= 0.0f; }),
		m_trails.end()
	);
}
