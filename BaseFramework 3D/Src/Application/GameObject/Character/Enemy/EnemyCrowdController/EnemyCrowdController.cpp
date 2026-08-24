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

	// ★ moveVec をセットするだけ
	enemy->SetMoveVec(desired);
}

Math::Vector3 EnemyCrowdController::CalcAvoidance(EnemyBase* enemy)
{
	Math::Vector3 force = { 0,0,0 };
	float avoidDist = 0.8f;

	auto& enemies = enemy->GetEnemyManager()->GetEnemyList();

	for (auto& other : enemies)
	{
		if (other.get() == enemy) continue;

		Math::Vector3 diff = enemy->GetPos() - other->GetPos();
		diff.y = 0;

		float dist = diff.Length();
		if (dist < avoidDist && dist > 0.001f)
		{
			diff.Normalize();
			force += diff * (avoidDist - dist);
		}
	}

	return force;
}



Math::Vector3 EnemyCrowdController::CalcSeparation(EnemyBase* enemy)
{
	Math::Vector3 force = { 0,0,0 };
	float radius = 2.0f;

	auto& enemies = enemy->GetEnemyManager()->GetEnemyList();

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

