//---- 自分のヘッダー ----
#include "PrepareSceneDisplay.h"							//準備シーンの画面管理

//---- シーン ----
#include"../../../../Scene/SceneManager.h"					//シーンの管理
#include"../../../../Scene/PrepareScene/PrepareScene.h"		//準備シーン

//---- 便利機能 ----
#include"../../../../Info/MouseInfo/MouseInfo.h"			//マウス入力
#include"../../../../Info/KeyInfo/KeyInfo.h"				//キー入力
#include"../../../../Info/NumDraw/NumDraw.h"				//数字の画像描画

//=============================================
// 基本の処理
//=============================================

void PrepareSceneDisplay::Init()
{
	LoadMenuTex();
	LoadInfoTex();

	//メニューの並び(左端・上下中央を基準)

	//左端のX座標(全ボタン共通)
	const float menuLeft = -600.0f;

	//一番上(EXPLORE)のY座標
	const float menuTopY = 270.0f;

	//ボタン同士の間隔
	const float menuSpaceY = 50.0f;

	for (int i = 0; i < static_cast<int>(PrepareMenu::MenuSize); i++)
	{
		m_menuSize[i].x = m_spMenuTex[i]->GetWidth() * 0.5;
		m_menuSize[i].y = m_spMenuTex[i]->GetHeight() * 0.5;

		m_menuDrawSize[i] = m_menuSize[i] * m_menuScale;

		//左端を揃えて上から順に並べる → 幅が段々短くなる階段状になる
		m_menuPos[i] = { menuLeft, menuTopY - menuSpaceY * i };

		m_mouseHitFlg[i] = false;
	}

	//画面タイトル
	SetTextPos(InfoText::Title, -100.0f, 299.0f);

	//パネルの左端X(3枚共通)
	const float panelLeft = 285.0f;

	//今の装備パネルの上端Y
	const float equipTop = 240.0f;

	//所持金パネルの上端Y
	const float moneyTop = 40.0f;

	//前回の記録パネルの上端Y
	const float recordTop = -90.0f;

	//項目の左端(点線の内側に入れるための余白込み)
	const float textLeft = panelLeft + 26.0f;

	//値(剣・脱出など)の左端
	const float valueLeft = panelLeft + 170.0f;

	//今の装備パネル(剣とダガーは同じ場所)
	SetTextPos(InfoText::NowEquip, panelLeft + 20.0f, equipTop - 48.0f);
	SetTextPos(InfoText::Weapon, textLeft, equipTop - 100.0f);
	SetTextPos(InfoText::Sword, valueLeft + 34.0f, equipTop - 100.0f);
	SetTextPos(InfoText::Dagger, valueLeft + 34.0f, equipTop - 100.0f);
	SetTextPos(InfoText::Potion, textLeft, equipTop - 146.0f);
	SetTextPos(InfoText::Count, panelLeft + 258.0f, equipTop - 146.0f);

	m_weaponIconPos = m_textPos[static_cast<int>(InfoText::Sword)];
	m_weaponIconPos.x -= m_weaponIconSize + 8.0f;

	//所持金パネル(Gは金額の数字の右)
	SetTextPos(InfoText::Money, panelLeft + 20.0f, moneyTop - 46.0f);
	SetTextPos(InfoText::Gold, panelLeft + 266.0f, moneyTop - 80.0f);

	//前回の記録パネル(階は到達階の数字の右、脱出とロストは同じ場所)
	SetTextPos(InfoText::LastRecord, panelLeft + 20.0f, recordTop - 48.0f);
	SetTextPos(InfoText::Reached, textLeft, recordTop - 92.0f);
	SetTextPos(InfoText::Floor, valueLeft + 22.0f, recordTop - 92.0f);
	SetTextPos(InfoText::ResultLabel, textLeft, recordTop - 136.0f);
	SetTextPos(InfoText::Escape, valueLeft, recordTop - 136.0f);
	SetTextPos(InfoText::Lost, valueLeft, recordTop - 136.0f);

	//パネルの位置(左端・上下中央)= 上端 - 高さの半分
	m_panelPos[static_cast<int>(InfoPanel::NowEquip)] = { panelLeft, equipTop - 180.0f / 2.0f };
	m_panelPos[static_cast<int>(InfoPanel::Money)] = { panelLeft, moneyTop - 110.0f / 2.0f };
	m_panelPos[static_cast<int>(InfoPanel::LastRecord)] = { panelLeft, recordTop - 170.0f / 2.0f };
}

void PrepareSceneDisplay::Update()
{
	if (m_TabActiveFlg) { return; }


	for (int i = 0; i < static_cast<int>(PrepareMenu::MenuSize); i++)
	{
		if (MouseHit(m_menuPos[i], m_menuDrawSize[i].x, m_menuDrawSize[i].y))
		{
			m_mouseHitFlg[i] = true;

			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				switch (static_cast<PrepareMenu>(i))
				{
				case PrepareMenu::Explore:	OnExplore();	break;
				case PrepareMenu::Equip:	OnEquip();		break;
				case PrepareMenu::Shop:		OnShop();		break;
				case PrepareMenu::Records:	OnRecords();	break;
				default:									break;
				}
			}
		}
		else
		{
			m_mouseHitFlg[i] = false;
		}
	}
}

void PrepareSceneDisplay::DrawSprite()
{
	if (m_TabActiveFlg) { return; }
	for (int i = 0; i < static_cast<int>(PrepareMenu::MenuSize); i++)
	{
		if (!m_spMenuTex[i]) { continue; }
		if (!m_spNotMenuTex[i]) { continue; }

		float w = m_menuDrawSize[i].x;
		float h = m_menuDrawSize[i].y;

		if (m_mouseHitFlg[i])
		{
			w *= m_mouseHitScale;
			h *= m_mouseHitScale;
		}
		//選ばれているか、マウスが乗っていたら明るい画像
		const bool isSelect = IsMenuSelected(static_cast<PrepareMenu>(i)) || m_mouseHitFlg[i];
		KdTexture* pTex;

		if (isSelect)
		{
			pTex = m_spMenuTex[i].get();
		}
		else
		{
			pTex = m_spNotMenuTex[i].get();
		}


		//pivotを{0,0.5}にして左端基準で描画
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			pTex,
			static_cast<int>(m_menuPos[i].x), static_cast<int>(m_menuPos[i].y),
			static_cast<int>(w), static_cast<int>(h),
			nullptr, &kWhiteColor, { 0.0f, 0.5f });
	}

	//ここが開かれてたら描画
	if (GetActive())
	{
		Draw();
	}
}

//=============================================
// 設定
//=============================================

void PrepareSceneDisplay::SetMoney(long _money)
{
	//マイナスにはしない
	m_money = std::max(0L, _money);
}

//=============================================
// 基本の処理
//=============================================

void PrepareSceneDisplay::Draw()
{
	auto& sprite = KdShaderManager::Instance().m_spriteShader;

	//パネル(文字より先に描く)
	for (int i = 0; i < static_cast<int>(InfoPanel::PanelSize); i++)
	{
		if (!m_spPanelTex[i]) { continue; }
		sprite.DrawTex(m_spPanelTex[i],
			static_cast<int>(m_panelPos[i].x), static_cast<int>(m_panelPos[i].y),
			nullptr, &kWhiteColor, { 0.0f, 0.5f });
	}

	//いつも出す文字
	DrawInfoText(InfoText::Title);
	DrawInfoText(InfoText::NowEquip);
	DrawInfoText(InfoText::Weapon);
	DrawInfoText(InfoText::Potion);
	DrawInfoText(InfoText::Count);
	DrawInfoText(InfoText::Money);
	DrawInfoText(InfoText::Gold);
	DrawInfoText(InfoText::LastRecord);
	DrawInfoText(InfoText::Reached);
	DrawInfoText(InfoText::Floor);
	DrawInfoText(InfoText::ResultLabel);

	//今の装備で切り替える文字
	bool isDagger = (SceneManager::Instance().GetPlayerWeaponInfo() == PlayerWeaponInfo_Dagger);
	DrawInfoText(isDagger ? InfoText::Dagger : InfoText::Sword);

	//武器アイコン(今の装備に合わせて切り替え)
	int weapon = SceneManager::Instance().GetPlayerWeaponInfo();
	if (m_spWeaponIconTex[weapon])
	{
		sprite.DrawTex(m_spWeaponIconTex[weapon].get(),
			static_cast<int>(m_weaponIconPos.x), static_cast<int>(m_weaponIconPos.y),
			static_cast<int>(m_weaponIconSize), static_cast<int>(m_weaponIconSize),
			nullptr, &kWhiteColor, { 0.0f, 0.5f });
	}

	//前回の記録
	if (m_gameRecord.m_hasLatest && m_gameRecord.m_latest.m_displayFloorInfo.m_displayFloor > 0)
	{
		//[0]は全体の情報
		int  floor = m_gameRecord.m_latest.m_displayFloorInfo.m_displayFloor;
		bool isLost = m_gameRecord.m_latest.m_playerDead;

		//「到達階」の右 〜 「階」の左
		DrawInfoNum(floor, InfoText::Floor, false, 64.0f);
		DrawInfoText(isLost ? InfoText::Lost : InfoText::Escape);
	}

	//ポーションの数(「ポーション」の右 〜 「個」の左)
	DrawInfoNum(m_potionNum, InfoText::Count, false, 72.0f);

	//所持金は3桁区切り(1,250)。パネルの左端 〜 「G」の左
	DrawInfoNum(m_money, InfoText::Gold, true, 230.0f);
}

//=============================================
// その他の処理
//=============================================

void PrepareSceneDisplay::LoadMenuTex()
{
	for (int i = 0; i < static_cast<int>(PrepareMenu::MenuSize); i++)
	{
		if (!m_spMenuTex[i])
		{
			m_spMenuTex[i] = std::make_shared<KdTexture>();
			if (!m_spMenuTex[i]->Load(m_menuTexPath[i]))
			{
				KdDebugGUI::Instance().AddLog(U8("[Error] PrepareMenuの画像が読み込めません : %s\n"), m_menuTexPath[i].c_str());
			}
		}



		if (!m_spNotMenuTex[i])
		{
			m_spNotMenuTex[i] = std::make_shared<KdTexture>();
			if (!m_spNotMenuTex[i]->Load(m_menuNotTexPath[i]))
			{
				KdDebugGUI::Instance().AddLog(U8("[Error] PrepareMenuの画像が読み込めません : %s\n"), m_menuTexPath[i].c_str());
			}
		}

	}
}

void PrepareSceneDisplay::LoadInfoTex()
{
	LoadTexArray(m_spPanelTex, m_panelTexPath, static_cast<int>(InfoPanel::PanelSize));
	LoadTexArray(m_spTextTex, m_textTexPath, static_cast<int>(InfoText::TextSize));
	LoadTexArray(m_spWeaponIconTex, m_weaponIconTexPath, PlayerWeaponInfoSize);
}

void PrepareSceneDisplay::LoadTexArray(std::shared_ptr<KdTexture>* _spTex, const std::string* _path, int _num)
{
	for (int i = 0; i < _num; i++)
	{
		if (_spTex[i]) { continue; }

		_spTex[i] = std::make_shared<KdTexture>();
		if (!_spTex[i]->Load(_path[i]))
		{
			KdDebugGUI::Instance().AddLog(U8("[Error] PrepareSceneDisplayの画像が読み込めません : %s\n"), _path[i].c_str());
		}
	}
}

bool PrepareSceneDisplay::MouseHit(Math::Vector2 _pos, float _w, float _h)
{
	float left = _pos.x;
	float right = _pos.x + _w;
	float top = _pos.y - _h / 2.0f;
	float bottom = _pos.y + _h / 2.0f;

	float mx = MouseInfo::Instance().m_windowPos.x;
	float my = MouseInfo::Instance().m_windowPos.y;

	if (left <= mx && mx <= right &&
		top <= my && my <= bottom)
	{
		//ButtonBaseと同じく、当たっていたらカーソルの見た目を変える
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		return true;
	}

	return false;
}

void PrepareSceneDisplay::SetTextPos(InfoText _text, float _x, float _y)
{
	m_textPos[static_cast<int>(_text)] = { _x, _y };
}

void PrepareSceneDisplay::DrawInfoText(InfoText _text, Math::Vector2 _offset)
{
	int i = static_cast<int>(_text);
	if (!m_spTextTex[i]) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTextTex[i],
		static_cast<int>(m_textPos[i].x + _offset.x),
		static_cast<int>(m_textPos[i].y + _offset.y),
		nullptr, &kWhiteColor, { 0.0f, 0.5f });
}

void PrepareSceneDisplay::DrawInfoNum(long _num, InfoText _unit, bool _separator, float _maxW)
{
	const Math::Vector2& unitPos = m_textPos[static_cast<int>(_unit)];

	NumDraw::GetInstance().Draw(_num, RAligned,
		{ unitPos.x - m_numGap, unitPos.y },
		Math::Color(1, 1, 1, 1), m_numSize, _separator, -1, { _maxW, 0.0f });
}

//=============================================
// メニューが押された時の処理
//=============================================

void PrepareSceneDisplay::OnExplore()
{
	if (auto spScene = m_wpPrepareScene.lock())
	{
		spScene->ChangeTab(PrepareScene::PrepareTab::None);
	}
}

void PrepareSceneDisplay::OnEquip()
{
	if (auto spScene = m_wpPrepareScene.lock())
	{
		spScene->ChangeTab(PrepareScene::PrepareTab::Equip);
	}
}

void PrepareSceneDisplay::OnShop()
{
	if (auto spScene = m_wpPrepareScene.lock())
	{
		spScene->ChangeTab(PrepareScene::PrepareTab::Shop);
	}
}

void PrepareSceneDisplay::OnRecords()
{
	if (auto spScene = m_wpPrepareScene.lock())
	{
		spScene->ChangeTab(PrepareScene::PrepareTab::Records);
	}
}

bool PrepareSceneDisplay::IsMenuSelected(PrepareMenu _menu) const
{
	auto spScene = m_wpPrepareScene.lock();
	if (!spScene) { return true; }

	const PrepareScene::PrepareTab nowTab = spScene->GetCurrentTab();

	//メニューとタブを対応させる(並び順が違うので1つずつ)
	switch (_menu)
	{
	case PrepareMenu::Equip:   return nowTab == PrepareScene::PrepareTab::Equip;
	case PrepareMenu::Shop:    return nowTab == PrepareScene::PrepareTab::Shop;
	case PrepareMenu::Records: return nowTab == PrepareScene::PrepareTab::Records;
	case PrepareMenu::Explore: return nowTab == PrepareScene::PrepareTab::None;
	default:                   return false;
	}
}
