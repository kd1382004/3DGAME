#include "EnemyCrowdController.h"
#include"../EnemyBase.h"
#include"../EnemyManager.h"
void EnemyCrowdController::Update(EnemyBase* enemy, float dt)
{
	Math::Vector3 toPlayer = CalcToPlayer(enemy);
	Math::Vector3 separation = CalcSeparation(enemy);
	Math::Vector3 avoidance = CalcAvoidance(enemy);

	Math::Vector3 desired =
		toPlayer * 1.0f
		+ separation * 0.6f
		+ avoidance * 1.2f;

	if (desired.LengthSquared() > 0.0001f)
		desired.Normalize();

	enemy->SetMoveVec(desired);
}

Math::Vector3 EnemyCrowdController::CalcAvoidance(EnemyBase* enemy)
{
	Math::Vector3 force = Math::Vector3::Zero;
	if (!enemy) return force;

	auto spManager = enemy->GetEnemyManager();
	if (!spManager) return force;

	const float avoidDist = 0.8f;
	const float avoidDistSq = avoidDist * avoidDist;
	const float maxForce = 1.0f;   // 押し出しの上限

	const Math::Vector3 myPos = enemy->GetPos();

	for (auto& other : spManager->GetEnemyUpdateList())
	{
		if (!other || other.get() == enemy) continue;
		// if (other->IsDead()) continue;   // 死亡判定がある場合

		Math::Vector3 diff = myPos - other->GetPos();
		diff.y = 0.0f;

		float distSq = diff.LengthSquared();
		if (distSq >= avoidDistSq) continue;   // 遠い敵はここで除外（平方根なし）

		float dist = std::sqrt(distSq);

		Math::Vector3 dir;
		if (dist > 0.001f)
		{
			dir = diff / dist;
		}
		else
		{
			// 完全に重なっている → アドレスの大小で方向を決めてお互い逆に押す
			dir = (enemy < other.get()) ? Math::Vector3(1, 0, 0) : Math::Vector3(-1, 0, 0);
			dist = 0.0f;
		}

		// 近いほど強く押す（0〜1）
		float strength = (avoidDist - dist) / avoidDist;
		force += dir * strength;
	}

	// 押し出しすぎ防止
	if (force.LengthSquared() > maxForce * maxForce)
	{
		force.Normalize();
		force *= maxForce;
	}

	return force;
}



Math::Vector3 EnemyCrowdController::CalcSeparation(EnemyBase* enemy)
{
	Math::Vector3 force = { 0,0,0 };
	float radius = 2.0f;

	auto& enemies = enemy->GetEnemyManager()->GetEnemyUpdateList();

	for (auto& other : enemies)
	{
		if (other.get() == enemy) continue;

		Math::Vector3 diff = enemy->GetPos() - other->GetPos();
		diff.y = 0;

		float dist = diff.Length();
		if (dist < radius && dist > 0.001f)
		{
			force += diff / (dist * dist) * 0.5;
		}
	}

	return force;
}



Math::Vector3 EnemyCrowdController::CalcToPlayer(EnemyBase* enemy)
{
	Math::Vector3 toPlayer = enemy->GetPlayerPos() - enemy->GetPos();
	toPlayer.y = 0;
	if (toPlayer.LengthSquared() > 0.0001f)
		toPlayer.Normalize();

	return toPlayer;
}

