#include "PlayreLV.h"
#include"../PlayerBase.h"
#include"../../../../Info/VOL/VOLInfo.h"

#include"../../../UI/HPBar/HPBar.h"
void PlayreLV::AddExp(int _gainedExp, PlayerBase* player)
{

	if (!player) { return; }

	n_nowExperiencePoints += _gainedExp;

	bool lvUpFlg = false;	// このとき1回でもレベルが上がったか

	while (n_nowExperiencePoints >= m_nextLVExperiencePoints)
	{
		int nowLV = player->GetLV();
		nowLV += 1;

		n_nowExperiencePoints -= m_nextLVExperiencePoints;
		player->SetLV(nowLV);

		lvUpFlg = true;
	}

	// レベルアップの光（足元）。一度に何レベル上がっても1回だけ
	if (lvUpFlg)
	{
		KdEffekseerManager::GetInstance().Play("LevelUp/LevelUp.efkefc", player->GetPos(), 1.0f, 1.0f, false);

		// レベルアップの音
		{
			std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sound/SE/Player/LevelUp.wav", false);
			if (se) { se->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
		}
	}
}

void PlayreLV::CalcNextLevelExp(int _LV)
{
	m_nextLVExperiencePoints = 10 + _LV * _LV * 10;
}
