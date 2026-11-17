#pragma once
#include"../UIBase.h"

class NextFloorGaugeUI :public UIBase
{
public:

	//初期化
	void Init()override;
	//更新
	void Update()override;
	//2Dの描画
	void DrawSprite()override;


	//ゲージの割合をセット
	// _percent ... 割合(0〜1)
	void SetGaugePercent(float _percent) { m_gaugePercent = _percent; }

	//表示するかをセット
	// _flg ... 表示するか
	void SetDrawFlg(bool _flg) { m_drawFlg = _flg; }

	//表示するキーをセット
	// _key ... キーの番号
	void SetActionKey(int _key) { m_actionKey = _key; }
private:

	//サイズ
	Math::Vector2 m_siz;

	//ゲージの画像
	std::shared_ptr<KdTexture> m_spGaugeTexture = nullptr;

	//ゲージの割合(0〜1)
	float m_gaugePercent;

	//表示するか
	bool m_drawFlg = false;

	//キー表示の大きさ
	Math::Vector2 m_actionKeySiz;

	//表示するキー
	int m_actionKey = 'E';

	//キー表示のアルファ
	float m_actionKeyAlpha = 1.0f;
	//1フレームあたりのアルファの増減(点滅の速さ)
	float m_actionKeyPushAlpha = 0.01f;
	//キー表示のアルファの最小
	float m_actionKeyAlphaMin = 0.3f;
};
