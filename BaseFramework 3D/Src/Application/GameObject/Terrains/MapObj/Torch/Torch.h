#pragma once

#include"../MapObjBase.h"

class Torch:public MapObjBase
{
public:
	Torch() {};
	~Torch() {};

	void Init()override;

	void PreDraw();
	void DrawLit();

	void SetPos(const Math::Vector3& pos)override;

	void SetRotation(Math::Matrix _rMat);


	void SetCamera(std::shared_ptr<CameraBase>_camera) { m_wpCamera = _camera; }
private:

	std::weak_ptr<CameraBase>m_wpCamera;


	Math::Vector3 m_pos;

	std::shared_ptr<KdModelWork>m_spTorchModel;

	//視錐台用のBoxInfo
	KdCollider::BoxInfo m_frustumBox;

	bool m_isInView = false;
};
