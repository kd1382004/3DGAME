#pragma once

class UAEffectShaderManager
{
public:

	void Init();

	// エフェクトのクリア
	void ClearEffect();

	// 1エフェクトあたりのデータ
	struct EffectData
	{
		Math::Vector3 colorPos = {};
		float colorRadius = 0.0f;
		Math::Vector3 colorColor = {};
		float dummy = 0.0f; // 16バイト境界パッキング用
	};

	// 定数バッファ構造体
	struct cbEffect
	{
		int colorEnable = 0;
		int effectNum = 0;
		int dummy[2] = {};

		EffectData effects[10]; // 最大10個
	};





	void WriteCBColoerEnable(bool enable);
	void WriteCBColoer(Math::Vector3 pos, float radius, Math::Vector3 color);

	// 解放
	void Release();

private:



	// エフェクトの追加
	void AddEffect(Math::Vector3 pos, float radius, Math::Vector3 color);

	UAEffectShaderManager() {};
	~UAEffectShaderManager() {};

	//エフェクト定数バッファ
	KdConstantBuffer<cbEffect>	m_cb10_Effect;

public:
	static UAEffectShaderManager& Instance()
	{
		static UAEffectShaderManager instance;
		return instance;
	}
};

