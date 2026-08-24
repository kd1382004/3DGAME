#include "NumDraw.h"

void NumDraw::Drow(long _Num, Aligned _aligned, Math::Vector2 _pos, Math::Color _color, float _siz, bool _Separator,int _digit)
{
	//10以上なら桁数を求める
	{
		//桁数を入れるワークspace
		long w = 0;

		if (_Num >= 10)
		{
			//桁数を求める
			for (long long i = 1; i <= _Num; i *= 10)
			{
				w++;
			}
		}
		else
		{
			w = 1;
		}


		//分解した一桁が入る
		std::vector<int>w_Num;

		if (_digit - w > 0)
		{
			int dig = _digit - w;

			for (int i = 0; i < dig; i++)
			{
				w_Num.push_back(0);
			}
		}

		if (_Num < 10)
		{
			w_Num.push_back(_Num);
		}
		else
		{
			for (int i = 0; i < w; i++)
			{
				int a = 1;

				for (int j = i; j < w - 1; j++)
				{
					a *= 10;
				}

				w_Num.push_back(_Num / a);
				_Num %= a;
			}
		}



		//スタート位置がどっちかを選別
		switch (_aligned)
		{
		case LAligned:
			break;
		case RAligned:
			std::reverse(w_Num.begin(), w_Num.end());
			break;
		default:
			break;
		}

		Math::Vector2 pos;
		for (int i = 0; i < w_Num.size(); i++)
		{
			Math::Rectangle rec1 = { 1 + recX * w_Num[i],0,recX ,recY };
			switch (_aligned)
			{
			case LAligned:

				pos = { _pos.x + i * (recX * _siz) - ((recX * _siz) / 2 * i) + (i * 5 * _siz) , _pos.y };

				KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, pos.x, pos.y, recX * _siz, recY * _siz, &rec1, &_color);
				if (_Separator)
				{
					if ((w_Num.size() - i) % 3 == 1 && w_Num.size() - i >= 4)
					{
						rec1 = Math::Rectangle{ 1 + recX * 10,0,recX ,recY };
						pos.x += recX * _siz / 2;
						KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, pos.x, pos.y, recX * _siz, recY * _siz, &rec1, &_color);
					}
				}

				break;
			case RAligned:

				pos = { _pos.x - i * (recX * _siz) + ((recX * _siz) / 2 * i) - (i * 5 * _siz), _pos.y };

				if (_Separator)
				{
					if (i % 3 == 0 && i != 0)
					{
						float x = pos.x + recX * _siz / 2;
						pos = { x, pos.y };
						rec1 = Math::Rectangle{ 1 + recX * 10,0,recX ,recY };
						KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, pos.x, pos.y, recX * _siz, recY * _siz, &rec1, &_color);
						pos.x -= (recX * _siz) / 2;
					}
				}

				pos = { _pos.x - i * (recX * _siz) + ((recX * _siz) / 2 * i) - (i * 5 * _siz), _pos.y };
				rec1 = { 1 + recX * w_Num[i],0,recX ,recY };
				KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, pos.x, pos.y, recX * _siz, recY * _siz, &rec1, &_color);
				break;
			default:
				break;
			}
		}
	}
}

void NumDraw::Init()
{
	m_tex = std::make_shared<KdTexture>();
	m_tex->Load("Asset/Textures/Num/pixel-letters-7-8x14_transparent.png");
}

void NumDraw::Release()
{

}