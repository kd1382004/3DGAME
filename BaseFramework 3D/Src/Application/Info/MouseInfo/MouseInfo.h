#pragma once

class MouseInfo
{
public:
	~MouseInfo();

	//マウスの状態
	enum MouseType
	{
		//何にもあったってない(デフォルト)
		MouseType_Nomal,	

		//何かに当たってる(Clickで何かがおこる)
		MouseType_HIT,	

		//クリックされてる
		MouseType_Click,

		//マウスがフリーじゃない(例:カメラを動かすために固定している)
		MouseType_NotFree,
	};


	void Update();
	void DrawSprite();

	bool GetMouseFreeFlg() { return m_mouseFreeFlg; }

	void SetMouseFreeFlg(bool _free) 
	{ 
		m_mouseFreeFlg = _free; 
	
		if (m_mouseFreeFlg)
		{
			m_mouseType = MouseType::MouseType_Nomal;

		}
		else
		{
			m_mouseType = MouseType::MouseType_NotFree;
		}
	}

	POINT m_pos = {};

	POINT GetFixMousePos() const { return m_fixMousePos; }

	void SetMouseType(MouseType _type) { m_mouseType = _type; }

	void SetMousePosFixMousePos();

	bool GetFocusWindowFlg() const { return m_FocusWindowFlg; }
private:

	void Init();

	MouseInfo() { Init(); };

	//マウスをゲーム内においてフリーにするか
	bool m_mouseFreeFlg = true;

	// カメラ回転用マウス座標の差分
	POINT m_fixMousePos{};

	//マウスの状態管理
	MouseType m_mouseType = MouseType::MouseType_Nomal;

	//ウィンドウ単位でのフォーカスがあるかどうか
	bool m_FocusWindowFlg = false;
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

