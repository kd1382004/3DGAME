#pragma once
#include"../UIBase.h"

class PotionTexInfo;
class PlayerBase;

class BuffUI :public UIBase
{
public:
	BuffUI() {};
	~BuffUI() override{};

	void Init()override;

	void PreDraw()override;
	void DrawSprite()override;

	void SetPotionTexInfo(std::shared_ptr<PotionTexInfo> _spPotionTexInfo) { m_wpPotionTexInfo = _spPotionTexInfo; }

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayer) { m_wpPlayer = _spPlayer; }
private:
	std::weak_ptr<PotionTexInfo>m_wpPotionTexInfo;
	std::weak_ptr<PlayerBase>m_wpPlayer;

	struct BuffUIInfo {

		int m_ID;
		float m_angle = 0;
		Math::Vector2 m_pos;

	};

	Math::Vector2 m_iconSiz;

	std::list<BuffUIInfo> m_PotionBuffUIInfoList;

	std::shared_ptr<KdTexture>m_buffTex;
};
