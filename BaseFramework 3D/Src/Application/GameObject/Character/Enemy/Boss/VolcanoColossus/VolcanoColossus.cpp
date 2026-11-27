//---- 自分のヘッダー ----
#include "VolcanoColossus.h"			//ボス(火山の巨神・火山帯)

//=============================================
// 基本の処理
//=============================================

void VolcanoColossus::Init()
{
	Titan::Init();

	//岩の体なので、石のヒットエフェクトと音にする
	m_material = EnemyMaterial::Stone;
}
