#include "PlayreLV.h"
#include"../PlayerBase.h"

#include"../../../UI/HPBar/HPBar.h"
void PlayreLV::AddExp(int _gainedExp, PlayerBase* player)
{

	if (!player) { return; }

	n_nowExperiencePoints += _gainedExp;

	while (n_nowExperiencePoints >= m_nextLVExperiencePoints)
	{
		int nowLV = player->GetLV();
		nowLV += 1;

		n_nowExperiencePoints -= m_nextLVExperiencePoints;
		player->SetLV(nowLV);
	}
}

void PlayreLV::CalcNextLevelExp(int _LV)
{
	m_nextLVExperiencePoints = 10 + _LV * _LV * 10;
}
