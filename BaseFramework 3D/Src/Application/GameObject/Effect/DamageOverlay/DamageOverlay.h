#pragma once

#include"../EffectBase.h"
class DamageOverlay:public EffectBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	DamageOverlay() {};

	//デストラクタ
	~DamageOverlay()override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新前の処理
	void PreUpdate()override;

	//更新(だんだん薄くする)
	void Update()override;

	//画面の端を赤くする画像を描く
	void DrawSprite()override;

	//---- その他の処理 ----

	//ダメージを受けたときに呼ぶ(受けたダメージの割合で濃さを決める)
	// _damage ... 受けたダメージ
	// _maxHP ... 最大HP
	void OnDamaged(float _damage, float _maxHP)
	{
		// 最大体力に対してどれくらい削れたか（0〜1）
		float ratio = _damage / _maxHP;

		// 濃さを調整
		float intensity = std::min(0.7f, ratio * 2.0f);

		m_damageOverlayAlpha = intensity;
	}

private:

	//---- メンバ変数 ----

	//画面の端を赤くする画像
	std::shared_ptr<KdTexture>m_spDamageOverlayTex;

	//今の濃さ(不透明度)
	float m_damageOverlayAlpha;

	//1秒で薄くなる速さ
	float m_damageOverlayFadeSpeed = 1.0f;
};
