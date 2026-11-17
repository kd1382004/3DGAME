#pragma once
#include"../MapBase.h"



class StairsBase :public MapBase
{
public:
	//コンストラクタ
	StairsBase() {};
	//デストラクタ
	~StairsBase() override;

	//初期化
	void Init()override;
	//更新
	void Update()override;

	//更新後の処理
	void PostUpdate()override;

	//当たり判定の処理
	void IsHit()override;

private:

	//プレイヤーが乗ってるかどうか
	bool m_playerHit=false;

	//階段のオーラのエフェクト
	std::weak_ptr<KdEffekseerObject> m_wpAuraEffect;

	//エフェクトを出す位置(ローカル)
	Math::Vector3 m_effectLocalPos;

	// 階段のまわりで鳴り続ける魔法の音（ループ・3D）
	std::shared_ptr<KdSoundInstance3D> m_spAmbient3DSE;
};
