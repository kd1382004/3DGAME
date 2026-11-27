//---- 自分のヘッダー ----
#include "SkeletonWarrior.h"			//敵(骸骨兵・黄昏の霊廟)

//=============================================
// 基本の処理
//=============================================

void SkeletonWarrior::Init()
{
	ScrapBot::Init();

	//骨の体なので、石のヒットエフェクトと音にする
	m_material = EnemyMaterial::Stone;
}