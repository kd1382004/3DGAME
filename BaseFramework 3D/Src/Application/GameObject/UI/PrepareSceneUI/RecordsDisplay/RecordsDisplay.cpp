//---- 自分のヘッダー ----
#include "RecordsDisplay.h"								//準備シーンの記録画面

//---- シーン ----
#include"../../../../Scene/SceneManager.h"				//シーンの管理

//---- 便利機能 ----
#include"../../../../Info/MouseInfo/MouseInfo.h"		//マウス入力
#include"../../../../Info/KeyInfo/KeyInfo.h"			//キー入力
#include"../../../../Info/NumDraw/NumDraw.h"			//数字の画像描画

//---- 標準ライブラリ ----
#include <ctime>										//時刻
#include <cstdlib>										//乱数・数値変換など
#include <string>										//文字列

//=============================================
// 基本の処理
//=============================================

void RecordsDisplay::Init()
{
	LoadTex();

	KeyInfo::Instance().SetKeyValid(VK_LBUTTON);

	ResetSelect();
}

void RecordsDisplay::Update()
{
	// 開いているタブでなければ何もしない
	if (!m_activeFlg) { return; }

	// カーソルが乗っている枠を探す
	m_hoverIndex = -1;
	for (int i = 0; i <= m_rankingNum; i++)
	{
		//記録が無い枠は選べない
		if (!GetCardResult(i)) { continue; }

		if (MouseHit(GetCardPos(i), m_cardW, m_cardH))
		{
			m_hoverIndex = i;

			//クリックで詳細に出す記録を切り替える
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_selectIndex = i;
			}

			break;
		}
	}
}

void RecordsDisplay::DrawSprite()
{
	// 開いているタブでなければ何もしない
	if (!m_activeFlg) { return; }

	DrawTex(GetTex(RecordsTex::Title), m_titlePos, m_pivotLeft);

	//左側(最新のプレイ・ランキング)
	DrawTex(GetTex(RecordsTex::LatestPlay), m_latestLabelPos, m_pivotLeft);
	DrawTex(GetTex(RecordsTex::RankingLabel), m_rankingLabelPos, m_pivotLeft);
	for (int i = 0; i <= m_rankingNum; i++)
	{
		DrawCard(i);
	}

	//右側(記録の詳細)
	DrawDetail();
}

//=============================================
// 設定
//=============================================

void RecordsDisplay::SetGameRecord(const GameRecord& _gameRecord)
{
	m_gameRecord = _gameRecord;
	ResetSelect();
}

//=============================================
// その他の処理
//=============================================

bool RecordsDisplay::MouseHit(Math::Vector2 _pos, float _w, float _h)
{
	// _pos … 中心座標(画面中央が0,0・上がプラス)
	float left   = _pos.x - _w / 2.0f;
	float right  = _pos.x + _w / 2.0f;
	float top    = _pos.y - _h / 2.0f;
	float bottom = _pos.y + _h / 2.0f;

	float mx = static_cast<float>(MouseInfo::Instance().m_windowPos.x);
	float my = static_cast<float>(MouseInfo::Instance().m_windowPos.y);

	if (left <= mx && mx <= right &&
		top <= my && my <= bottom)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		return true;
	}

	return false;
}

void RecordsDisplay::LoadTex()
{
	for (int i = 0; i < static_cast<int>(RecordsTex::TexSize); i++)
	{
		LoadOneTex(m_spTex[i], m_texPath[i]);
	}

	for (int w = 0; w < PlayerWeaponInfoSize; w++)
	{
		LoadOneTex(m_spWeaponIconTex[w], m_weaponIconTexPath[w]);
		LoadOneTex(m_spWeaponNameTex[w], m_weaponNameTexPath[w]);
	}
}

void RecordsDisplay::LoadOneTex(std::shared_ptr<KdTexture>& _spTex, const std::string& _path)
{
	if (_spTex) { return; }

	_spTex = std::make_shared<KdTexture>();
	if (!_spTex->Load(_path))
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] RecordsDisplayの画像が読み込めません : %s\n"), _path.c_str());
	}
}

void RecordsDisplay::ResetSelect()
{
	m_hoverIndex = -1;
	m_selectIndex = -1;

	// 最新 → ランキング1位 → … の順で、最初に記録がある枠を選ぶ
	for (int i = 0; i <= m_rankingNum; i++)
	{
		if (GetCardResult(i))
		{
			m_selectIndex = i;
			return;
		}
	}
}

const GameResult* RecordsDisplay::GetCardResult(int _index) const
{
	// 0 … 最新のプレイ
	if (_index == 0)
	{
		return m_gameRecord.m_hasLatest ? &m_gameRecord.m_latest : nullptr;
	}

	// 1〜3 … ランキング(m_bestは1位から順に入っている)
	const int bestIndex = _index - 1;
	if (bestIndex < 0 || bestIndex >= static_cast<int>(m_gameRecord.m_best.size())) { return nullptr; }

	return &m_gameRecord.m_best[bestIndex];
}

Math::Vector2 RecordsDisplay::GetCardPos(int _index) const
{
	if (_index == 0) { return m_latestCardPos; }

	return { m_rankingCardPos.x, m_rankingCardPos.y - m_rankingSpace * (_index - 1) };
}

int RecordsDisplay::GetRank(int _index) const
{
	// ランキングの枠なら、そのまま順位
	if (_index >= 1) { return _index; }

	// 最新のプレイは、同じ日時の記録がランキングにあればその順位
	// (日時が入っていない前のセーブデータは、順位を出さない)
	const GameResult* pLatest = GetCardResult(0);
	if (!pLatest || pLatest->m_playDate == 0) { return 0; }

	for (int i = 0; i < static_cast<int>(m_gameRecord.m_best.size()); i++)
	{
		if (m_gameRecord.m_best[i].m_playDate == pLatest->m_playDate)
		{
			return i + 1;
		}
	}
	return 0;
}

float RecordsDisplay::GetTexW(const std::shared_ptr<KdTexture>& _spTex) const
{
	if (!_spTex) { return 0.0f; }
	return static_cast<float>(_spTex->GetWidth());
}

float RecordsDisplay::GetNumW(long _num, int _digit, bool _separator) const
{
	// NumDrawと同じ数え方(桁数 + 3桁区切りのカンマ)
	int len = static_cast<int>(std::to_string(std::abs(_num)).size());
	if (_digit > len) { len = _digit; }

	int glyphs = len;
	if (_separator) { glyphs += (len - 1) / 3; }

	// 元画像は1文字9px
	return 9.0f * m_numSize * glyphs;
}

void RecordsDisplay::DrawTex(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos, const Math::Vector2& _pivot)
{
	if (!_spTex) { return; }
	DrawTexSize(_spTex, _pos, static_cast<float>(_spTex->GetWidth()), static_cast<float>(_spTex->GetHeight()), _pivot);
}

void RecordsDisplay::DrawTexSize(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos, float _w, float _h, const Math::Vector2& _pivot)
{
	if (!_spTex) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		_spTex.get(),
		static_cast<int>(_pos.x), static_cast<int>(_pos.y),
		static_cast<int>(_w), static_cast<int>(_h),
		nullptr, &kWhiteColor, _pivot);
}

float RecordsDisplay::DrawTexL(const std::shared_ptr<KdTexture>& _spTex, float _x, float _y)
{
	DrawTex(_spTex, { _x, _y }, m_pivotLeft);
	return _x + GetTexW(_spTex);
}

float RecordsDisplay::DrawNumL(long _num, float _x, float _y, const Math::Color& _color, int _digit)
{
	NumDraw::GetInstance().Draw(_num, LAligned, { _x, _y }, _color, m_numSize, false, _digit);
	return _x + GetNumW(_num, _digit);
}

float RecordsDisplay::DrawNumSymbol(RecordsTex _tex, float _x, float _y)
{
	// 数字1文字と同じ大きさ(9×14pxを拡大)
	const float w = 9.0f * m_numSize;
	const float h = 14.0f * m_numSize;

	// ドット絵がにじまないよう、ポイントフィルタで描く(NumDrawと同じ)
	KdShaderManager::Instance().m_spriteShader.Begin(false);
	DrawTexSize(GetTex(_tex), { _x, _y }, w, h, m_pivotLeft);
	KdShaderManager::Instance().m_spriteShader.End();

	return _x + w;
}

void RecordsDisplay::DrawNumUnitR(long _num, const std::shared_ptr<KdTexture>& _spUnit, float _right, float _y, bool _separator)
{
	// 単位を右端にそろえて、数字はそのすぐ左に右揃え
	float numRight = _right;
	if (_spUnit)
	{
		DrawTex(_spUnit, { _right, _y }, m_pivotRight);
		numRight -= GetTexW(_spUnit) + m_numGap;
	}

	NumDraw::GetInstance().Draw(_num, RAligned, { numRight, _y }, m_numColor, m_numSize, _separator);
}

void RecordsDisplay::DrawTimeR(float _timeSc, float _right, float _y)
{
	const int timeSc = static_cast<int>(_timeSc);
	const int sc = timeSc % 60;		// 秒
	const int mine = timeSc / 60;	// 分(1時間を超えても分で数える)

	const float numW = 9.0f * m_numSize;

	// 右から 秒(2桁) → 「:」 → 分(2桁以上)
	NumDraw::GetInstance().Draw(sc, RAligned, { _right, _y }, m_numColor, m_numSize, false, 2);
	const float colonLeft = _right - numW * 2.0f - numW;
	DrawNumSymbol(RecordsTex::NumColon, colonLeft, _y);
	NumDraw::GetInstance().Draw(mine, RAligned, { colonLeft, _y }, m_numColor, m_numSize, false, 2);
}

void RecordsDisplay::DrawDateL(long long _playDate, float _x, float _y)
{
	// 日時が入っていない(前のセーブデータ)ときは出さない
	if (_playDate <= 0) { return; }

	std::time_t t = static_cast<std::time_t>(_playDate);
	std::tm local = {};
	if (localtime_s(&local, &t) != 0) { return; }

	// 月/日
	float x = DrawNumL(local.tm_mon + 1, _x, _y, m_dateColor, 2);
	x = DrawNumSymbol(RecordsTex::NumSlash, x, _y);
	x = DrawNumL(local.tm_mday, x, _y, m_dateColor, 2);

	// 時:分
	x += m_itemGap;
	x = DrawNumL(local.tm_hour, x, _y, m_dateColor, 2);
	x = DrawNumSymbol(RecordsTex::NumColon, x, _y);
	DrawNumL(local.tm_min, x, _y, m_dateColor, 2);
}

void RecordsDisplay::DrawCard(int _index)
{
	const GameResult* pResult = GetCardResult(_index);

	const Math::Vector2 pos = GetCardPos(_index);
	const float left = pos.x - m_cardW / 2.0f;
	const float top  = pos.y + m_cardH / 2.0f;
	const float right = pos.x + m_cardW / 2.0f;

	//記録が無い枠は、枠組み(背景・点線・順位)だけ出す
	if (!pResult)
	{
		DrawEmptyCard(_index, pos, left, top);
		return;
	}

	const DisplayFloorInfo& info = pResult->m_displayFloorInfo;

	//背景
	DrawTex(GetTex(RecordsTex::CardPanel), pos);

	//---- 1行目 … 結果(左)と時間(右) ----
	const float line1Y = top - 32.0f;
	float x = left + m_cardPad;

	if (_index == 0)
	{
		// 最新のプレイ … 「5階で脱出」「6階でロスト」
		x = DrawNumL(info.m_displayFloor, x, line1Y, m_numColor) + m_numGap;
		x = DrawTexL(GetTex(RecordsTex::Floor), x, line1Y);
		DrawTexL(GetTex(pResult->m_playerDead ? RecordsTex::ResultLost : RecordsTex::ResultEscape), x, line1Y);
	}
	else
	{
		// ランキング … 「1位 8階 脱出」
		// 「階」「脱出」「ロスト」の画像は左右に余白があるので、「階」のあとはすき間を足さない
		// (足すと「ロスト」が右の時間にかぶる)
		x = DrawNumL(_index, x, line1Y, m_numColor) + m_numGap;
		x = DrawTexL(GetTex(RecordsTex::RankSuffix), x, line1Y) + m_rankGap;
		x = DrawNumL(info.m_displayFloor, x, line1Y, m_numColor) + m_numGap;
		x = DrawTexL(GetTex(RecordsTex::Floor), x, line1Y);
		DrawTexL(GetTex(pResult->m_playerDead ? RecordsTex::Lost : RecordsTex::Escape), x, line1Y);
	}

	DrawTimeR(info.m_playTimeSc, right - m_cardPad, line1Y);

	//---- 横の点線 ----
	DrawTex(GetTex(RecordsTex::CardDivider), { left + m_cardPad, top - 53.0f }, m_pivotLeft);

	//---- 2行目 … 武器 | 撃破 | 宝箱 ----
	const float line2Y = top - 80.0f;

	//縦の点線(武器と撃破の間・撃破と宝箱の間)
	DrawTex(GetTex(RecordsTex::CardDividerV), { left + m_cardPad + m_colDivider1X, line2Y + 2.0f });
	DrawTex(GetTex(RecordsTex::CardDividerV), { left + m_cardPad + m_colDivider2X, line2Y + 2.0f });

	//武器アイコン
	const int weapon = pResult->m_carryIn.m_weapon;
	if (weapon >= 0 && weapon < PlayerWeaponInfoSize)
	{
		DrawTexSize(m_spWeaponIconTex[weapon], { left + m_cardPad, line2Y }, m_cardIconSize, m_cardIconSize, m_pivotLeft);
	}

	//撃破(文字は左詰め・数字は右詰め)
	DrawTexL(GetTex(RecordsTex::KillLabel), left + m_cardPad + m_colKillLeft, line2Y);
	DrawNumUnitR(info.m_enemyKillCount, nullptr, left + m_cardPad + m_colKillRight, line2Y);

	//宝箱(文字は左詰め・数字は右詰め)
	DrawTexL(GetTex(RecordsTex::TreasureLabel), left + m_cardPad + m_colTreasureLeft, line2Y);
	DrawNumUnitR(info.m_treasureChestsNum, nullptr, right - m_cardPad, line2Y);

	//---- 枠(選んでいる → 金、カーソルが乗っている → 銀) ----
	if (_index == m_selectIndex)
	{
		DrawTex(GetTex(RecordsTex::SelectFrame), pos);
	}
	else if (_index == m_hoverIndex)
	{
		DrawTex(GetTex(RecordsTex::HoverFrame), pos);
	}
}

void RecordsDisplay::DrawEmptyCard(int _index, const Math::Vector2& _pos, float _left, float _top)
{
	//背景
	DrawTex(GetTex(RecordsTex::CardPanel), _pos);

	//ランキングの枠なら順位(「1位」など)だけ出す
	const float line1Y = _top - 32.0f;
	if (_index >= 1)
	{
		const float x = DrawNumL(_index, _left + m_cardPad, line1Y, m_numColor) + m_numGap;
		DrawTexL(GetTex(RecordsTex::RankSuffix), x, line1Y);
	}

	//横の点線
	DrawTex(GetTex(RecordsTex::CardDivider), { _left + m_cardPad, _top - 53.0f }, m_pivotLeft);

	//縦の点線(武器と撃破の間・撃破と宝箱の間)
	const float line2Y = _top - 80.0f;
	DrawTex(GetTex(RecordsTex::CardDividerV), { _left + m_cardPad + m_colDivider1X, line2Y + 2.0f });
	DrawTex(GetTex(RecordsTex::CardDividerV), { _left + m_cardPad + m_colDivider2X, line2Y + 2.0f });
}

void RecordsDisplay::DrawDetail()
{
	//背景
	DrawTex(GetTex(RecordsTex::DetailPanel), m_detailPos);

	const float left  = m_detailPos.x - m_detailW / 2.0f;
	const float top   = m_detailPos.y + m_detailH / 2.0f;
	const float right = m_detailPos.x + m_detailW / 2.0f;

	//文字の左端・数字の右端
	const float textLeft = left + m_detailPad;
	const float numRight = right - m_detailPad;

	//見出し
	DrawTexL(GetTex(RecordsTex::DetailTitle), textLeft, top - 48.0f);
	DrawTex(GetTex(RecordsTex::DetailDivider), { textLeft, top - 72.0f }, m_pivotLeft);

	//選んでいる記録が無ければ見出しだけ
	const GameResult* pResult = GetCardResult(m_selectIndex);
	if (!pResult) { return; }

	const DisplayFloorInfo& info = pResult->m_displayFloorInfo;

	//---- 結果と順位 ----
	float y = top - 108.0f;
	float x = DrawNumL(info.m_displayFloor, textLeft, y, m_numColor) + m_numGap;
	x = DrawTexL(GetTex(RecordsTex::Floor), x, y);
	DrawTexL(GetTex(pResult->m_playerDead ? RecordsTex::ResultLost : RecordsTex::ResultEscape), x, y);

	//順位(ランキングに入っていれば「3位」)
	const int rank = GetRank(m_selectIndex);
	if (rank > 0)
	{
		DrawNumUnitR(rank, GetTex(RecordsTex::RankSuffix), numRight, y);
	}

	//日時
	DrawDateL(pResult->m_playDate, textLeft, top - 144.0f);

	DrawTex(GetTex(RecordsTex::DetailDivider), { textLeft, top - 168.0f }, m_pivotLeft);

	//---- 撃破・宝箱・時間・獲得(文字は左詰め・数字は右詰め) ----
	const float rowSpace = 36.0f;
	y = top - 204.0f;

	DrawTexL(GetTex(RecordsTex::KillLabel), textLeft, y);
	DrawNumUnitR(info.m_enemyKillCount, GetTex(RecordsTex::Body), numRight, y);
	y -= rowSpace;

	DrawTexL(GetTex(RecordsTex::TreasureLabel), textLeft, y);
	DrawNumUnitR(info.m_treasureChestsNum, GetTex(RecordsTex::Count), numRight, y);
	y -= rowSpace;

	DrawTexL(GetTex(RecordsTex::TimeLabel), textLeft, y);
	DrawTimeR(info.m_playTimeSc, numRight, y);
	y -= rowSpace;

	DrawTexL(GetTex(RecordsTex::GainLabel), textLeft, y);
	DrawNumUnitR(pResult->m_getMoney, GetTex(RecordsTex::Gold), numRight, y, true);

	DrawTex(GetTex(RecordsTex::DetailDivider), { textLeft, top - 336.0f }, m_pivotLeft);

	//---- 持ち込み(EXPLOREの装備・所持金と同じ項目) ----
	DrawTexL(GetTex(RecordsTex::CarryLabel), textLeft, top - 372.0f);

	const CarryInInfo& carryIn = pResult->m_carryIn;
	y = top - 410.0f;

	//武器(右端に名前、そのすぐ左にアイコン)
	DrawTexL(GetTex(RecordsTex::Weapon), textLeft, y);
	const int weapon = carryIn.m_weapon;
	if (weapon >= 0 && weapon < PlayerWeaponInfoSize)
	{
		const auto& spName = m_spWeaponNameTex[weapon];
		DrawTex(spName, { numRight, y }, m_pivotRight);
		const float iconRight = numRight - GetTexW(spName) - 8.0f;
		DrawTexSize(m_spWeaponIconTex[weapon], { iconRight, y }, m_detailIconSize, m_detailIconSize, m_pivotRight);
	}
	y -= rowSpace;

	//ポーション
	DrawTexL(GetTex(RecordsTex::Potion), textLeft, y);
	DrawNumUnitR(carryIn.m_potionNum, GetTex(RecordsTex::Count), numRight, y);
	y -= rowSpace;

	//所持金
	DrawTexL(GetTex(RecordsTex::MoneyLabel), textLeft, y);
	DrawNumUnitR(carryIn.m_money, GetTex(RecordsTex::Gold), numRight, y, true);
}
