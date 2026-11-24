//---- 自分のヘッダー ----
#include "ResultDisplay.h"													//リザルト画面

//---- プレイヤー ----
#include"../../../Character/Player/PlayerInventory/PlayerInventory.h"		//プレイヤーの所持品

//---- UI ----
#include"ResultDisplayBase.h"												//リザルト画面の基底クラス
#include"ResultDisplay_Score/ResultDisplay_Score.h"							//リザルト画面(スコア)
#include"ResultDisplay_PotionInventory/ResultDisplay_PotionInventory.h"		//リザルト画面(ポーション)
#include"ResultDisplay_PlayerStatus/ResultDisplay_PlayerStatus.h"			//リザルト画面(ステータス)
#include"ResultDisplay_WeponInventory/ResultDisplay_WeponInventory.h"		//リザルト画面(武器強化)

//---- 便利機能 ----
#include"../../../../Info/MouseInfo/MouseInfo.h"							//マウス入力
#include"../../../../Info/KeyInfo/KeyInfo.h"								//キー入力
#include"../../../../Info/VOL/VOLInfo.h"									//音量設定

void ResultDisplay::Init()
{

	if (!m_spResultBackTex)
	{
		m_spResultBackTex = std::make_shared<KdTexture>();
		m_spResultBackTex->Load("Asset/Textures/GameUI/Result/ResultBack.png");
	}
	if (!m_spResultBack2Tex)
	{
		m_spResultBack2Tex = std::make_shared<KdTexture>();
		m_spResultBack2Tex->Load("Asset/Textures/GameUI/Result/back.png");
	}

	if (!m_spArrowTex)
	{
		m_spArrowTex = std::make_shared<KdTexture>();
		m_spArrowTex->Load("Asset/Textures/GameUI/Result/Arrow.png");
		m_arrowSize = { (float)m_spArrowTex->GetWidth(),(float)m_spArrowTex->GetHeight() };
		m_arrowLPos = { -400, 0 };
		m_arrowRPos = { 400, 0 };
	}

	m_spResultDisplay_Score = std::make_shared<ResultDisplay_Score>();
	m_spResultDisplay_Score->Init();
	m_resultDisplayList.push_back(m_spResultDisplay_Score);



	m_spResultDisplay_PotionInventory = std::make_shared<ResultDisplay_PotionInventory>();
	m_spResultDisplay_PotionInventory->Init();
	m_resultDisplayList.push_back(m_spResultDisplay_PotionInventory);

	m_spResultDisplay_WeaponInventory = std::make_shared<ResultDisplay_WeponInventory>();
	m_spResultDisplay_WeaponInventory->Init();
	m_resultDisplayList.push_back(m_spResultDisplay_WeaponInventory);

	m_spResultDisplay_PlayerStatus = std::make_shared<ResultDisplay_PlayerStatus>();
	m_spResultDisplay_PlayerStatus->Init();
	m_resultDisplayList.push_back(m_spResultDisplay_PlayerStatus);
}

void ResultDisplay::Update()
{

	ArrowUpdate();

	LostUpdate();

	if (m_currentDisplayIndex < 0 || m_currentDisplayIndex >= m_resultDisplayList.size()) { return; }
	m_resultDisplayList[m_currentDisplayIndex]->Update();
}

void ResultDisplay::PreDraw()
{
	if (m_currentDisplayIndex < 0 || m_currentDisplayIndex >= m_resultDisplayList.size()) { return; }
	m_resultDisplayList[m_currentDisplayIndex]->PreDraw();
}

void ResultDisplay::DrawSprite()
{
	if (m_spResultBackTex)
	{
		Math::Color col = { 3,3,3,1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spResultBackTex, 0, 0, nullptr, &col);
	}

	if (m_spResultBack2Tex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spResultBack2Tex, 0, 0, nullptr);
	}



	ArrowDrawSprite();

	if (m_currentDisplayIndex < 0 || m_currentDisplayIndex >= m_resultDisplayList.size()) { return; }
	m_resultDisplayList[m_currentDisplayIndex]->DrawSprite();


	if (m_currentDisplayIndex > 0)
	{
		if (m_playerDead)
		{
			LostDraw();
		}
	}

}

void ResultDisplay::SetPath(std::string _path)
{

}

void ResultDisplay::SetDisplayFloorInfo(const DisplayFloorInfo& _displayFloorInfo)
{
	m_spResultDisplay_Score->SetDisplayFloorInfo(_displayFloorInfo);
}

void ResultDisplay::SetPlayerInventoryList(std::vector<std::vector<Inventory>> _playerInventoryList)
{
	m_playerInventoryList = _playerInventoryList;


	if (m_spResultDisplay_PotionInventory)
	{
		m_spResultDisplay_PotionInventory->AddPotionTexInfo(_playerInventoryList[0]);
	}
	if (m_spResultDisplay_WeaponInventory)
	{
		m_spResultDisplay_WeaponInventory->WeaponsStrengtheningInfoInit(_playerInventoryList[1]);
	}

}

void ResultDisplay::SetPlayerStatusGameResultFilePath(std::string _path)
{
	if (m_spResultDisplay_PlayerStatus)
	{
		m_spResultDisplay_PlayerStatus->SetPlayerStatusGameResultFilePath(_path);
	}
}

void ResultDisplay::SetSpWeaponBase(std::shared_ptr<WeaponBase> _spWeaponBase)
{
	if (m_spResultDisplay_WeaponInventory)
	{
		m_spResultDisplay_WeaponInventory->SetSpWeaponBase(_spWeaponBase);
	}
	if (m_spResultDisplay_PlayerStatus)
	{
		m_spResultDisplay_PlayerStatus->SetSpWeaponBase(_spWeaponBase);
	}
}

void ResultDisplay::SetPlayerDead(bool _flg)
{
	m_playerDead = _flg;

	if (m_playerDead)
	{
		if (!m_spLostTex)
		{
			m_spLostTex = std::make_shared<KdTexture>();
			m_spLostTex->Load("Asset/Textures/GameUI/Result/Lost.png");

			m_lostTexPos = { 390, 270 };

			m_lostTexSize.x = m_spLostTex->GetWidth();
			m_lostTexSize.y = m_spLostTex->GetHeight();
		}

		m_lostStampAngle = kLostStampBaseAngle + KdRandom::GetFloat(-kLostStampAngleRange, kLostStampAngleRange);
	}
}

void ResultDisplay::ArrowUpdate()
{
	m_arrowAlpha += m_addArrowAlpha * DeltaTime::Instance().GetRealDeltaTime();
	if (m_arrowAlpha > 1)
	{
		m_arrowAlpha = 1;
		m_addArrowAlpha *= -1;
	}

	if (m_arrowAlpha < 0.3)
	{
		m_arrowAlpha = 0.3;
		m_addArrowAlpha *= -1;
	}







	POINT mouse = MouseInfo::Instance().m_windowPos;
	{

		//左
		float left = m_arrowLPos.x - m_arrowSize.x / 2;
		float right = m_arrowLPos.x + m_arrowSize.x / 2;
		float top = m_arrowLPos.y + m_arrowSize.y / 2;
		float bot = m_arrowLPos.y - m_arrowSize.y / 2;

		if (mouse.x >= left && mouse.x <= right
			&& mouse.y <= top && mouse.y >= bot)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			m_arrowLHitFlg = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				const int prevIndex = m_currentDisplayIndex;
				m_currentDisplayIndex--;

				if (0 >= m_currentDisplayIndex)
				{
					m_currentDisplayIndex = 0;
				}

				// ページが変わったときだけ、めくる音
				if (prevIndex != m_currentDisplayIndex)
				{
					{
						std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/PageTurn.wav", false);
						if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
					}
				}
			}
		}
		else
		{
			m_arrowLHitFlg = false;
		}
	}
	{

		//migi
		float left = m_arrowRPos.x - m_arrowSize.x / 2;
		float right = m_arrowRPos.x + m_arrowSize.x / 2;
		float top = m_arrowRPos.y + m_arrowSize.y / 2;
		float bot = m_arrowRPos.y - m_arrowSize.y / 2;

		if (mouse.x >= left && mouse.x <= right
			&& mouse.y <= top && mouse.y >= bot)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			m_arrowRHitFlg = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				const int prevIndex = m_currentDisplayIndex;
				m_currentDisplayIndex++;

				if (m_resultDisplayList.size() <= m_currentDisplayIndex)
				{
					m_currentDisplayIndex = m_resultDisplayList.size() - 1;
				}

				// ページが変わったときだけ、めくる音
				if (prevIndex != m_currentDisplayIndex)
				{
					{
						std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/PageTurn.wav", false);
						if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
					}
				}
			}
		}
		else
		{
			m_arrowRHitFlg = false;
		}
	}

}

void ResultDisplay::ArrowDrawSprite()
{
	if (m_spArrowTex)
	{


		Math::Color col = { 1,1,1,m_arrowAlpha };

		Math::Vector2 size = m_arrowSize;
		if (m_arrowLHitFlg)
		{
			size *= m_arrowHitScale;
		}

		if (m_currentDisplayIndex != 0)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spArrowTex, m_arrowLPos.x, m_arrowLPos.y, size.x, size.y, nullptr, &col);
		}


		size = m_arrowSize;
		if (m_arrowRHitFlg)
		{
			size *= m_arrowHitScale;
		}
		if (m_currentDisplayIndex != m_resultDisplayList.size() - 1)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spArrowTex, m_arrowRPos.x, m_arrowRPos.y, size.x, size.y, nullptr, &col, { 0.5,0.5 }, 180);
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
	if (m_currentDisplayIndex <= 0) { return; }

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
			std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/Stamp.wav", false);
			if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
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
	if (!m_spLostTex) { return; }

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

	const float w = (float)m_spLostTex->GetWidth() * scale;
	const float h = (float)m_spLostTex->GetHeight() * scale;
	const Math::Vector2 pos = m_lostTexPos + shake;

	// ハンコ本体
	Math::Color col = { 1,0,0,alpha };
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spLostTex, pos.x, pos.y, w, h, nullptr, &col, { 0.5,1 }, m_lostStampAngle);

	// 押した瞬間の光（同じ画像を加算で重ねる）
	if (flash > 0.0f)
	{
		Math::Color flashCol = { 1,1,1,flash * 0.8f };
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spLostTex, pos.x, pos.y, w, h, nullptr, &flashCol, { 0.5,1 }, m_lostStampAngle);
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
	}
}
