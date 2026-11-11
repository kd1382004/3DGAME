#pragma once

#include"../AccessoryBase.h"


struct FlashlightConfig
{
	Math::Vector3 color = { 10,10,10 };
	float radius = 100.0f;
	float angle = 30.0f;

	Math::Vector3 localPos = { -0.35f, 0.0f, 0.0f }; // 腰からのずらし
	Math::Vector3 localRot = { 0.0f, 180.0f, 0.0f }; // 腰からの回転（度）
	bool stableLightDir = true; // ライトの向きはプレイヤーの正面に固定するか
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
