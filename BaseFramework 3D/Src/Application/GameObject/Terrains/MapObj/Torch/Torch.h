#pragma once

#include"../MapObjBase.h"

class Torch :public MapObjBase
{
public:
	Torch() {};
	~Torch()override { Release(); };

	void Init()override;


	void PostUpdate()override;
	void PreDraw();
	void DrawLit();
	void GenerateDepthMapFromLight()override;
	void SetPos(const Math::Vector3& pos)override;

	void SetRotation(Math::Matrix _rMat);


	void SetCamera(std::shared_ptr<CameraBase>_camera) { m_wpCamera = _camera; }

	void SetInHaunk(bool _flg)override { SetEffectUpdate(_flg); };
private:

	void SetEffectUpdate(bool _flg);
	void Release();

	std::weak_ptr<CameraBase>m_wpCamera;



	Math::Vector3 m_pos;

	std::shared_ptr<KdModelWork>m_spTorchModel;

	//視錐台用のBoxInfo
	KdCollider::BoxInfo m_frustumBox;

	bool m_isInView = false;


	//エフェクト
	std::weak_ptr<KdEffekseerObject> m_wpAuraEffect;
	Math::Vector3 m_effectLocalPos;
	Math::Vector3 m_effectLocalColoer;
	float m_radius;

	float m_flickerTime = 0.0f;
	float m_flickerAmount = 0.1f; // 揺れ幅（半径に対する割合）
	float m_flickerSpeed =0.3;   // 揺れの速さの倍率

	//音
	std::shared_ptr<KdSoundInstance3D> m_spFireSE;

};
