#pragma once

class ButtonBase;

class ButtonManager :public KdGameObject
{
public:

	void InitTitel();
	void InitResult();
	virtual void Update()override;
	virtual void DrawSprite()override;

	void SetPos(const Math::Vector3& pos);

protected:

	void Init()override;

	std::list<std::shared_ptr<ButtonBase>> m_buttonList;

	std::vector<Math::Vector3>m_posList;

	int m_posListNum;

	std::shared_ptr < KdSoundInstance> m_bottonChange;
	std::shared_ptr < KdSoundInstance> m_bottonDecision;
};
