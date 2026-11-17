#pragma once

#include"EffectBase.h"
class EffectManager :public KdGameObject
{
public:
	//コンストラクタ
	EffectManager() {};
	//デストラクタ
	~EffectManager()override {};

	//初期化
	void Init()override;
	//更新前の処理
	void PreUpdate()override;
	//更新
	void Update()override;

	//エフェクトの描画
	void DrawEffect()override;
	//2D描画
	void DrawSprite()override;

	//エフェクトを追加する
	// _spEffect ... 追加するエフェクト
	void AddEffectList(std::shared_ptr<EffectBase> _spEffect) { m_effectBaseList.push_back(_spEffect); }
private:

	//エフェクトのリスト
	std::list< std::shared_ptr<EffectBase>>m_effectBaseList;

};
