#include "PauseUI.h"
#include"../MouseInfo/MouseInfo.h"
#include"../KeyInfo/KeyInfo.h"

void PauseUI::Init()
{
	TexLoad();
	SetTexPos();
}

void PauseUI::Update()
{
	if (GetAsyncKeyState(VK_ESCAPE)&0x8000)
	{
		if (!m_escFlg)
		{
			m_pauseMenuFlg = !m_pauseMenuFlg;
			m_escFlg = true;

			if (m_pauseMenuFlg)
			{
				m_pausePage = PausePage::Main;
				MouseInfo::Instance().SetMouseFreeFlg(true);
			}
			else
			{
				MouseInfo::Instance().SetMouseFreeFlg(false);
			}
		}
	}
	else
	{
		m_escFlg = false;
	}


	if (!m_pauseMenuFlg) { return; }

	switch (m_pausePage)
	{
	case PausePage::Main:
		MainMenuUpdate();
		break;
	default:
		break;
	}

}

void PauseUI::DrawSprite()
{
	if (!m_pauseMenuFlg) { return; }
	
	//メインメニュー
	MainMenuDraw();
}

void PauseUI::MainMenuDraw()
{
	//パネル
	if (m_spMainPanelTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spMainPanelTex, m_mainPanelPos.x, m_mainPanelPos.y);
	}

	//「一時停止」
	if (m_spPauseTitleTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPauseTitleTex, m_pauseTitlePos.x, m_pauseTitlePos.y);
	}

	//「ゲームに戻る」
	std::shared_ptr<KdTexture>spResumeButtonTex;

	if (m_resumeButtonHoverFlg)
	{
		spResumeButtonTex = m_spResumeButtonHoverTex;
	}
	else
	{
		spResumeButtonTex = m_spResumeButtonTex;
	}


	if (spResumeButtonTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spResumeButtonTex, m_resumeButtonPos.x, m_resumeButtonPos.y);
	}

	//「音量設定」
	std::shared_ptr<KdTexture>spVolumeButtonTex;

	if (m_volumeButtonHoverFlg)
	{
		spVolumeButtonTex = m_spVolumeButtonHoverTex;
	}
	else
	{
		spVolumeButtonTex = m_spVolumeButtonTex;
	}


	if (spVolumeButtonTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spVolumeButtonTex, m_volumeButtonPos.x, m_volumeButtonPos.y);
	}

	//「タイトルへ」
	std::shared_ptr<KdTexture>spTitleButtonTex;

	if (m_titleButtonHoverFlg)
	{
		spTitleButtonTex = m_spTitleButtonHoverTex;
	}
	else
	{
		spTitleButtonTex = m_spTitleButtonTex;
	}


	if (spTitleButtonTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spTitleButtonTex, m_titleButtonPos.x, m_titleButtonPos.y);
	}

	//「ゲーム終了」
	std::shared_ptr<KdTexture>spExitButtonTex;

	if (m_exitButtonHoverFlg)
	{
		spExitButtonTex = m_spExitButtonHoverTex;
	}
	else
	{
		spExitButtonTex = m_spExitButtonTex;
	}


	if (spExitButtonTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spExitButtonTex, m_exitButtonPos.x, m_exitButtonPos.y);
	}

	//「ESCで閉じる」
	if (m_spEscCloseTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spEscCloseTex, m_escClosePos.x, m_escClosePos.y);
	}
}

void PauseUI::MainMenuUpdate()
{
	//マウスが乗っているか
	m_resumeButtonHoverFlg = IsMouseOnTex(m_spResumeButtonTex, m_resumeButtonPos);
	m_volumeButtonHoverFlg = IsMouseOnTex(m_spVolumeButtonTex, m_volumeButtonPos);
	m_titleButtonHoverFlg = IsMouseOnTex(m_spTitleButtonTex, m_titleButtonPos);
	m_exitButtonHoverFlg = IsMouseOnTex(m_spExitButtonTex, m_exitButtonPos);

	//どれかに乗っていたらカーソルを変える
	if (m_resumeButtonHoverFlg || m_volumeButtonHoverFlg || m_titleButtonHoverFlg || m_exitButtonHoverFlg)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType_HIT);
	}

	//クリック
	if (!KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true)) { return; }

	//「ゲームに戻る」
	if (m_resumeButtonHoverFlg)
	{
		PauseClose();
		return;
	}

	//「音量設定」
	if (m_volumeButtonHoverFlg)
	{
		m_pausePage = PausePage::Volume;
		return;
	}

	//「タイトルへ」
	if (m_titleButtonHoverFlg)
	{
		m_pausePage = PausePage::ConfirmTitle;
		return;
	}

	//「ゲーム終了」
	if (m_exitButtonHoverFlg)
	{
		m_pausePage = PausePage::ConfirmExit;
		return;
	}
}

bool PauseUI::IsMouseOnTex(const std::shared_ptr<KdTexture>&_spTex, const Math::Vector2 & _pos)
{
	if (!_spTex) { return false; }

	POINT mouse = MouseInfo::Instance().m_windowPos;

	float left = _pos.x - _spTex->GetWidth() / 2.0f;
	float right = _pos.x + _spTex->GetWidth() / 2.0f;
	float top = _pos.y + _spTex->GetHeight() / 2.0f;
	float bot = _pos.y - _spTex->GetHeight() / 2.0f;

	return (left <= mouse.x && right >= mouse.x &&
		bot <= mouse.y && top >= mouse.y);
}

void PauseUI::PauseClose()
{}

void PauseUI::TexLoad()
{
	//---- パネル ----
	if (!m_spMainPanelTex)
	{
		m_spMainPanelTex = std::make_shared<KdTexture>();
		m_spMainPanelTex->Load("Asset/Textures/GameUI/Pause/Panel/PauseMenuPanel.png");
	}

	if (!m_spVolumePanelTex)
	{
		m_spVolumePanelTex = std::make_shared<KdTexture>();
		m_spVolumePanelTex->Load("Asset/Textures/GameUI/Pause/Panel/VolumePanel.png");
	}

	if (!m_spConfirmPanelTex)
	{
		m_spConfirmPanelTex = std::make_shared<KdTexture>();
		m_spConfirmPanelTex->Load("Asset/Textures/GameUI/Pause/Panel/ConfirmPanel.png");
	}

	//---- ボタン ----
	if (!m_spResumeButtonTex)
	{
		m_spResumeButtonTex = std::make_shared<KdTexture>();
		m_spResumeButtonTex->Load("Asset/Textures/GameUI/Pause/Button/Resume.png");
	}
	if (!m_spResumeButtonHoverTex)
	{
		m_spResumeButtonHoverTex = std::make_shared<KdTexture>();
		m_spResumeButtonHoverTex->Load("Asset/Textures/GameUI/Pause/Button/Resume_Hover.png");
	}

	if (!m_spVolumeButtonTex)
	{
		m_spVolumeButtonTex = std::make_shared<KdTexture>();
		m_spVolumeButtonTex->Load("Asset/Textures/GameUI/Pause/Button/Volume.png");
	}
	if (!m_spVolumeButtonHoverTex)
	{
		m_spVolumeButtonHoverTex = std::make_shared<KdTexture>();
		m_spVolumeButtonHoverTex->Load("Asset/Textures/GameUI/Pause/Button/Volume_Hover.png");
	}

	if (!m_spTitleButtonTex)
	{
		m_spTitleButtonTex = std::make_shared<KdTexture>();
		m_spTitleButtonTex->Load("Asset/Textures/GameUI/Pause/Button/Title.png");
	}
	if (!m_spTitleButtonHoverTex)
	{
		m_spTitleButtonHoverTex = std::make_shared<KdTexture>();
		m_spTitleButtonHoverTex->Load("Asset/Textures/GameUI/Pause/Button/Title_Hover.png");
	}

	if (!m_spExitButtonTex)
	{
		m_spExitButtonTex = std::make_shared<KdTexture>();
		m_spExitButtonTex->Load("Asset/Textures/GameUI/Pause/Button/Exit.png");
	}
	if (!m_spExitButtonHoverTex)
	{
		m_spExitButtonHoverTex = std::make_shared<KdTexture>();
		m_spExitButtonHoverTex->Load("Asset/Textures/GameUI/Pause/Button/Exit_Hover.png");
	}

	if (!m_spBackButtonTex)
	{
		m_spBackButtonTex = std::make_shared<KdTexture>();
		m_spBackButtonTex->Load("Asset/Textures/GameUI/Pause/Button/Back.png");
	}
	if (!m_spBackButtonHoverTex)
	{
		m_spBackButtonHoverTex = std::make_shared<KdTexture>();
		m_spBackButtonHoverTex->Load("Asset/Textures/GameUI/Pause/Button/Back_Hover.png");
	}

	if (!m_spYesButtonTex)
	{
		m_spYesButtonTex = std::make_shared<KdTexture>();
		m_spYesButtonTex->Load("Asset/Textures/GameUI/Pause/Button/Yes.png");
	}
	if (!m_spYesButtonHoverTex)
	{
		m_spYesButtonHoverTex = std::make_shared<KdTexture>();
		m_spYesButtonHoverTex->Load("Asset/Textures/GameUI/Pause/Button/Yes_Hover.png");
	}

	if (!m_spNoButtonTex)
	{
		m_spNoButtonTex = std::make_shared<KdTexture>();
		m_spNoButtonTex->Load("Asset/Textures/GameUI/Pause/Button/No.png");
	}
	if (!m_spNoButtonHoverTex)
	{
		m_spNoButtonHoverTex = std::make_shared<KdTexture>();
		m_spNoButtonHoverTex->Load("Asset/Textures/GameUI/Pause/Button/No_Hover.png");
	}

	//---- 文字 ----
	if (!m_spPauseTitleTex)
	{
		m_spPauseTitleTex = std::make_shared<KdTexture>();
		m_spPauseTitleTex->Load("Asset/Textures/GameUI/Pause/Text/PauseTitle.png");
	}

	if (!m_spVolumeTitleTex)
	{
		m_spVolumeTitleTex = std::make_shared<KdTexture>();
		m_spVolumeTitleTex->Load("Asset/Textures/GameUI/Pause/Text/VolumeTitle.png");
	}

	if (!m_spEscCloseTex)
	{
		m_spEscCloseTex = std::make_shared<KdTexture>();
		m_spEscCloseTex->Load("Asset/Textures/GameUI/Pause/Text/EscClose.png");
	}

	if (!m_spConfirmTitleTitleTex)
	{
		m_spConfirmTitleTitleTex = std::make_shared<KdTexture>();
		m_spConfirmTitleTitleTex->Load("Asset/Textures/GameUI/Pause/Text/ConfirmTitle_Title.png");
	}

	if (!m_spConfirmTitleExitTex)
	{
		m_spConfirmTitleExitTex = std::make_shared<KdTexture>();
		m_spConfirmTitleExitTex->Load("Asset/Textures/GameUI/Pause/Text/ConfirmTitle_Exit.png");
	}

	if (!m_spConfirmMsgTitleTex)
	{
		m_spConfirmMsgTitleTex = std::make_shared<KdTexture>();
		m_spConfirmMsgTitleTex->Load("Asset/Textures/GameUI/Pause/Text/ConfirmMsg_Title.png");
	}

	if (!m_spConfirmMsgExitTex)
	{
		m_spConfirmMsgExitTex = std::make_shared<KdTexture>();
		m_spConfirmMsgExitTex->Load("Asset/Textures/GameUI/Pause/Text/ConfirmMsg_Exit.png");
	}

	if (!m_spMasterTex)
	{
		m_spMasterTex = std::make_shared<KdTexture>();
		m_spMasterTex->Load("Asset/Textures/GameUI/Pause/Text/Master.png");
	}

	if (!m_spBGMTex)
	{
		m_spBGMTex = std::make_shared<KdTexture>();
		m_spBGMTex->Load("Asset/Textures/GameUI/Pause/Text/BGM.png");
	}

	if (!m_spSETex)
	{
		m_spSETex = std::make_shared<KdTexture>();
		m_spSETex->Load("Asset/Textures/GameUI/Pause/Text/SE.png");
	}

	if (!m_spPercentTex)
	{
		m_spPercentTex = std::make_shared<KdTexture>();
		m_spPercentTex->Load("Asset/Textures/GameUI/Pause/Text/Percent.png");
	}

	//---- スライダー ----
	if (!m_spSliderTrackTex)
	{
		m_spSliderTrackTex = std::make_shared<KdTexture>();
		m_spSliderTrackTex->Load("Asset/Textures/GameUI/Pause/Slider/SliderTrack.png");
	}

	if (!m_spSliderFillTex)
	{
		m_spSliderFillTex = std::make_shared<KdTexture>();
		m_spSliderFillTex->Load("Asset/Textures/GameUI/Pause/Slider/SliderFill.png");
	}

	if (!m_spSliderThumbTex)
	{
		m_spSliderThumbTex = std::make_shared<KdTexture>();
		m_spSliderThumbTex->Load("Asset/Textures/GameUI/Pause/Slider/SliderThumb.png");
	}

	if (!m_spSliderThumbHoverTex)
	{
		m_spSliderThumbHoverTex = std::make_shared<KdTexture>();
		m_spSliderThumbHoverTex->Load("Asset/Textures/GameUI/Pause/Slider/SliderThumb_Hover.png");
	}
}

void PauseUI::SetTexPos()
{
	//---- パネル ----
	m_mainPanelPos = { 0, 0 };
	m_volumePanelPos = { 0, 0 };
	m_confirmPanelPos = { 0, 0 };

	//---- メインメニュー ----
	m_pauseTitlePos = { 0, 165 };
	m_resumeButtonPos = { 0, 85 };
	m_volumeButtonPos = { 0, 15 };
	m_titleButtonPos = { 0, -55 };
	m_exitButtonPos = { 0, -125 };
	m_escClosePos = { 0, -250 };

	//---- 音量設定 ----
	m_volumeTitlePos = { 0, 120 };

	m_masterPos = { -270, 55 };
	m_BGMPos = { -270, -5 };
	m_SEPos = { -270, -65 };

	m_masterSliderPos = { 10, 55 };
	m_BGMSliderPos = { 10, -5 };
	m_SESliderPos = { 10, -65 };

	m_masterPercentPos = { 248, 55 };
	m_BGMPercentPos = { 248, -5 };
	m_SEPercentPos = { 248, -65 };

	m_backButtonPos = { 0, -130 };
	m_volumeEscClosePos = { 0, -205 };

	//---- 確認ダイアログ ----
	m_confirmTitlePos = { 0, 62 };
	m_confirmMsgPos = { 0, 10 };
	m_noButtonPos = { -110, -65 };
	m_yesButtonPos = { 110, -65 };
}
