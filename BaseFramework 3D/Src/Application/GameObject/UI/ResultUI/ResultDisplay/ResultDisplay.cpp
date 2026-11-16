#include "ResultDisplay.h"

#include"ResultDisplayBase.h"
#include"ResultDisplay_Score/ResultDisplay_Score.h"
#include"ResultDisplay_PotionInventory/ResultDisplay_PotionInventory.h"
#include"ResultDisplay_PlayerStatus/ResultDisplay_PlayerStatus.h"
#include"ResultDisplay_WeponInventory/ResultDisplay_WeponInventory.h"

#include"../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Info/VOL/VOLInfo.h"

#include"../../../Character/Player/PlayerInventory/PlayerInventory.h"
void ResultDisplay::Init()
{

	if (!m_ResultBackTex)
	{
		m_ResultBackTex = std::make_shared<KdTexture>();
		m_ResultBackTex->Load("Asset/Textures/GameUI/Result/ResultBack.png");
	}
	if (!m_ResultBack2Tex)
	{
		m_ResultBack2Tex = std::make_shared<KdTexture>();
		m_ResultBack2Tex->Load("Asset/Textures/GameUI/Result/back.png");
	}

	if (!m_ArrowTex)
	{
		m_ArrowTex = std::make_shared<KdTexture>();
		m_ArrowTex->Load("Asset/Textures/GameUI/Result/Arrow.png");
		m_ArrowSiz = { (float)m_ArrowTex->GetWidth(),(float)m_ArrowTex->GetHeight() };
		m_ArrowLPos = { -400, 0 };
		m_ArrowRPos = { 400, 0 };
	}

	m_ResultDisplay_Score = std::make_shared<ResultDisplay_Score>();
	m_ResultDisplay_Score->Init();
	m_ResultDisplayList.push_back(m_ResultDisplay_Score);



	m_ResultDisplay_PotionInventory = std::make_shared<ResultDisplay_PotionInventory>();
	m_ResultDisplay_PotionInventory->Init();
	m_ResultDisplayList.push_back(m_ResultDisplay_PotionInventory);

	m_ResultDisplay_WeponInventory = std::make_shared<ResultDisplay_WeponInventory>();
	m_ResultDisplay_WeponInventory->Init();
	m_ResultDisplayList.push_back(m_ResultDisplay_WeponInventory);

	m_ResultDisplay_PlayerStatus = std::make_shared<ResultDisplay_PlayerStatus>();
	m_ResultDisplay_PlayerStatus->Init();
	m_ResultDisplayList.push_back(m_ResultDisplay_PlayerStatus);
}

void ResultDisplay::Update()
{

	ArrowUpdate();

	LostUpdate();

	if (m_CurrentDisplayIndex < 0 || m_CurrentDisplayIndex >= m_ResultDisplayList.size()) { return; }
	m_ResultDisplayList[m_CurrentDisplayIndex]->Update();
}

void ResultDisplay::PreDraw()
{
	if (m_CurrentDisplayIndex < 0 || m_CurrentDisplayIndex >= m_ResultDisplayList.size()) { return; }
	m_ResultDisplayList[m_CurrentDisplayIndex]->PreDraw();
}

void ResultDisplay::DrawSprite()
{
	if (m_ResultBackTex)
	{
		Math::Color col = { 3,3,3,1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_ResultBackTex, 0, 0, nullptr, &col);
	}

	if (m_ResultBack2Tex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_ResultBack2Tex, 0, 0, nullptr);
	}



	ArrowDrawSprite();

	if (m_CurrentDisplayIndex < 0 || m_CurrentDisplayIndex >= m_ResultDisplayList.size()) { return; }
	m_ResultDisplayList[m_CurrentDisplayIndex]->DrawSprite();


	if (m_CurrentDisplayIndex > 0)
	{
		if (m_playerDead)
		{
			LostDraw();
		}
	}

}

void ResultDisplay::SetPath(std::string path)
{

}

void ResultDisplay::SetDisplayFloorInfo(std::vector<DisplayFloorInfo> _displayFloorInfo)
{
	m_ResultDisplay_Score->SetDisplayFloorInfo(_displayFloorInfo);
}

void ResultDisplay::SetPlayerInventoryList(std::vector<std::vector<Inventory>> _playerInventoryList)
{
	m_playerInventoryList = _playerInventoryList;


	if (m_ResultDisplay_PotionInventory)
	{
		m_ResultDisplay_PotionInventory->AddPotionTexInfo(_playerInventoryList[0]);
	}
	if (m_ResultDisplay_WeponInventory)
	{
		m_ResultDisplay_WeponInventory->WeaponsStrengtheningInfoInit(_playerInventoryList[1]);
	}

}

void ResultDisplay::SetPlayerStatusGameResultFilePath(std::string _path)
{
	if (m_ResultDisplay_PlayerStatus)
	{
		m_ResultDisplay_PlayerStatus->SetPlayerStatusGameResultFilePath(_path);
	}
}

void ResultDisplay::SetSpWeaponBase(std::shared_ptr<WeaponBase> spWeaponBase)
{
	if (m_ResultDisplay_WeponInventory)
	{
		m_ResultDisplay_WeponInventory->SetSpWeaponBase(spWeaponBase);
	}
	if (m_ResultDisplay_PlayerStatus)
	{
		m_ResultDisplay_PlayerStatus->SetSpWeaponBase(spWeaponBase);
	}
}

void ResultDisplay::SetPlayerDead(bool _flg)
{
	m_playerDead = _flg;

	if (m_playerDead)
	{
		if (!m_lostTex)
		{
			m_lostTex = std::make_shared<KdTexture>();
			m_lostTex->Load("Asset/Textures/GameUI/Result/Lost.png");

			m_lostTexPos = { 390, 270 };

			m_lostTexSiz.x = m_lostTex->GetWidth();
			m_lostTexSiz.y = m_lostTex->GetHeight();
		}

		m_lostStampAngle = kLostStampBaseAngle + KdRandom::GetFloat(-kLostStampAngleRange, kLostStampAngleRange);
	}
}

void ResultDisplay::ArrowUpdate()
{
	m_AroowAlpha += m_AddAroowAlpha * DeltaTime::Instance().GetRealDeltaTime();
	if (m_AroowAlpha > 1)
	{
		m_AroowAlpha = 1;
		m_AddAroowAlpha *= -1;
	}

	if (m_AroowAlpha < 0.3)
	{
		m_AroowAlpha = 0.3;
		m_AddAroowAlpha *= -1;
	}







	POINT mouse = MouseInfo::Instance().m_windowPos;
	{

		//左
		float Left = m_ArrowLPos.x - m_ArrowSiz.x / 2;
		float Roght = m_ArrowLPos.x + m_ArrowSiz.x / 2;
		float top = m_ArrowLPos.y + m_ArrowSiz.y / 2;
		float bot = m_ArrowLPos.y - m_ArrowSiz.y / 2;

		if (mouse.x >= Left && mouse.x <= Roght
			&& mouse.y <= top && mouse.y >= bot)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			m_ArrowLHiFlg = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				const int prevIndex = m_CurrentDisplayIndex;
				m_CurrentDisplayIndex--;

				if (0 >= m_CurrentDisplayIndex)
				{
					m_CurrentDisplayIndex = 0;
				}

				// ページが変わったときだけ、めくる音
				if (prevIndex != m_CurrentDisplayIndex)
				{
					{
						std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/PageTurn.wav", false);
						if (se) { se->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
					}
				}
			}
		}
		else
		{
			m_ArrowLHiFlg = false;
		}
	}
	{

		//migi
		float Left = m_ArrowRPos.x - m_ArrowSiz.x / 2;
		float Roght = m_ArrowRPos.x + m_ArrowSiz.x / 2;
		float top = m_ArrowRPos.y + m_ArrowSiz.y / 2;
		float bot = m_ArrowRPos.y - m_ArrowSiz.y / 2;

		if (mouse.x >= Left && mouse.x <= Roght
			&& mouse.y <= top && mouse.y >= bot)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			m_ArrowRHiFlg = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				const int prevIndex = m_CurrentDisplayIndex;
				m_CurrentDisplayIndex++;

				if (m_ResultDisplayList.size() <= m_CurrentDisplayIndex)
				{
					m_CurrentDisplayIndex = m_ResultDisplayList.size() - 1;
				}

				// ページが変わったときだけ、めくる音
				if (prevIndex != m_CurrentDisplayIndex)
				{
					{
						std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/PageTurn.wav", false);
						if (se) { se->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
					}
				}
			}
		}
		else
		{
			m_ArrowRHiFlg = false;
		}
	}

}

void ResultDisplay::ArrowDrawSprite()
{
	if (m_ArrowTex)
	{


		Math::Color col = { 1,1,1,m_AroowAlpha };

		Math::Vector2 Siz = m_ArrowSiz;
		if (m_ArrowLHiFlg)
		{
			Siz *= m_AroowHitScale;
		}

		if (m_CurrentDisplayIndex != 0)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_ArrowTex, m_ArrowLPos.x, m_ArrowLPos.y, Siz.x, Siz.y, nullptr, &col);
		}


		Siz = m_ArrowSiz;
		if (m_ArrowRHiFlg)
		{
			Siz *= m_AroowHitScale;
		}
		if (m_CurrentDisplayIndex != m_ResultDisplayList.size() - 1)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_ArrowTex, m_ArrowRPos.x, m_ArrowRPos.y, Siz.x, Siz.y, nullptr, &col, { 0.5,0.5 }, 180);
		}
	}

}

void ResultDisplay::LostUpdate()
{
	if (!m_playerDead) { return; }

	const float dt = DeltaTime::Instance().GetRealDeltaTime();

	// 揺れは押した後も残り時間がある間だけ減らす
	if (m_lostShakeTimer > 0.0f)
	{
		m_lostShakeTimer -= dt;
		if (m_lostShakeTimer < 0.0f) { m_lostShakeTimer = 0.0f; }
	}

	// もう押し終わっていたら何もしない
	if (m_lostStampedFlg) { return; }

	// ハンコが出るページ（2ページ目以降）にいる間だけ進める
	if (m_CurrentDisplayIndex <= 0) { return; }

	const float hitTime = kLostDelayTime + kLostFallTime;		// 押した瞬間
	const float endTime = hitTime + kLostBounceTime;			// 演出が終わる時間

	const float prevTime = m_lostTimer;
	m_lostTimer += dt;

	// 押した瞬間（このフレームで hitTime をまたいだ）に揺らし始める
	if (prevTime < hitTime && m_lostTimer >= hitTime)
	{
		m_lostShakeTimer = kLostShakeTime;

		// ハンコを押した音
		{
			std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/Stamp.wav", false);
			if (se) { se->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
		}
	}

	// 跳ね返りまで終わったら押し終わり
	if (m_lostTimer >= endTime)
	{
		m_lostStampedFlg = true;
	}
}

void ResultDisplay::LostDraw()
{
	if (!m_lostTex) { return; }

	float scale = 1.0f;		// 大きさ（倍）
	float alpha = kLostStampAlpha;		// 透明度
	float flash = 0.0f;		// 白く光らせる強さ（0〜1）

	if (!m_lostStampedFlg)
	{
		const float t = m_lostTimer;

		// まだ出す前
		if (t < kLostDelayTime) { return; }

		if (t < kLostDelayTime + kLostFallTime)
		{
			// 押されている途中：大きい → 1倍（だんだん速く）、透明 → 不透明
			float r = (t - kLostDelayTime) / kLostFallTime;
			float ease = r * r;
			scale = kLostStartScale + (1.0f - kLostStartScale) * ease;
			alpha = r * kLostStampAlpha;
		}
		else
		{
			// 押した後：1倍 → 少し膨らむ → 1倍、光はだんだん消える
			float r = (t - kLostDelayTime - kLostFallTime) / kLostBounceTime;
			r = std::clamp(r, 0.0f, 1.0f);
			scale = 1.0f + kLostBounceScale * sinf(DirectX::XM_PI * r);
			flash = 1.0f - r;
		}
	}

	// 揺れ（残り時間が多いほど強い）
	Math::Vector2 shake = {};
	if (m_lostShakeTimer > 0.0f)
	{
		float p = m_lostShakeTimer / kLostShakeTime;		// 1 → 0
		float amp = kLostShakePower * p;
		shake.x = sinf(m_lostShakeTimer * 90.0f) * amp;
		shake.y = cosf(m_lostShakeTimer * 70.0f) * amp;
	}

	const float w = (float)m_lostTex->GetWidth() * scale;
	const float h = (float)m_lostTex->GetHeight() * scale;
	const Math::Vector2 pos = m_lostTexPos + shake;

	// ハンコ本体
	Math::Color col = { 1,0,0,alpha };
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_lostTex, pos.x, pos.y, w, h, nullptr, &col, { 0.5,1 }, m_lostStampAngle);

	// 押した瞬間の光（同じ画像を加算で重ねる）
	if (flash > 0.0f)
	{
		Math::Color flashCol = { 1,1,1,flash * 0.8f };
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_lostTex, pos.x, pos.y, w, h, nullptr, &flashCol, { 0.5,1 }, m_lostStampAngle);
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
	}
}
