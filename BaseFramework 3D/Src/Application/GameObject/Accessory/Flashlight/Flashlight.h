#pragma once

#include"../AccessoryBase.h"


struct FlashlightConfig
{
	//ライトの色(明るさ込み)
	Math::Vector3 m_color = { 10,10,10 };
	//ライトが届く距離
	float m_radius = 100.0f;
	//ライトの広がる角度(度)
	float m_angle = 30.0f;

	Math::Vector3 m_localPos = { -0.35f, 0.0f, 0.0f }; // 腰からのずらし
	Math::Vector3 m_localRot = { 0.0f, 180.0f, 0.0f }; // 腰からの回転（度）
	bool m_stableLightDir = true; // ライトの向きはプレイヤーの正面に固定するか
};

class Flashlight :public AccessoryBase
{
public:
	//コンストラクタ
	Flashlight() {};
	//デストラクタ
	~Flashlight()override {};

	//初期化
	void Init() override;
	//更新
	void Update() override;
	//更新後の処理(ライトの位置・向きを決める)
	void PostUpdate() override;

	//影の描画
	void GenerateDepthMapFromLight() override;
	//ライトありの描画
	void DrawLit() override;
	//光る部分の描画
	void DrawBright() override;

	//ライトの設定を変えるImGui
	void DrawImGui();

private:


	//懐中電灯のモデル
	std::shared_ptr<KdModelWork>m_spFlashlightModel;

	//ライトの設定
	FlashlightConfig m_flashlightconfig;
};
