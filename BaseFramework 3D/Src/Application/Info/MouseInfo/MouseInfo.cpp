//---- 自分のヘッダー ----
#include"MouseInfo.h"				//マウス入力

//---- アプリケーション ----
#include"../../main.h"				//アプリケーション本体(ウィンドウ・FPS管理)

//---- 便利機能 ----
#include"../KeyInfo/KeyInfo.h"		//キー入力

//=============================================
// コンストラクタ・デストラクタ
//=============================================

MouseInfo::~MouseInfo()
{

}

//=============================================
// 基本の処理
//=============================================

void MouseInfo::Update()
{
	//************************************************
	//ウィンドウ単位でのフォーカスがあるかどうか
	m_focusWindowFlg = (GetForegroundWindow() == Application::Instance().GetWindowHandle());

	POINT mousePos;
	GetCursorPos(&mousePos);

	//指定のウィンドウ基準のマウス座標に変換(実行画面の左上が(0,0))
	ScreenToClient(Application::Instance().GetWindowHandle(), &mousePos);


	if (mousePos.x > -m_fixMousePos.x && mousePos.x < m_fixMousePos.x &&
		mousePos.y > -m_fixMousePos.y && mousePos.y < m_fixMousePos.y)
	{
		if (!m_focusWindowFlg)
		{
			return;
		}

	}

	//************************************************

	//************************************************
	//デバック用のマウスフリー切り替え
	m_mouseOldType = m_mouseType;

	if (m_mouseFreeFlg)
	{
		m_mouseType = MouseType::MouseType_Normal;

	}
	else
	{
		m_mouseType = MouseType::MouseType_NotFree;
	}

	//************************************************



	//************************************************
	//マウスがフリーならウィンドウ基準のマウス座標に変換

	if (m_mouseFreeFlg)
	{
		//ディスプレイ状のマウス座標を取得(pc画面の左上が(0,0))
		GetCursorPos(&m_windowPos);

		//指定のウィンドウ基準のマウス座標に変換(実行画面の左上が(0,0))
		ScreenToClient(Application::Instance().GetWindowHandle(), &m_windowPos);


		//マウスの座標系を実行ウィンドウの座標(中心が(0,0)に補正)
		m_windowPos.x -= m_fixMousePos.x;
		m_windowPos.y -= m_fixMousePos.y;
		m_windowPos.y *= -1;
	}
	//************************************************
}

void MouseInfo::DrawSprite()
{
	if (!m_spMousePointer) { return; }

	// NotFree のときは描かない
	if (m_mouseType == MouseType::MouseType_NotFree) { return; }

	// 1マスの大きさ（画像の高さと同じ正方形）
	const long cell = (long)m_spMousePointer->GetHeight();

	// enum の番号 × 1マスの幅で切り出す（画像は enum と同じ順に並んでいる）
	Math::Rectangle rec = { (long)m_mouseType * cell, 0, cell, cell };

	Math::Vector2 piv = { 0,1 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spMousePointer, m_windowPos.x, m_windowPos.y, 24, 24, &rec, nullptr, piv);

}

//=============================================
// 取得
//=============================================

POINT MouseInfo::GetWindowMousePos()
{

	POINT pos;
	pos.x = m_fixMousePos.x;
	pos.y = m_fixMousePos.y;
	ClientToScreen(Application::Instance().GetWindowHandle(), &pos);

	return pos;
}

int MouseInfo::GetMouseWheelValue() const
{
	return Application::Instance().GetMouseWheelValue();
}

//=============================================
// 設定
//=============================================

void MouseInfo::SetMouseFreeFlg(bool _free)
{
	m_mouseFreeFlg = _free;
	
	if (!m_mouseFreeFlg)
	{
		//カーソルも中心に戻す(戻したあとカメラが一気に回らないように)
		SetMousePosFixMousePos();
	}
}

void MouseInfo::SetMouseType(MouseType _type)
{

	//ボタンなどに乗った瞬間だけ鳴らす
	if (_type == MouseType::MouseType_HIT &&
		m_mouseType != MouseType::MouseType_HIT &&
		m_mouseOldType != MouseType::MouseType_HIT)
	{
		if (m_spHitSE)
		{
			m_spHitSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
			m_spHitSE->Play();
		}
	}

	m_mouseType = _type;


	m_mouseType = _type;
}

void MouseInfo::SetMousePosFixMousePos()
{
	if (!m_focusWindowFlg)
	{
		return;
	}

	POINT pos;
	pos.x = m_fixMousePos.x;
	pos.y = m_fixMousePos.y;
	if (ClientToScreen(Application::Instance().GetWindowHandle(), &pos))
	{
		SetCursorPos(pos.x, pos.y);
	}
}

//=============================================
// その他の処理
//=============================================

void MouseInfo::SEMouseClickPlay()
{
	if (m_spClickSE)
	{
		m_spClickSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
		m_spClickSE->Play();
	}
}

//=============================================
// 基本の処理
//=============================================

void MouseInfo::Init()
{
	// ↓ウィンドウ中央座標
	m_fixMousePos.x = 640;
	m_fixMousePos.y = 360;


	//マウスをウィンドウの中央に設定
	POINT pos;
	pos.x = m_fixMousePos.x;
	pos.y = m_fixMousePos.y;
	if (ClientToScreen(Application::Instance().GetWindowHandle(), &pos))
	{
		SetCursorPos(pos.x, pos.y);
	}


	if (!m_spMousePointer)
	{
		m_spMousePointer = std::make_shared<KdTexture>();
		m_spMousePointer->Load("Asset/Textures/Mouse/MousePointer.png");
	}

	if (!m_spClickSE)
	{
		m_spClickSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Mouse/click.wav", false);
		if (m_spClickSE)
		{
			m_spClickSE->Stop();
			m_spClickSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
		}
	}
	if (!m_spHitSE)
	{
		m_spHitSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Mouse/Hit.wav", false);
		if (m_spHitSE)
		{
			m_spHitSE->Stop();
			m_spHitSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
		}
	}
}
