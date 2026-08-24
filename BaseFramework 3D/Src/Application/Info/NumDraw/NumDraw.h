#pragma once

enum Aligned
{
	LAligned, //左揃え
	RAligned, //右揃え
};

class NumDraw
{
public:

	~NumDraw() { Release(); };

	//引数
	//long Num...表示したい数字(整数)
	//Aligned aligned...どちらに揃えるか(LAligned : RAligned)
	//Math::Vector2 pos...alignedで揃えた方から始めるスタート座標
	// color...色
	//float siz...サイズ
	//bool Separator..3桁区入りを入れるかどうか
	//digit..表示させる桁数(-1の場合そのままの桁) 
	void Drow(long Num, Aligned aligned, Math::Vector2 pos, Math::Color color = Math::Color(1, 1, 1, 1), float siz = 1, bool Separator = false, int digit = -1);

private:

	void Init();
	void Release();
	NumDraw() { Init(); }


	std::shared_ptr<KdTexture> m_tex;
	//数字直径横
	int recX = 9;
	//数字直径縦
	int recY = 14;

public:
	static NumDraw& GetInstance()
	{
		static NumDraw instance;
		return instance;
	}
};

