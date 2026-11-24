//---- 自分のヘッダー ----
#include "PlayerAccessory.h"								//プレイヤーの装着アクセサリー

//---- プレイヤー ----
#include"../PlayerBase.h"									//プレイヤー

//---- アクセサリー ----
#include"../../../Accessory/Flashlight/Flashlight.h"		//懐中電灯

void PlayerAccessory::Init()
{

}

void PlayerAccessory::Update(PlayerBase * _player)
{
	std::shared_ptr<Flashlight> spFlashlight = m_wpFlashlight.lock();
	if (!spFlashlight) { return; }

	// 腰ボーンのワールド行列（アニメーションの移動・回転込み）
	spFlashlight->SetParentMatrix(_player->GetBoneWorldMatrix("Hips"));
	// 向きを安定させる用にプレイヤー本体の行列も渡す
	spFlashlight->SetOwnerMatrix(_player->GetMatrix());
}
