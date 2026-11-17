#pragma once

enum Aligned
{
	LAligned, //左揃え
	RAligned, //右揃え
	CAligned, //中央揃え
};

class NumDraw
{
public:

	//デストラクタ
	~NumDraw() { Release(); };

	//数字を画像で描く
	// _num ... 表示したい数字(整数)
	// _aligned ... どちらに揃えるか(LAligned:左揃え / RAligned:右揃え / CAligned:中央揃え)
	// _pos ... _alignedで揃えた側の端の座標(上下は真ん中)
	// _color ... 色
	// _siz ... 大きさの倍率(元の画像は1文字9×14)
	// _separator ... 3桁ごとにカンマを入れるか
	// _digit ... 表示する桁数(足りない分は0で埋める。-1ならそのままの桁)
	// _maxSiz ... 超えてはいけない大きさ(横・縦。0なら制限なし。超えるときは倍率を下げて収める)
	void Draw(long _num, Aligned _aligned, Math::Vector2 _pos, Math::Color _color = Math::Color(1, 1, 1, 1), float _siz = 1, bool _separator = false, int _digit = -1, Math::Vector2 _maxSiz = { 0, 0 });

private:

	//初期化(数字の画像の読み込み)
	void Init();
	//解放
	void Release();
	//コンストラクタ(シングルトンなので外から作れない)
	NumDraw() { Init(); }


	//数字(0〜9とカンマ)が並んだ画像
	std::shared_ptr<KdTexture> m_spTex;
	//数字直径横
	int m_recX = 9;
	//数字直径縦
	int m_recY = 14;

public:
	//インスタンスを取得する(シングルトン)
	//戻り値 ... NumDrawのインスタンス
	static NumDraw& GetInstance()
	{
		static NumDraw instance;
		return instance;
	}
};

