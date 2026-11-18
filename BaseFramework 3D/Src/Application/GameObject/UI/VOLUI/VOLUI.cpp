#include "VOLUI.h"
#include"../../../Info/MouseInfo/MouseInfo.h"
#include"../../../Info/KeyInfo/KeyInfo.h"

void VOLUI::Init()
{
	m_volChangeOpenFlg = false;


	if (!m_spVolTex)
	{
		m_spVolTex = std::make_shared<KdTexture>();
		m_spVolTex->Load("Asset/Textures/GameUI/VOL/VOLIcon.png");
		m_volSiz.x = m_spVolTex->GetWidth();
		m_volSiz.y = m_spVolTex->GetHeight();
	}

	if (!m_spVolBackTex)
	{
		m_spVolBackTex = std::make_shared<KdTexture>();
		m_spVolBackTex->Load("Asset/Textures/GameUI/VOL/VOLBack.png");
		m_volBackSiz.x = m_spVolBackTex->GetWidth();
		m_volBackSiz.y = m_spVolBackTex->GetHeight();
	}


	//つまみ
	if (!m_spVolThumbTex)
	{
		m_spVolThumbTex = std::make_shared<KdTexture>();
		m_spVolThumbTex->Load("Asset/Textures/GameUI/VOL/VOLThumb.png");
		m_volThumbSiz = { (float)m_spVolThumbTex->GetWidth(),(float)m_spVolThumbTex->GetHeight() };
	}
	if (!m_spVolThumbHoverTex)
	{
		m_spVolThumbHoverTex = std::make_shared<KdTexture>();
		m_spVolThumbHoverTex->Load("Asset/Textures/GameUI/VOL/VOLThumb_Hover.png");
	}
	m_volDragIndex = -1;

	for (int i = 0;i < kVolInfoSiz;i++)
	{
		m_volInfo[i].m_spVolTex = std::make_shared<KdTexture>();
		m_volInfo[i].m_spVolValTex = std::make_shared<KdTexture>();

		switch (i)
		{
		case 0:
			m_volInfo[i].m_spVolTex->Load("Asset/Textures/GameUI/VOL/MAIcon.png");
			m_volInfo[i].m_vol = VOLInfo::Instance().GetMSVol();
			break;
		case 1:
			m_volInfo[i].m_spVolTex->Load("Asset/Textures/GameUI/VOL/BGMIcon.png");
			m_volInfo[i].m_vol = VOLInfo::Instance().GetBGMVol();
			break;
		case 2:
			m_volInfo[i].m_spVolTex->Load("Asset/Textures/GameUI/VOL/SEIcon.png");
			m_volInfo[i].m_vol = VOLInfo::Instance().GetSEVol();
			break;
		default:
			break;
		}

		m_volInfo[i].m_spVolValTex->Load("Asset/Textures/GameUI/VOL/VOLValue.png");



		m_volInfo[i].m_volSiz.x = m_volInfo[i].m_spVolTex->GetWidth();
		m_volInfo[i].m_volSiz.y = m_volInfo[i].m_spVolTex->GetHeight();

		m_volInfo[i].m_volValSiz.x = m_volInfo[i].m_spVolValTex->GetWidth();
		m_volInfo[i].m_volValSiz.y = m_volInfo[i].m_spVolValTex->GetHeight();


	}
}

void VOLUI::Update()
{
	POINT mouse = MouseInfo::Instance().m_windowPos;

	{
		float left = m_volPos.x - m_volSiz.x / 2;
		float right = m_volPos.x + m_volSiz.x / 2;
		float top = m_volPos.y + m_volSiz.y/2;
		float bot = m_volPos.y - m_volSiz.y / 2;

		if (left <= mouse.x && right >= mouse.x &&
			top >= mouse.y && bot <= mouse.y)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType_HIT);
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON,true))
			{
				m_volChangeOpenFlg = !m_volChangeOpenFlg;
			}
		}
	}

	if (!m_volChangeOpenFlg)
	{
		m_volDragIndex = -1;
		return;
	}

	// 枠の外をクリックしたら閉じる（ドラッグ中は閉じない）
	if (m_volDragIndex < 0)
	{
		float left = m_volBackPos.x - m_volBackSiz.x / 2;
		float right = m_volBackPos.x + m_volBackSiz.x / 2;
		float top = m_volBackPos.y + m_volBackSiz.y;
		float bot = m_volBackPos.y;

		bool isOnBack = (left <= mouse.x && right >= mouse.x &&
			top >= mouse.y && bot <= mouse.y);

		if (!isOnBack && KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{
			m_volChangeOpenFlg = false;
			return;
		}
	}

	// つまみの操作
	VolThumbUpdate();

	// 音量を反映
	for (int i = 0;i < kVolInfoSiz;i++)
	{
		switch (i)
		{
		case 0:
			VOLInfo::Instance().SetMSVol(m_volInfo[i].m_vol);
			break;
		case 1:
			VOLInfo::Instance().SetBGMVol(m_volInfo[i].m_vol);
			break;
		case 2:
			VOLInfo::Instance().SetSEVol(m_volInfo[i].m_vol);
			break;
		default:
			break;
		}
	}
}

void VOLUI::DrawSprite()
{


	if (m_spVolTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spVolTex, m_volPos.x, m_volPos.y);
	}

	if (!m_volChangeOpenFlg) { return; }

	if (m_spVolBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spVolBackTex, m_volBackPos.x, m_volBackPos.y, nullptr, nullptr, { 0.5,0 });
	}

	for (int i = 0;i < kVolInfoSiz;i++)
	{
		if (!m_volInfo[i].m_spVolTex) { continue; }
		if (!m_volInfo[i].m_spVolValTex) { continue; }

		KdShaderManager::Instance().m_spriteShader.DrawTex(m_volInfo[i].m_spVolTex, m_volInfo[i].m_volPos.x, m_volInfo[i].m_volPos.y);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_volInfo[i].m_spVolValTex, m_volInfo[i].m_volValPos.x, m_volInfo[i].m_volValPos.y, nullptr, nullptr, { 0.5,0 });


		// 緑のゲージ：画像を縦につぶさず、下から音量の分だけ切り取って描く（目盛りがつぶれないように）
		{
			const long texW = (long)m_volInfo[i].m_volValSiz.x;
			const long texH = (long)m_volInfo[i].m_volValSiz.y;
			const long cutH = (long)(texH * m_volInfo[i].m_vol);		// 見せる高さ（下から）

			if (cutH > 0)
			{
				// 画像の y は上が 0 なので、下の cutH ぶんを切り取る
				Math::Rectangle rec = { 0, texH - cutH, texW, cutH };
				Math::Color color = { 0,0.7,0,1 };
				KdShaderManager::Instance().m_spriteShader.DrawTex(m_volInfo[i].m_spVolValTex, m_volInfo[i].m_volValPos.x, m_volInfo[i].m_volValPos.y, (int)texW, (int)cutH, &rec, &color, { 0.5,0 });
			}
		}

		//つまみ（ドット絵がにじまないよう、ポイントフィルタで描く）
		const std::shared_ptr<KdTexture>& spThumbTex = m_volInfo[i].m_thumbHoverFlg ? m_spVolThumbHoverTex : m_spVolThumbTex;
		if (spThumbTex)
		{
			KdShaderManager::Instance().m_spriteShader.Begin(false);
			KdShaderManager::Instance().m_spriteShader.DrawTex(spThumbTex, m_volInfo[i].m_volValPos.x, VolToThumbY(i), m_volThumbSiz.x, m_volThumbSiz.y);
			KdShaderManager::Instance().m_spriteShader.End();
		}
	}
}

void VOLUI::SetVolPos(Math::Vector2 _pos)
{
	m_volPos = _pos;

	m_volBackPos = _pos;
	m_volBackPos.y += m_volSiz.y / 2;


	Math::Vector2 volInfobasePos = m_volBackPos;
	volInfobasePos.y += m_volSiz.y + 20;
	volInfobasePos.x -= (m_volBackSiz.x / kVolInfoSiz);


	float siz = m_volBackSiz.x / 3;
	for (int i = 0;i < kVolInfoSiz;i++)
	{
		m_volInfo[i].m_volPos = volInfobasePos;
		m_volInfo[i].m_volValPos = m_volInfo[i].m_volPos;
		m_volInfo[i].m_volValPos.y += m_volInfo[i].m_volSiz.y / 2;


		volInfobasePos.x += siz;
	}
}

float VOLUI::VolToThumbY(int _index) const
{
	const VolInfo& info = m_volInfo[_index];
	const float bot = info.m_volValPos.y + m_volBarInset;
	const float top = info.m_volValPos.y + info.m_volValSiz.y - m_volBarInset;
	return bot + (top - bot) * info.m_vol;
}

float VOLUI::ThumbYToVol(int _index, float _y) const
{
	const VolInfo& info = m_volInfo[_index];
	const float bot = info.m_volValPos.y + m_volBarInset;
	const float top = info.m_volValPos.y + info.m_volValSiz.y - m_volBarInset;
	if (top - bot <= 0.0f) { return 0.0f; }
	return std::clamp((_y - bot) / (top - bot), 0.0f, 1.0f);
}

void VOLUI::VolThumbUpdate()
{
	POINT mouse = MouseInfo::Instance().m_windowPos;

	//---------- ドラッグ中 ----------
	if (m_volDragIndex >= 0)
	{
		// 左ボタンが押されたままか（離したらドラッグ終了）
		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
		{
			m_volDragIndex = -1;
		}
		else
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType_HIT);

			// つまみの中心が来てほしいy（つかんだ位置のずれを引く）
			float wantY = (float)mouse.y - m_volDragOffsetY;
			m_volInfo[m_volDragIndex].m_vol = ThumbYToVol(m_volDragIndex, wantY);
		}
	}

	for (int i = 0;i < kVolInfoSiz;i++)
	{
		VolInfo& info = m_volInfo[i];

		// つまみの四角
		const float thumbY = VolToThumbY(i);
		float thumbLeft = info.m_volValPos.x - m_volThumbSiz.x / 2;
		float thumbRight = info.m_volValPos.x + m_volThumbSiz.x / 2;
		float thumbTop = thumbY + m_volThumbSiz.y / 2;
		float thumbBot = thumbY - m_volThumbSiz.y / 2;

		// バーの四角（バーは下が基準 { 0.5,0 }）
		float barLeft = info.m_volValPos.x - info.m_volValSiz.x / 2;
		float barRight = info.m_volValPos.x + info.m_volValSiz.x / 2;
		float barTop = info.m_volValPos.y + info.m_volValSiz.y;
		float barBot = info.m_volValPos.y;

		bool isOnThumb = (mouse.x >= thumbLeft && mouse.x <= thumbRight &&
			mouse.y >= thumbBot && mouse.y <= thumbTop);

		bool isOnBar = (mouse.x >= barLeft && mouse.x <= barRight &&
			mouse.y >= barBot && mouse.y <= barTop);

		// つまみの色：マウスが乗っているか、このバーをドラッグ中なら明るく
		info.m_thumbHoverFlg = isOnThumb || (m_volDragIndex == i);

		// ドラッグ中はほかのバーを触らない
		if (m_volDragIndex >= 0) { continue; }

		if (isOnThumb || isOnBar)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType_HIT);

			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_volDragIndex = i;

				if (isOnThumb)
				{
					// つまみをつかんだ：つかんだ位置のずれを覚える
					m_volDragOffsetY = (float)mouse.y - thumbY;
				}
				else
				{
					// バーをクリック：その位置へつまみを移動して、そのままドラッグ
					m_volDragOffsetY = 0.0f;
					info.m_vol = ThumbYToVol(i, (float)mouse.y);
				}
				info.m_thumbHoverFlg = true;
			}

			//---------- マウスホイール ----------
			int wheel = MouseInfo::Instance().GetMouseWheelValue();
			if (wheel >= 1) { info.m_vol += m_volWheelStep; }
			else if (wheel <= -1) { info.m_vol -= m_volWheelStep; }
			info.m_vol = std::clamp(info.m_vol, 0.0f, 1.0f);
		}
	}
}
