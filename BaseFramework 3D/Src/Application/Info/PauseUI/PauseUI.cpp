#include "PauseUI.h"
#include"../MouseInfo/MouseInfo.h"
#include"../KeyInfo/KeyInfo.h"
#include"../NumDraw/NumDraw.h"
#include"../FadeDraw/FadeDraw.h"

#include"../../main.h"
#include"../../Scene/SceneManager.h"
void PauseUI::Init()
{
	TexLoad();
	SetTexPos();
}

void PauseUI::Update()
{
	switch (m_pausePage)
	{
	case PauseUI::PausePage::Main:
		MainMenuUpdate();
		break;
	case PauseUI::PausePage::Volume:
		VolumePageUpdate();
		break;
	case PauseUI::PausePage::ConfirmTitle:
	case PauseUI::PausePage::ConfirmExit:
		ConfirmUpdate();
		break;
	default:
		break;
	}
}

void PauseUI::DrawSprite()
{
	if (!m_pauseMenuFlg) { return; }
	
	//後ろを暗くする
	if (m_spBlackTex)
	{
		Math::Color color = { 0, 0, 0, 0.6f };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spBlackTex, 0, 0, 1280, 720, nullptr, &color);
	}

	switch (m_pausePage)
	{
	case PauseUI::PausePage::Main:
		MainMenuDraw();
		break;
	case PauseUI::PausePage::Volume:
		VolumePageDraw();
		break;
	case PauseUI::PausePage::ConfirmTitle:
	case PauseUI::PausePage::ConfirmExit:
		ConfirmDraw();
		break;
	default:
		break;
	}
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

	//「ESCで戻る」
	if (m_spEscCloseTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spEscCloseTex, m_escClosePos.x, m_escClosePos.y);
	}
}

void PauseUI::VolumePageUpdate()
{
	SliderDragUpdate();
	BackButtonUpdate();

	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		m_escFlg = true;
		m_backButtonHoverFlg = false;
		m_dragIndex = DragSlider::DragSlider_None;
		m_pausePage = PausePage::Main;
	}
}

void PauseUI::SliderDragUpdate()
{
	//つかんでないならどれをつかんでるか見る
	if (m_dragIndex == DragSlider::DragSlider_None)
	{
		m_thumbHoverFlg[(int)(DragSlider::DragSlider_Master)] = IsMouseOnSlider(m_masterSliderPos);
		m_thumbHoverFlg[(int)(DragSlider::DragSlider_BGM)] = IsMouseOnSlider(m_BGMSliderPos);
		m_thumbHoverFlg[(int)(DragSlider::DragSlider_SE)] = IsMouseOnSlider(m_SESliderPos);

		for (int i = 0; i < (int)(DragSlider::DragSlider_Siz); i++)
		{
			if (m_thumbHoverFlg[i])
			{
				MouseInfo::Instance().SetMouseType(MouseInfo::MouseType_HIT);

				//クリック
				if (!KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true)) { continue; }
				m_dragIndex = (DragSlider)(i);
				SetVolume((DragSlider)i, MouseXToVol(GetSliderPos((DragSlider)(i))));
				break;
			}
		}
	}
	else
	{
		//左クリックされてなかったら選ばれてない状態に
		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) 
		{
			m_dragIndex = DragSlider::DragSlider_None; 
			return;
		}
		m_thumbHoverFlg[m_dragIndex] = true;
		SetVolume(m_dragIndex, MouseXToVol(GetSliderPos(m_dragIndex)));
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType_Drag);
	}

}

bool PauseUI::IsMouseOnSlider(const Math::Vector2& _sliderPos)
{
	return IsMouseOnTex(m_spSliderTrackTex, _sliderPos);
}

bool PauseUI::IsMouseOnThumb(const Math::Vector2& _sliderPos, float _vol)
{
	return false;
}

float PauseUI::MouseXToVol(const Math::Vector2& _sliderPos)
{
	if (!m_spSliderTrackTex) { return 0.0f; }

	POINT mouse = MouseInfo::Instance().m_windowPos;

	//バーの幅と左端
	float width = (float)m_spSliderTrackTex->GetWidth();
	float left = _sliderPos.x - width / 2.0f;

	//左端からどれだけ右にあるか(0〜1)
	float vol = (mouse.x - left) / width;

	//バーの外に出ても0〜1におさめる
	return std::clamp(vol, 0.0f, 1.0f);
}

void PauseUI::SetVolume(int _index, float _vol)
{
	switch (_index)
	{
	case DragSlider_Master:
		VOLInfo::Instance().SetMSVol(_vol);
		break;
	case DragSlider_BGM:
		VOLInfo::Instance().SetBGMVol(_vol);
		break;
	case DragSlider_SE:
		VOLInfo::Instance().SetSEVol(_vol);
		break;
	default:
		break;
	}
}

float PauseUI::GetVolume(int _index)
{
	return 0.0f;
}

Math::Vector2 PauseUI::GetSliderPos(DragSlider _slider)
{
	switch (_slider)
	{
	case DragSlider_Master:	return m_masterSliderPos;
	case DragSlider_BGM:	return m_BGMSliderPos;
	case DragSlider_SE:		return m_SESliderPos;
	default:				return { 0, 0 };
	}
}

void PauseUI::VolumePageDraw()
{
	//パネル
	if (m_spVolumePanelTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spVolumePanelTex, m_volumePanelPos.x, m_volumePanelPos.y);
	}

	//「音量設定」
	if (m_spVolumeTitleTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spVolumeTitleTex, m_volumeTitlePos.x, m_volumeTitlePos.y);
	}

	//今の音量(0〜1)
	float masterVol = VOLInfo::Instance().GetMSVol();
	float BGMVol = VOLInfo::Instance().GetBGMVol();
	float SEVol = VOLInfo::Instance().GetSEVol();

	//---- 全体 ----

	//「全体」(左端そろえ)
	if (m_spMasterTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spMasterTex, m_masterPos.x, m_masterPos.y, nullptr, &kWhiteColor, { 0.0f, 0.5f });
	}

	//バーの枠
	if (m_spSliderTrackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spSliderTrackTex, m_masterSliderPos.x, m_masterSliderPos.y);
	}

	//バーの中身(音量の分だけ左から切り取る)
	if (m_spSliderFillTex)
	{
		int fillW = (int)(m_spSliderFillTex->GetWidth() * masterVol);
		int fillH = m_spSliderFillTex->GetHeight();
		float left = m_masterSliderPos.x - m_spSliderFillTex->GetWidth() / 2.0f;
		Math::Rectangle rect = { 0, 0, fillW, fillH };

		if (fillW > 0)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spSliderFillTex, left, m_masterSliderPos.y, fillW, fillH, &rect, &kWhiteColor, { 0.0f, 0.5f });
		}
	}

	//つまみ
	std::shared_ptr<KdTexture>spMasterThumbTex;

	if (m_thumbHoverFlg[DragSlider_Master])
	{
		spMasterThumbTex = m_spSliderThumbHoverTex;
	}
	else
	{
		spMasterThumbTex = m_spSliderThumbTex;
	}


	if (spMasterThumbTex && m_spSliderTrackTex)
	{
		float thumbX = m_masterSliderPos.x - m_spSliderTrackTex->GetWidth() / 2.0f + m_spSliderTrackTex->GetWidth() * masterVol;
		KdShaderManager::Instance().m_spriteShader.DrawTex(spMasterThumbTex, thumbX, m_masterSliderPos.y);
	}

	//数字(右端そろえ)
	NumDraw::GetInstance().Draw((long)std::lround(masterVol * 100.0f), RAligned, { m_masterPercentPos.x - 3.0f, m_masterPercentPos.y }, { 1,1,1,1 }, 2.0f);

	//「%」(左端そろえ)
	if (m_spPercentTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPercentTex, m_masterPercentPos.x, m_masterPercentPos.y, nullptr, &kWhiteColor, { 0.0f, 0.5f });
	}

	//---- BGM ----

	//「BGM」(左端そろえ)
	if (m_spBGMTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spBGMTex, m_BGMPos.x, m_BGMPos.y, nullptr, &kWhiteColor, { 0.0f, 0.5f });
	}

	//バーの枠
	if (m_spSliderTrackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spSliderTrackTex, m_BGMSliderPos.x, m_BGMSliderPos.y);
	}

	//バーの中身(音量の分だけ左から切り取る)
	if (m_spSliderFillTex)
	{
		int fillW = (int)(m_spSliderFillTex->GetWidth() * BGMVol);
		int fillH = m_spSliderFillTex->GetHeight();
		float left = m_BGMSliderPos.x - m_spSliderFillTex->GetWidth() / 2.0f;
		Math::Rectangle rect = { 0, 0, fillW, fillH };

		if (fillW > 0)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spSliderFillTex, left, m_BGMSliderPos.y, fillW, fillH, &rect, &kWhiteColor, { 0.0f, 0.5f });
		}
	}

	//つまみ
	std::shared_ptr<KdTexture>spBGMThumbTex;

	if (m_thumbHoverFlg[DragSlider_BGM])
	{
		spBGMThumbTex = m_spSliderThumbHoverTex;
	}
	else
	{
		spBGMThumbTex = m_spSliderThumbTex;
	}


	if (spBGMThumbTex && m_spSliderTrackTex)
	{
		float thumbX = m_BGMSliderPos.x - m_spSliderTrackTex->GetWidth() / 2.0f + m_spSliderTrackTex->GetWidth() * BGMVol;
		KdShaderManager::Instance().m_spriteShader.DrawTex(spBGMThumbTex, thumbX, m_BGMSliderPos.y);
	}

	//数字(右端そろえ)
	NumDraw::GetInstance().Draw((long)std::lround(BGMVol * 100.0f), RAligned, { m_BGMPercentPos.x - 3.0f, m_BGMPercentPos.y }, { 1,1,1,1 }, 2.0f);

	//「%」(左端そろえ)
	if (m_spPercentTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPercentTex, m_BGMPercentPos.x, m_BGMPercentPos.y, nullptr, &kWhiteColor, { 0.0f, 0.5f });
	}

	//---- 効果音 ----

	//「効果音」(左端そろえ)
	if (m_spSETex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spSETex, m_SEPos.x, m_SEPos.y, nullptr, &kWhiteColor, { 0.0f, 0.5f });
	}

	//バーの枠
	if (m_spSliderTrackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spSliderTrackTex, m_SESliderPos.x, m_SESliderPos.y);
	}

	//バーの中身(音量の分だけ左から切り取る)
	if (m_spSliderFillTex)
	{
		int fillW = (int)(m_spSliderFillTex->GetWidth() * SEVol);
		int fillH = m_spSliderFillTex->GetHeight();
		float left = m_SESliderPos.x - m_spSliderFillTex->GetWidth() / 2.0f;
		Math::Rectangle rect = { 0, 0, fillW, fillH };

		if (fillW > 0)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spSliderFillTex, left, m_SESliderPos.y, fillW, fillH, &rect, &kWhiteColor, { 0.0f, 0.5f });
		}
	}

	//つまみ
	std::shared_ptr<KdTexture>spSEThumbTex;

	if (m_thumbHoverFlg[DragSlider_SE])
	{
		spSEThumbTex = m_spSliderThumbHoverTex;
	}
	else
	{
		spSEThumbTex = m_spSliderThumbTex;
	}


	if (spSEThumbTex && m_spSliderTrackTex)
	{
		float thumbX = m_SESliderPos.x - m_spSliderTrackTex->GetWidth() / 2.0f + m_spSliderTrackTex->GetWidth() * SEVol;
		KdShaderManager::Instance().m_spriteShader.DrawTex(spSEThumbTex, thumbX, m_SESliderPos.y);
	}
	//数字(右端そろえ)
	NumDraw::GetInstance().Draw((long)std::lround(SEVol * 100.0f), RAligned, { m_SEPercentPos.x - 3.0f, m_SEPercentPos.y }, { 1,1,1,1 }, 2.0f);

	//「%」(左端そろえ)
	if (m_spPercentTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPercentTex, m_SEPercentPos.x, m_SEPercentPos.y, nullptr, &kWhiteColor, { 0.0f, 0.5f });
	}

	//---- 下 ----

	//「戻る」
	std::shared_ptr<KdTexture>spBackButtonTex;

	if (m_backButtonHoverFlg)
	{
		spBackButtonTex = m_spBackButtonHoverTex;
	}
	else
	{
		spBackButtonTex = m_spBackButtonTex;
	}


	if (spBackButtonTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spBackButtonTex, m_backButtonPos.x, m_backButtonPos.y);
	}

	//「ESCで閉じる」
	if (m_spEscBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spEscBackTex, m_volumeEscClosePos.x, m_volumeEscClosePos.y);
	}
}

void PauseUI::BackButtonUpdate()
{
	//ドラッグ中は「戻る」を見ない
	if (m_dragIndex != DragSlider::DragSlider_None)
	{
		m_backButtonHoverFlg = false;
		return;
	}

	//「戻る」にマウスが乗っているか
	m_backButtonHoverFlg = IsMouseOnTex(m_spBackButtonTex, m_backButtonPos);

	if (!m_backButtonHoverFlg) { return; }

	MouseInfo::Instance().SetMouseType(MouseInfo::MouseType_HIT);

	//クリック
	if (!KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true)) { return; }

	m_backButtonHoverFlg = false;
	m_pausePage = PausePage::Main;
}

void PauseUI::MainMenuUpdate()
{

	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		if (!m_escFlg)
		{
			//閉じているときは、今開いていいかを確かめてから開く
			if (m_pauseMenuFlg || SceneManager::Instance().CanOpenPause())
			{
				PauseClose();
			}

			m_escFlg = true;
		}
	}
	else
	{
		m_escFlg = false;
	}


	if (!m_pauseMenuFlg) { return; }

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

void PauseUI::ConfirmDraw()
{
	//パネル
	if (m_spConfirmPanelTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spConfirmPanelTex, m_confirmPanelPos.x, m_confirmPanelPos.y);
	}

	//質問と説明(タイトルへ・ゲーム終了で切り替え)
	std::shared_ptr<KdTexture>spConfirmTitleTex;
	std::shared_ptr<KdTexture>spConfirmMsgTex;

	if (m_pausePage == PausePage::ConfirmTitle)
	{
		spConfirmTitleTex = m_spConfirmTitleTitleTex;
		spConfirmMsgTex = m_spConfirmMsgTitleTex;
	}
	else
	{
		spConfirmTitleTex = m_spConfirmTitleExitTex;
		spConfirmMsgTex = m_spConfirmMsgExitTex;
	}

	//「タイトルへ戻りますか？」「ゲームを終了しますか？」
	if (spConfirmTitleTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spConfirmTitleTex, m_confirmTitlePos.x, m_confirmTitlePos.y);
	}

	//説明
	if (spConfirmMsgTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spConfirmMsgTex, m_confirmMsgPos.x, m_confirmMsgPos.y);
	}

	//「いいえ」
	std::shared_ptr<KdTexture>spNoButtonTex;

	if (m_noButtonHoverFlg)
	{
		spNoButtonTex = m_spNoButtonHoverTex;
	}
	else
	{
		spNoButtonTex = m_spNoButtonTex;
	}


	if (spNoButtonTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spNoButtonTex, m_noButtonPos.x, m_noButtonPos.y);
	}

	//「はい」
	std::shared_ptr<KdTexture>spYesButtonTex;

	if (m_yesButtonHoverFlg)
	{
		spYesButtonTex = m_spYesButtonHoverTex;
	}
	else
	{
		spYesButtonTex = m_spYesButtonTex;
	}


	if (spYesButtonTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spYesButtonTex, m_yesButtonPos.x, m_yesButtonPos.y);
	}


	//「ESCで戻る」
	if (m_spEscCloseTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spEscCloseTex, m_escClosePos.x, m_escClosePos.y);
	}
}

void PauseUI::ConfirmUpdate()
{
	//ESCでメインメニューに戻る(「いいえ」と同じ)
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		m_escFlg = true;
		m_yesButtonHoverFlg = false;
		m_noButtonHoverFlg = false;
		m_pausePage = PausePage::Main;
		return;
	}


	//マウスが乗っているか
	m_yesButtonHoverFlg = IsMouseOnTex(m_spYesButtonTex, m_yesButtonPos);
	m_noButtonHoverFlg = IsMouseOnTex(m_spNoButtonTex, m_noButtonPos);

	//どれかに乗っていたらカーソルを変える
	if (m_noButtonHoverFlg || m_yesButtonHoverFlg)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType_HIT);
	}

	//クリック
	if (!KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true)) { return; }

	//「いいえ」
	if (m_noButtonHoverFlg)
	{
		m_noButtonHoverFlg = false;
		m_pausePage = PausePage::Main;
		return;
	}

	//「はい」
	if (m_yesButtonHoverFlg)
	{
		m_yesButtonHoverFlg = false;

		if (m_pausePage == PausePage::ConfirmTitle)
		{
			//タイトルへ
			FadeDraw::Instance().SetFadeType(FadeIn, 1);
			SceneManager::Instance().SetNextScene(SceneManager::SceneType::Title);
			PauseClose();
		}
		else
		{
			//ゲーム終了
			Application::Instance().End();
		}
		return;
	}
}

bool PauseUI::IsMouseOnTex(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos)
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
{
	m_pauseMenuFlg = !m_pauseMenuFlg;
	m_pausePage = PausePage::Main;


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

	if (!m_spEscBackTex)
	{
		m_spEscBackTex = std::make_shared<KdTexture>();
		m_spEscBackTex->Load("Asset/Textures/GameUI/Pause/Text/EscBack.png");
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


	if (!m_spBlackTex)
	{
		m_spBlackTex = std::make_shared<KdTexture>();
		m_spBlackTex->Load("Asset/Textures/FadeDraw/FadeDraw.png");
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
