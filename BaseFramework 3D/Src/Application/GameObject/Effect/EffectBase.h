#pragma once

class EffectBase:public KdGameObject
{
public:
	EffectBase() {};
	~EffectBase() override{};

	void Init()override;
	void PreUpdate()override;
	void Update()override;

	void DrawEffect()override;
	void DrawSprite()override;

private:

};
