#include "NumDraw.h"
#include <string>
#include <vector>
#include <cmath>

void NumDraw::Drow(long _Num, Aligned _aligned, Math::Vector2 _pos, Math::Color _color, float _siz, bool _Separator, int _digit)
{
	if (!m_tex) return;

	// ドット絵フォントのにじみ・隣の文字の映り込み(縦線)を防ぐため、ポイントフィルタリング(Point_Clamp)で描画
	KdShaderManager::Instance().m_spriteShader.Begin(false);

	// 1. 数値を文字列化（桁数指定のパディング処理も含む）
	long num = std::abs(_Num);
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
		if (_Separator && distFromRight > 0 && distFromRight % 3 == 0)
		{
			glyphs.push_back(10); // 10番目のスプライトがカンマ ','
		}
	}

	// 3. 各文字の描画サイズと位置を計算して描画
	float charW = recX * _siz;
	float charH = recY * _siz;
	int totalGlyphs = static_cast<int>(glyphs.size());

	for (int k = 0; k < totalGlyphs; k++)
	{
		Math::Rectangle srcRect = { recX * glyphs[k], 0, recX, recY };
		Math::Vector2 drawPos = _pos;

		switch (_aligned)
		{
		case LAligned:
			// 左揃え：_pos から右方向へ順番に配置
			drawPos.x = _pos.x + k * charW;
			break;

		case RAligned:
			// 右揃え：_pos を右端として、左方向へ戻って配置
			drawPos.x = _pos.x - (totalGlyphs - 1 - k) * charW;
			break;

		default:
			break;
		}

		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_tex,
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
	m_tex = std::make_shared<KdTexture>();
	m_tex->Load("Asset/Textures/Num/pixel-letters-7-8x14_transparent.png");
	if (m_tex)
	{
		// 横99px / 11スプライト (0~9 + カンマ,) = 横幅9px
		recX = m_tex->GetInfo().Width / 11;
		recY = m_tex->GetInfo().Height;
	}
}

void NumDraw::Release()
{

}
