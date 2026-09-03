#pragma once

#include"../AccessoryBase.h"


struct FlashlightConfig
{
	Math::Vector3 color = {10,9,7 };  // 光の色・強度
	float radius = 200.0f;                         // 照射距離
	float angle = 30.0f;                           // 照射角度
};

class Flashlight :public AccessoryBase
{
public:
	Flashlight() {};
	~Flashlight()override {};

	void Init() override;
	void Update() override;
	void PostUpdate() override;

	void GenerateDepthMapFromLight() override;
	void DrawLit() override;
	void DrawBright() override;

	void DrawImGui();

private:


	std::shared_ptr<KdModelWork>m_spFlashlightModel;

	FlashlightConfig m_flashlightconfig;
};
