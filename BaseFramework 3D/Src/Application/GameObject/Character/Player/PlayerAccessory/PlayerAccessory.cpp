#include "PlayerAccessory.h"

#include"../PlayerBase.h"

#include"../../../Accessory/Flashlight/Flashlight.h"
void PlayerAccessory::Init()
{

}

void PlayerAccessory::Update(PlayerBase * _player)
{
	std::shared_ptr<Flashlight>spFlashlight = m_spFlashlight.lock();
	if (!spFlashlight) { return;}


	Math::Matrix Mat;


	Math::Vector3 hipPos = _player->GetBonePosition("Hips");
	float angle = _player->GetAngle();

	hipPos += _player->GetPos();

	Math::Matrix tMat = Math::Matrix::CreateTranslation(hipPos);
	Math::Matrix rMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(angle));

	Mat = rMat * tMat;

	spFlashlight->SetParentMatrix(Mat);
}
