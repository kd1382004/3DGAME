#pragma once
#include"../UIBase.h"

class NextFloorGaugeUI :public UIBase
{
public:

	void Init()override;
	void Update()override;
	void DrawSprite()override;


	void SetGaugePercent(float _percent) { m_gaugePercent = _percent; }

	void setDrawFlg(bool _flg) { m_drawFlg = _flg; }

	void SetActionKey(int _key) { m_actionKey = _key; }
private:

	//サイズ
	Math::Vector2 m_siz;

	std::shared_ptr<KdTexture> m_spGaugeTexture = nullptr;

	float m_gaugePercent;

	bool m_drawFlg = false;

	Math::Vector2 m_actionKeySiz;

	int m_actionKey = 'E';

	float m_actionKeyAlpha = 1.0f;
	float m_actionKeyPushAlpha = 0.01f;
	float m_actionKeyAlphaMine = 0.3f;
};
