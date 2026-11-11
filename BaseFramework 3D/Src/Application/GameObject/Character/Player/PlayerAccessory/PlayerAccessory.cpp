#include "PlayerAccessory.h"

#include"../PlayerBase.h"

#include"../../../Accessory/Flashlight/Flashlight.h"
void PlayerAccessory::Init()
{

}

void PlayerAccessory::Update(PlayerBase * _player)
{
	std::shared_ptr<Flashlight> spFlashlight = m_spFlashlight.lock();
	if (!spFlashlight) { return; }

	// 腰ボーンのワールド行列（アニメーションの移動・回転込み）
	spFlashlight->SetParentMatrix(_player->GetBoneWorldMatrix("Hips"));
	// 向きを安定させる用にプレイヤー本体の行列も渡す
	spFlashlight->SetOwnerMatrix(_player->GetMatrix());
}
