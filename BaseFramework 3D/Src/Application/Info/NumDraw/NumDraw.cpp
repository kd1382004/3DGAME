#include "NumDraw.h"
#include <string>
#include <vector>
#include <cmath>

void NumDraw::Draw(long _num, Aligned _aligned, Math::Vector2 _pos, Math::Color _color, float _siz, bool _separator, int _digit, Math::Vector2 _maxSiz)
{
	if (!m_spTex) return;

	// ドット絵フォントのにじみ・隣の文字の映り込み(縦線)を防ぐため、ポイントフィルタリング(Point_Clamp)で描画
	KdShaderManager::Instance().m_spriteShader.Begin(false);

	// 1. 数値を文字列化（桁数指定のパディング処理も含む）
	long num = std::abs(_num);
	std::string numStr = std::to_string(num);

	// _digitが指定されていれば、その桁数になるように先頭を'0'で埋める
	if (_digit > (int)numStr.size())
	{
		numStr = std::string(_digit - numStr.size(), '0') + numStr;
	}

	// 2. 描画するスプライトインデックス（0〜9: 数字, 10: カンマ）の配列を作成
	std::vector<int> glyphs;
	int numLen = static_cast<int>(numStr.size());

	for (int i = 0; i < numLen; i++)
	{
		int digit = numStr[i] - '0';
		glyphs.push_back(digit);

		// 3桁区切りのカンマ判定 (右からの桁数位置)
		int distFromRight = numLen - 1 - i;
		if (_separator && distFromRight > 0 && distFromRight % 3 == 0)
		{
			glyphs.push_back(10); // 10番目のスプライトがカンマ ','
		}
	}

	// 3. 各文字の描画サイズと位置を計算して描画
	float charW = m_recX * _siz;
	float charH = m_recY * _siz;
	int totalGlyphs = static_cast<int>(glyphs.size());

	// 超えてはいけない大きさがあれば、横・縦それぞれの倍率を下げて収める
	// 横 … 全部の文字を並べた幅が _maxSiz.x を超えたら、1文字の幅を縮める
	if (_maxSiz.x > 0.0f && totalGlyphs > 0 && charW * totalGlyphs > _maxSiz.x)
	{
		charW = _maxSiz.x / totalGlyphs;
	}
	// 縦 … 1文字の高さが _maxSiz.y を超えたら、高さを縮める
	if (_maxSiz.y > 0.0f && charH > _maxSiz.y)
	{
		charH = _maxSiz.y;
	}

	for (int k = 0; k < totalGlyphs; k++)
	{
		Math::Rectangle srcRect = { m_recX * glyphs[k], 0, m_recX, m_recY };
		Math::Vector2 drawPos = _pos;

		switch (_aligned)
		{
		case LAligned:
			// 左揃え：_pos から右方向へ順番に配置（_pos.x が左端）
			drawPos.x = _pos.x + (k + 0.5f) * charW;
			break;

		case RAligned:
			// 右揃え：_pos を右端として、左方向へ戻って配置（_pos.x が右端）
			drawPos.x = _pos.x - (totalGlyphs - k - 0.5f) * charW;
			break;

		case CAligned:
			// 中央揃え：_pos を全体の中央とする
			drawPos.x = _pos.x + (k - totalGlyphs / 2.0f + 0.5f) * charW;
			break;

		default:
			break;
		}


		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_spTex,
			(int)drawPos.x,
			(int)drawPos.y,
			(int)charW,
			(int)charH,
			&srcRect,
			&_color
		);
	}

	// 描画終了
	KdShaderManager::Instance().m_spriteShader.End();
}

void NumDraw::Init()
{
	m_spTex = std::make_shared<KdTexture>();
	m_spTex->Load("Asset/Textures/Num/pixel-letters-7-8x14_transparent.png");
	if (m_spTex)
	{
		// 横99px / 11スプライト (0~9 + カンマ,) = 横幅9px
		m_recX = m_spTex->GetInfo().Width / 11;
		m_recY = m_spTex->GetInfo().Height;
	}
}

void NumDraw::Release()
{

}
