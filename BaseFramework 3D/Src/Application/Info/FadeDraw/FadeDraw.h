#pragma once


enum FadeType
{
	FedeIn,
	FedeOut,
	NoFede,
};

class FadeDraw
{
public:
	~FadeDraw() {};
	
	void Update();
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

	bool GetFadeFlg() { return m_fadeFlg; }

private:

	void Init();


	std::shared_ptr<KdTexture>m_tex;

	FadeType m_fadeType = NoFede;

	bool m_fadeFlg = false;
	bool m_fadeFineFlg = false;

	Math::Color m_color;

	float m_time;

	float m_nowTime;


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
