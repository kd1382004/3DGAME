//---- 自分のヘッダー ----
#include "BoneBehemoth.h"			//敵(骨の巨躯・黄昏の霊廟)

//=============================================
// 基本の処理
//=============================================

void BoneBehemoth::Init()
{
	PressBot::Init();

	//骨の体なので、石のヒットエフェクトと音にする
	m_material = EnemyMaterial::Stone;
}