//---- 自分のヘッダー ----
#include "EnemyCrowdController.h"		//敵同士の押し合い・群れ制御

//---- 敵 ----
#include"../EnemyBase.h"				//敵の基底クラス
#include"../EnemyManager.h"				//敵の管理

//=============================================
// 基本の処理
//=============================================

void EnemyCrowdController::Update(EnemyBase* _enemy, float _dt)
{
	Math::Vector3 toPlayer = CalcToPlayer(_enemy);
	Math::Vector3 separation = CalcSeparation(_enemy);
	Math::Vector3 avoidance = CalcAvoidance(_enemy);

	Math::Vector3 desired =
		toPlayer * 1.0f
		+ separation * 0.6f
		+ avoidance * 1.2f;

	if (desired.LengthSquared() > 0.0001f)
		desired.Normalize();

	_enemy->SetMoveVec(desired);
}

//=============================================
// その他の処理
//=============================================

Math::Vector3 EnemyCrowdController::CalcSeparation(EnemyBase* _enemy)
{
	Math::Vector3 force = { 0,0,0 };
	float radius = 2.0f;

	auto& enemies = _enemy->GetEnemyManager()->GetEnemyUpdateList();

	for (auto& other : enemies)
	{
		if (other.get() == _enemy) continue;

		Math::Vector3 diff = _enemy->GetPos() - other->GetPos();
		diff.y = 0;

		float dist = diff.Length();
		if (dist < radius && dist > 0.001f)
		{
			force += diff / (dist * dist) * 0.5;
		}
	}

	return force;
}

Math::Vector3 EnemyCrowdController::CalcAvoidance(EnemyBase* _enemy)
{
	Math::Vector3 force = Math::Vector3::Zero;
	if (!_enemy) return force;

	auto spManager = _enemy->GetEnemyManager();
	if (!spManager) return force;

	const float avoidDist = 0.8f;
	const float avoidDistSq = avoidDist * avoidDist;
	const float maxForce = 1.0f;   // 押し出しの上限

	const Math::Vector3 myPos = _enemy->GetPos();

	for (auto& other : spManager->GetEnemyUpdateList())
	{
		if (!other || other.get() == _enemy) continue;
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
			dir = (_enemy < other.get()) ? Math::Vector3(1, 0, 0) : Math::Vector3(-1, 0, 0);
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

Math::Vector3 EnemyCrowdController::CalcToPlayer(EnemyBase* _enemy)
{
	Math::Vector3 toPlayer = _enemy->GetPlayerPos() - _enemy->GetPos();
	toPlayer.y = 0;
	if (toPlayer.LengthSquared() > 0.0001f)
		toPlayer.Normalize();

	return toPlayer;
}
