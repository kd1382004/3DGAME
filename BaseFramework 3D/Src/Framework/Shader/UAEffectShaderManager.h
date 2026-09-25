#pragma once

class UAEffectShaderManager
{
public:

	void Init();

	// エフェクトのクリア
	void ClearEffect();

	// 1エフェクトあたりのデータ
	struct CircleEffectData
	{
		Math::Vector3 colorPos = {};
		float colorRadius = 0.0f;
		Math::Vector3 colorColor = {};
		float dummy = 0.0f; // 16バイト境界パッキング用
	};

	struct BoxEffectData
	{
		Math::Vector3 colorPos = {};
		float colorRadiusX = 0.0f;

		float colorRadiusY = 0.0f;
		Math::Vector3 colorColor = {};

		float colorRadiusZ = 0.0f;
		float worldAngleY = 0;
		float dummy1 = 0.0f; // 16バイト境界パッキング用
		float dummy2 = 0.0f; // 16バイト境界パッキング用
	};

	// 定数バッファ構造体
	struct cbEffectData
	{
		int circleColorEnable = 0;
		int circleEffectNum = 0;
		int CircleDummy[2] = {};

		CircleEffectData circleEffects[10]; // 最大10個


		//ボックスカラー
		int boxColorEnable;
		int boxEffectNum; // 有効なエフェクトの個数
		int boxdummy[2] = {};
		BoxEffectData boxEffects[10]; // 最大10個まで保持
	};





	void WriteCBCircleEffectEnable(bool enable);
	void WriteCBCircleEffect(Math::Vector3 pos, float radius, Math::Vector3 color);

	void WriteCBBoxEffectEnable(bool enable);
	void WriteCBBoxEffect(Math::Vector3 pos,Math::Vector3 boxSiz, float worldAngleY,Math::Vector3 color);

	// 解放
	void Release();

private:



	// エフェクトの追加
	void AddCircleEffect(Math::Vector3 pos, float radius, Math::Vector3 color);

	UAEffectShaderManager() {};
	~UAEffectShaderManager() {};

	//エフェクト定数バッファ
	KdConstantBuffer<cbEffectData>	m_cb10_Effect;

public:
	static UAEffectShaderManager& Instance()
	{
		static UAEffectShaderManager instance;
		return instance;
	}
};

