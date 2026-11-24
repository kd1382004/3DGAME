//---- 自分のヘッダー ----
#include "KeyInfo.h"					//キー入力

//---- 便利機能 ----
#include"../MouseInfo/MouseInfo.h"		//マウス入力

//=============================================
// 基本の処理
//=============================================

void KeyInfo::Draw2Dkey(int _key, Math::Vector2 _2DPos, Math::Vector2 _size, float _alpha, Math::Vector2 _pivot)
{
	if (!m_spKeyTexture) { return; }
	Math::Color color = { 1,1,1,_alpha };
	Math::Rectangle srcRect;

	srcRect = GetKeySrcRect(_key);

	if (_key >= 'A' && _key <= 'Z')
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spKeyTexture, _2DPos.x, _2DPos.y, _size.x, _size.y, &srcRect, &color, _pivot);
		return;
	}


	switch (_key)
	{
	case VK_TAB:
	case VK_ESCAPE:
	case VK_SNAPSHOT:
	case VK_BACK:
	case VK_SHIFT:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_RETURN:
	case VK_CONTROL:
	case VK_MENU:
	case VK_SPACE:
	case VK_INSERT:
	case VK_DELETE:
	case VK_END:
	case VK_HOME:
	case VK_PAUSE:
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spKeyTexture2, _2DPos.x, _2DPos.y, _size.x, _size.y, &srcRect, &color, _pivot);
		return;
	}
}

//=============================================
// 取得
//=============================================

bool KeyInfo::GetValidKeyPush(int _key, bool _useFlg, bool _flg)
{
	bool flg = false;

	if (_key >= 'A' && _key <= 'Z')//アルファベット
	{
		int num = _key - 'A';
		flg = GetValidKeyPush(&m_keyAlphabet[num], _useFlg, _flg);
	}
	else if (_key >= VK_LEFT && _key <= VK_DOWN)//矢印
	{
		int num = _key - VK_LEFT;
		flg = GetValidKeyPush(&m_keyArrow[num], _useFlg, _flg);
	}
	else if (_key == VK_SPACE)//スペース
	{
		flg = GetValidKeyPush(&m_keySpace, _useFlg, _flg);
	}
	else if (_key == VK_RETURN) //ENTER
	{
		flg = GetValidKeyPush(&m_keyEnter, _useFlg, _flg);
	}
	else if (_key == VK_LBUTTON)//左クリック
	{
		flg = GetValidKeyPush(&m_mousebutton[0], _useFlg, _flg);
	}
	else if (_key == VK_RBUTTON)//右クリック
	{
		flg = GetValidKeyPush(&m_mousebutton[1], _useFlg, _flg);
	}
	else if (_key >= '0' && _key <= '9')
	{
		int num = _key - '0';
		flg = GetValidKeyPush(&m_keyNum[num], _useFlg, _flg);
	}
	else if (_key == VK_SHIFT)
	{
		int num = _key;
		flg = GetValidKeyPush(&m_shift, _useFlg, _flg);
	}
	else if (_key == VK_TAB)
	{
		int num = _key;
		flg = GetValidKeyPush(&m_keyTAB, _useFlg, _flg);
	}



	return  flg;
}

bool KeyInfo::GetValidKeyPush(Key* _key, bool _useFlg, bool _flg)
{
	bool flg = false;

	if (!_key->m_validFlg)
	{
		return flg;
	}

	//Keyが押されてるかどうか
	if (_key->m_pushFlg)
	{
		//そのKeyが使われたことがあるかどうか気にするか
		if (_useFlg)
		{
			//使用されたかどうかの反転を入れる
			flg = !_key->m_useFlg;
		}
		else
		{
			//気にしないならtrue
			flg = true;
		}

	}

	//もし使われたなら
	if (flg)
	{
		_key->m_useFlg = true;

		if (_useFlg && _flg)
		{
			if (_key->m_pushS > kPushMax)
			{
				_key->m_useFlg = false;
				_key->m_pushS = 0;
			}
		}

	}



	return flg;
}

//=============================================
// 設定
//=============================================

void KeyInfo::SetKeyValid(int _key)
{
	if (_key >= 'A' && _key <= 'Z')//アルファベット
	{
		int num = _key - 'A';
		m_keyAlphabet[num].m_validFlg = true;
	}
	else if (_key >= VK_LEFT && _key <= VK_DOWN)//矢印
	{
		int num = _key - VK_LEFT;
		m_keyArrow[num].m_validFlg = true;
	}
	else if (_key == VK_SPACE)//スペース
	{
		m_keySpace.m_validFlg = true;
	}
	else if (_key == VK_RETURN) //ENTER
	{
		m_keyEnter.m_validFlg = true;
	}
	else if (_key == VK_LBUTTON)//左クリック
	{
		m_mousebutton[0].m_validFlg = true;
	}
	else if (_key == VK_RBUTTON)//右クリック
	{
		m_mousebutton[1].m_validFlg = true;
	}
	else if (_key >= '0' && _key <= '9')
	{
		int num = _key - '0';
		m_keyNum[num].m_validFlg = true;
	}
	else if (_key == VK_SHIFT) //シフト
	{
		m_shift.m_validFlg = true;
	}
	else if (_key == VK_TAB)//TAB
	{
		m_keyTAB.m_validFlg = true;
	}
}

//=============================================
// その他の処理
//=============================================

void KeyInfo::KeyvalidReset()
{

}

void KeyInfo::UpdateKey()
{
	//アルファベット
	for (int i = 0; i < kAlphabetNum; i++)
	{
		if (!m_keyAlphabet[i].m_validFlg) { continue; }

		int num = 'A' + i;
		if (GetAsyncKeyState(num) & 0x8000)
		{
			m_keyAlphabet[i].m_pushFlg = true;

			if (m_keyAlphabet[i].m_useFlg)
			{
				m_keyAlphabet[i].m_pushS++;
			}
		}
		else
		{
			m_keyAlphabet[i].m_useFlg = false;
			m_keyAlphabet[i].m_pushFlg = false;
			m_keyAlphabet[i].m_pushS = 0;
		}
	}

	//矢印
	for (int i = 0; i < kArrowNum; i++)
	{
		if (!m_keyArrow[i].m_validFlg) { continue; }

		int num = i;
		if (GetAsyncKeyState(num) & 0x8000)
		{
			m_keyArrow[i].m_pushFlg = true;

			if (m_keyArrow[i].m_useFlg)
			{
				m_keyArrow[i].m_pushS++;
			}
		}
		else
		{
			m_keyArrow[i].m_useFlg = false;
			m_keyArrow[i].m_pushFlg = false;
			m_keyArrow[i].m_pushS = 0;
		}
	}

	//スペース
	if (m_keySpace.m_validFlg)
	{
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			m_keySpace.m_pushFlg = true;

			if (m_keySpace.m_useFlg)
			{
				m_keySpace.m_pushS++;
			}
		}
		else
		{
			m_keySpace.m_useFlg = false;
			m_keySpace.m_pushFlg = false;
			m_keySpace.m_pushS = 0;
		}
	}

	//マウスボタン
	for (int i = 0; i < kMousebuttonNum; i++)
	{
		if (!m_mousebutton[i].m_validFlg) { continue; }

		int key = VK_LBUTTON + i;

		if (GetAsyncKeyState(key) & 0x8000)
		{
			if (key == VK_LBUTTON && !m_mousebutton[i].m_pushFlg && MouseInfo::Instance().GetMouseFreeFlg())
			{
				MouseInfo::Instance().SEMouseClickPlay();
			}



			m_mousebutton[i].m_pushFlg = true;

			if (m_mousebutton[i].m_useFlg)
			{
				m_mousebutton[i].m_pushS++;
			}
		}
		else
		{
			m_mousebutton[i].m_useFlg = false;
			m_mousebutton[i].m_pushFlg = false;
			m_mousebutton[i].m_pushS = 0;
		}
	}


	//シフト
	if (m_shift.m_validFlg)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			m_shift.m_pushFlg = true;

			if (m_shift.m_useFlg)
			{
				m_shift.m_pushS++;
			}
		}
		else
		{
			m_shift.m_useFlg = false;
			m_shift.m_pushFlg = false;
			m_shift.m_pushS = 0;
		}
	}


	if (m_keyTAB.m_validFlg)
	{
		if (GetAsyncKeyState(VK_TAB) & 0x8000)
		{
			m_keyTAB.m_pushFlg = true;

			if (m_keyTAB.m_useFlg)
			{
				m_keyTAB.m_pushS++;
			}
		}
		else
		{
			m_keyTAB.m_useFlg = false;
			m_keyTAB.m_pushFlg = false;
			m_keyTAB.m_pushS = 0;
		}
	}
}

//=============================================
// 基本の処理
//=============================================

void KeyInfo::Init()
{
	if (!m_spKeyTexture)
	{
		m_spKeyTexture = std::make_shared<KdTexture>();
		m_spKeyTexture->Load("Asset/Textures/Key/Keyboard Letters and Symbols.png");


		m_keyTexSize.x = m_spKeyTexture->GetWidth() / kAlphabetNum;
		m_keyTexSize.y = m_spKeyTexture->GetHeight();
	}

	if (!m_spKeyTexture2)
	{
		m_spKeyTexture2 = std::make_shared<KdTexture>();
		m_spKeyTexture2->Load("Asset/Textures/Key/Keyboard Extras.png");

		m_keyTex2Size.x = m_spKeyTexture2->GetWidth() / 4;
		m_keyTex2Size.y = m_spKeyTexture2->GetHeight() / 4;

	}
}

//=============================================
// 取得
//=============================================

Math::Rectangle KeyInfo::GetKeySrcRect(int _key)
{
	Math::Rectangle srcRect;


	//アルファベット
	if (_key >= 'A' && _key <= 'Z')
	{
		int num = _key - 'A';
		srcRect = {
			(long)m_keyTexSize.x * num,
			0,
			(long)m_keyTexSize.x,
			(long)m_keyTexSize.y
		};
	}

	switch (_key)
	{
	case VK_TAB:
		srcRect = { (long)m_keyTex2Size.x * 0, (long)m_keyTex2Size.y * 0, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;

	case VK_ESCAPE:
		srcRect = { (long)m_keyTex2Size.x * 1, (long)m_keyTex2Size.y * 0, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;

	case VK_SNAPSHOT:
		srcRect = { (long)m_keyTex2Size.x * 2, (long)m_keyTex2Size.y * 0, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;

	case VK_BACK:
		srcRect = { (long)m_keyTex2Size.x * 3, (long)m_keyTex2Size.y * 0, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;

	case VK_SHIFT:
		srcRect = { (long)m_keyTex2Size.x * 0, (long)m_keyTex2Size.y * 1, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;

	case VK_PRIOR:
		srcRect = { (long)m_keyTex2Size.x * 1, (long)m_keyTex2Size.y * 1, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;
	
	case VK_NEXT:
		srcRect = { (long)m_keyTex2Size.x * 2, (long)m_keyTex2Size.y * 1, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;
	
	case VK_RETURN:
		srcRect = { (long)m_keyTex2Size.x * 3, (long)m_keyTex2Size.y * 1, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;

	case VK_CONTROL:
		srcRect = { (long)m_keyTex2Size.x * 0, (long)m_keyTex2Size.y * 2, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;

	case VK_MENU:
		srcRect = { (long)m_keyTex2Size.x * 1, (long)m_keyTex2Size.y * 2, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;

	case VK_SPACE:
		srcRect = { (long)m_keyTex2Size.x * 2, (long)m_keyTex2Size.y * 2, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;

	case VK_INSERT:   srcRect = { (long)m_keyTex2Size.x * 3, (long)m_keyTex2Size.y * 2, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;

	case VK_DELETE:
		srcRect = { (long)m_keyTex2Size.x * 0, (long)m_keyTex2Size.y * 3, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;

	case VK_END:
		srcRect = { (long)m_keyTex2Size.x * 1, (long)m_keyTex2Size.y * 3, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;

	case VK_HOME:
		srcRect = { (long)m_keyTex2Size.x * 2, (long)m_keyTex2Size.y * 3, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;

	case VK_PAUSE:
		srcRect = { (long)m_keyTex2Size.x * 3, (long)m_keyTex2Size.y * 3, (long)m_keyTex2Size.x, (long)m_keyTex2Size.y };
		break;
	}


	return srcRect;
}
