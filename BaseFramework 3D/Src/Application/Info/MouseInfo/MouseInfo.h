#pragma once

class MouseInfo
{
public:
	~MouseInfo();



	void Update();
	void DrawSprite();

	bool GetMouseFreeFlg() { return m_mouseFreeFlg; }
	void SetMouseFreeFlg(bool _free) { m_mouseFreeFlg = _free; }

	POINT m_pos = {};

	POINT GetFixMousePos() const { return m_fixMousePos; }
private:

	void Init();

	MouseInfo() { Init(); };

	//マウスをフリーにするか
	bool m_mouseFreeFlg = true;

	// カメラ回転用マウス座標の差分
	POINT m_fixMousePos{};
public:

	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static MouseInfo& Instance()
	{
		static MouseInfo instance;
		return instance;
	}
};

