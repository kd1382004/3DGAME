#pragma once


class ButtonBase :public KdGameObject
{
public:

	virtual void Init()override;
	virtual void Update()override;
	void DrawSprite()override;

	void SetPos(const Math::Vector3& pos);

	void KeyChosen(bool _choseFlg);
protected:

	std::shared_ptr<KdTexture> m_tex;
	Math::Vector2 m_pos = {};
	Math::Vector2 m_radius = {};
	float m_scale = 1.0f;
	float m_defScale = 2.0;
	float m_hitScale = 1.5;


	bool m_choseFlg;

	float m_alpha = 1;
	float m_alphaMine = 0.3;
	float m_alphaMax = 1;
	float m_alphaAdd = 0.01;

	void AlphaFlashing();

	void ImGUI()override;
};
