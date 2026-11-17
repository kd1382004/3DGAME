#include "PrepareSceneDisplay.h"
#include"../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Scene/SceneManager.h"
#include"../../../../Scene/PrepareScene/PrepareScene.h"
#include"../../../../Info/NumDraw/NumDraw.h"
namespace
{
	// メニュー画像の拡大率(元画像は高さ32px → 96px)
	constexpr float kMenuScale = 3.0f;

	//マウスが当たった時の拡大率
	constexpr float kmouseHitScale = 1.1f;
}

void PrepareSceneDisplay::Init()
{
	LoadMenuTex();
	LoadInfoTex();

	// メニューの並び(左端・上下中央を基準)
	const float menuLeft = -550.0f;	// 左端のX座標(全ボタン共通)
	const float menuTopY = 162.0f;	// 一番上(EXPLORE)のY座標
	const float menuSpaceY = 110.0f;	// ボタン同士の間隔

	for (int i = 0; i < static_cast<int>(PrepareMenu::MenuSize); i++)
	{
		m_menuSiz[i].x = m_spMenuTex[i]->GetWidth();
		m_menuSiz[i].y = m_spMenuTex[i]->GetHeight();

		m_menuDrawSiz[i] = m_menuSiz[i] * kMenuScale;


		// 左端を揃えて上から順に並べる → 幅が段々短くなる階段状になる
		m_menuPos[i] = { menuLeft, menuTopY - menuSpaceY * i };

		m_mouseHitFlg[i] = false;
	}



	// 文字の位置(左端・上下中央を基準。画面中央が0,0、上がプラス)
	auto setTextPos = [&](InfoText _text, float _x, float _y)
		{
			m_textPos[static_cast<int>(_text)] = { _x, _y };
		};

	// 画面タイトル
	setTextPos(InfoText::Title, -610.0f, 299.0f);

	// パネルの左端X(3枚共通)と、各パネルの上端Y
	const float panelLeft = 285.0f;
	const float equipTop = 240.0f;	// 今の装備
	const float moneyTop = 40.0f;	// 所持金
	const float recordTop = -90.0f;	// 前回の記録

	// 点線の内側に入れるための余白
	const float textLeft = panelLeft + 26.0f;	// 項目の左端
	const float valueLeft = panelLeft + 170.0f;	// 値(剣・脱出など)の左端

	// 今の装備パネル
	setTextPos(InfoText::NowEquip, panelLeft + 20.0f, equipTop - 48.0f);
	setTextPos(InfoText::Weapon, textLeft, equipTop - 100.0f);
	setTextPos(InfoText::Sword, valueLeft + 34.0f, equipTop - 100.0f);	// 剣とダガーは同じ場所
	setTextPos(InfoText::Dagger, valueLeft + 34.0f, equipTop - 100.0f);
	setTextPos(InfoText::Potion, textLeft, equipTop - 146.0f);
	setTextPos(InfoText::Count, panelLeft + 258.0f, equipTop - 146.0f);

	m_weaponIconPos = m_textPos[static_cast<int>(InfoText::Sword)];
	m_weaponIconPos.x -= m_weaponIconSiz + 8.0f;

	// 所持金パネル
	setTextPos(InfoText::Money, panelLeft + 20.0f, moneyTop - 46.0f);
	setTextPos(InfoText::Gold, panelLeft + 266.0f, moneyTop - 80.0f);	// 金額の数字の右

	// 前回の記録パネル
	setTextPos(InfoText::LastRecord, panelLeft + 20.0f, recordTop - 48.0f);
	setTextPos(InfoText::Reached, textLeft, recordTop - 92.0f);
	setTextPos(InfoText::Floor, valueLeft + 22.0f, recordTop - 92.0f);	// 到達階の数字の右
	setTextPos(InfoText::ResultLabel, textLeft, recordTop - 136.0f);
	setTextPos(InfoText::Escape, valueLeft, recordTop - 136.0f);	// 脱出とロストは同じ場所
	setTextPos(InfoText::Lost, valueLeft, recordTop - 136.0f);

	// パネルの位置(左端・上下中央)= 上端 - 高さの半分
	m_panelPos[static_cast<int>(InfoPanel::NowEquip)] = { panelLeft, equipTop - 180.0f / 2.0f };
	m_panelPos[static_cast<int>(InfoPanel::Money)] = { panelLeft, moneyTop - 110.0f / 2.0f };
	m_panelPos[static_cast<int>(InfoPanel::LastRecord)] = { panelLeft, recordTop - 170.0f / 2.0f };
}

void PrepareSceneDisplay::Update()
{
	for (int i = 0; i < static_cast<int>(PrepareMenu::MenuSize); i++)
	{
		if (MouseHit(m_menuPos[i], m_menuDrawSiz[i].x, m_menuDrawSiz[i].y))
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


	//ここが開かれてたら更新
	if (GetActive())
	{

	}
}

void PrepareSceneDisplay::DrawSprite()
{
	for (int i = 0; i < static_cast<int>(PrepareMenu::MenuSize); i++)
	{
		if (!m_spMenuTex[i]) { continue; }


		float w = m_menuDrawSiz[i].x;
		float h = m_menuDrawSiz[i].y;

		if (m_mouseHitFlg[i])
		{
			w *= kmouseHitScale;
			h *= kmouseHitScale;
		}

		// pivotを{0,0.5}にして左端基準で描画
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_spMenuTex[i].get(),
			static_cast<int>(m_menuPos[i].x), static_cast<int>(m_menuPos[i].y),
			w, h,
			nullptr, &kWhiteColor, { 0.0f, 0.5f });
	}


	//ここが開かれてたら描画
	if (GetActive())
	{
		Draw();
	}

}

void PrepareSceneDisplay::SetMoney(long _money)
{
	// マイナスにはしない
	m_money = std::max(0L, _money);
}

void PrepareSceneDisplay::LoadMenuTex()
{
	for (int i = 0; i < static_cast<int>(PrepareMenu::MenuSize); i++)
	{
		if (m_spMenuTex[i]) { continue; }

		m_spMenuTex[i] = std::make_shared<KdTexture>();
		if (!m_spMenuTex[i]->Load(m_menuTexPath[i]))
		{
			KdDebugGUI::Instance().AddLog(U8("[Error] PrepareMenuの画像が読み込めません : %s\n"), m_menuTexPath[i].c_str());
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

void PrepareSceneDisplay::Draw()
{
	auto& sprite = KdShaderManager::Instance().m_spriteShader;

	// 文字を1つ描く(_offsetで位置をずらせる)
	auto drawText = [&](InfoText _text, Math::Vector2 _offset = {})
		{
			int i = static_cast<int>(_text);
			if (!m_spTextTex[i]) { return; }
			sprite.DrawTex(m_spTextTex[i],
				static_cast<int>(m_textPos[i].x + _offset.x),
				static_cast<int>(m_textPos[i].y + _offset.y),
				nullptr, &kWhiteColor, { 0.0f, 0.5f });
		};


	// パネル(文字より先に描く)
	for (int i = 0; i < static_cast<int>(InfoPanel::PanelSize); i++)
	{
		if (!m_spPanelTex[i]) { continue; }
		sprite.DrawTex(m_spPanelTex[i],
			static_cast<int>(m_panelPos[i].x), static_cast<int>(m_panelPos[i].y),
			nullptr, &kWhiteColor, { 0.0f, 0.5f });
	}

	// いつも出す文字
	drawText(InfoText::Title);
	drawText(InfoText::NowEquip);
	drawText(InfoText::Weapon);
	drawText(InfoText::Potion);
	drawText(InfoText::Count);
	drawText(InfoText::Money);
	drawText(InfoText::Gold);
	drawText(InfoText::LastRecord);
	drawText(InfoText::Reached);
	drawText(InfoText::Floor);
	drawText(InfoText::ResultLabel);

	// ③ 状態で切り替える文字
	bool isDagger = (SceneManager::Instance().GetPlayerWeaponInfo() == PlayerWeaponInfo_Dagger);
	drawText(isDagger ? InfoText::Dagger : InfoText::Sword);



	// 武器アイコン(今の装備に合わせて切り替え)
	int weapon = SceneManager::Instance().GetPlayerWeaponInfo();
	if (m_spWeaponIconTex[weapon])
	{
		sprite.DrawTex(m_spWeaponIconTex[weapon].get(),
			static_cast<int>(m_weaponIconPos.x), static_cast<int>(m_weaponIconPos.y),
			static_cast<int>(m_weaponIconSiz), static_cast<int>(m_weaponIconSiz),
			nullptr, &kWhiteColor, { 0.0f, 0.5f });
	}


	// 数字(単位の文字のすぐ左に右揃えで描く)
	const float numSiz = 2.0f;	// 元画像9×14px → 18×28px
	const float numGap = 4.0f;	// 数字と単位のすき間

	// _maxW … 超えてはいけない横幅(項目の文字にかぶらないように。0なら制限なし)
	auto drawNum = [&](long _num, InfoText _unit, bool _separator = false, float _maxW = 0.0f)
		{
			const Math::Vector2& unitPos = m_textPos[static_cast<int>(_unit)];
			NumDraw::GetInstance().Draw(_num, RAligned,
				{ unitPos.x - numGap, unitPos.y },
				Math::Color(1, 1, 1, 1), numSiz, _separator, -1, { _maxW, 0.0f });
		};
	
	// 前回の記録(PrepareSceneDisplay)
	if (m_gameRecord.m_hasLatest && !m_gameRecord.m_latest.m_displayFloorInfo.empty())
	{
		int  floor = m_gameRecord.m_latest.m_displayFloorInfo[0].m_displayFloor;	// [0]は全体の情報
		bool isLost = m_gameRecord.m_latest.m_playerDead;

		drawNum(floor, InfoText::Floor, false, 64.0f);	// 「到達階」の右 〜 「階」の左
		drawText(isLost ? InfoText::Lost : InfoText::Escape);

	}



	
	drawNum(m_potionNum, InfoText::Count, false, 72.0f);		// 「ポーション」の右 〜 「個」の左
	drawNum(m_money, InfoText::Gold, true, 230.0f);	// 所持金は3桁区切り(1,250)。パネルの左端 〜 「G」の左
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
		// ButtonBaseと同じく、当たっていたらカーソルの見た目を変える
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		return true;
	}

	return false;
}

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
