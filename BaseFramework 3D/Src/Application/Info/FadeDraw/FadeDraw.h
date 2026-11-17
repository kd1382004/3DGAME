#pragma once


enum FadeType
{
	FadeIn,
	FadeOut,
	NoFade,
};

class FadeDraw
{
public:
	//デストラクタ
	~FadeDraw() {};
	
	//フェードを進める
	void Update();
	//フェードの画像を画面全体に描く
	void DrawSprite();

	//_type  ... フェードの種類
	//_time  ... ↑を何秒で終わらせるか
	//_color ...  フェードの色
	void SetFadeType(FadeType _type, float _time, Math::Color _color = { 0,0,0 })
	{
		m_fadeType = _type;
		m_fadeFlg = true;
		m_time = _time;
		m_nowTime = 0.0f;
	}

	//フェード中か
	//戻り値 ... フェード中ならtrue
	bool GetFadeFlg() { return m_fadeFlg; }

private:

	//初期化(画像の読み込み)
	void Init();


	//画面全体に描くフェード用の画像
	std::shared_ptr<KdTexture>m_spTex;

	//今のフェードの種類
	FadeType m_fadeType = NoFade;

	//フェード中か
	bool m_fadeFlg = false;
	//フェードが終わったか
	bool m_fadeFineFlg = false;

	//フェードの色
	Math::Color m_color;

	//フェードにかける時間(秒)
	float m_time;

	//フェードを始めてからの時間(秒)
	float m_nowTime;


	//コンストラクタ(シングルトンなので外から作れない)
	FadeDraw() { Init(); };
public:

	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static FadeDraw& Instance()
	{
		static FadeDraw instance;
		return instance;
	}
};
