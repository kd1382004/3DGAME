#include "KeyInfo.h"

void KeyInfo::SetKeyValid(int _key)
{
	if (_key >= 'A' && _key <= 'Z')//アルファベット
	{
		int Num = _key - 'A';
		KeyAlphabet[Num].m_validFlg = true;
	}
	else if (_key >= VK_LEFT && _key <= VK_DOWN)//矢印
	{
		int Num = _key - VK_LEFT;
		KeyArrow[Num].m_validFlg = true;
	}
	else if (_key == VK_SPACE)//スペース
	{
		KeySpace.m_validFlg = true;
	}
	else if (_key == VK_RETURN) //ENTER
	{
		KeyEnter.m_validFlg = true;
	}
	else if (_key == VK_LBUTTON)//左クリック
	{
		Mousebutton[0].m_validFlg = true;
	}
	else if (_key == VK_RBUTTON)//右クリック
	{
		Mousebutton[1].m_validFlg = true;
	}
	else if (_key >= '0' && _key <= '9')
	{
		int Num = _key - '0';
		KeyNum[Num].m_validFlg = true;
	}
}

void KeyInfo::KeyvalidReset()
{

}

void KeyInfo::UpdateKey()
{
	//アルファベット
	for (int i = 0; i < AlphabetNum; i++)
	{
		if (!KeyAlphabet[i].m_validFlg) { continue; }

		int Num = 'A' + i;
		if (GetAsyncKeyState(Num) & 0x8000)
		{
			KeyAlphabet[i].m_pushFlg = true;

			if (KeyAlphabet[i].m_useFlg)
			{
				KeyAlphabet[i].m_pushS++;
			}
		}
		else
		{
			KeyAlphabet[i].m_useFlg = false;
			KeyAlphabet[i].m_pushFlg = false;
			KeyAlphabet[i].m_pushS = 0;
		}
	}

	//矢印
	for (int i = 0; i < ArrowNum; i++)
	{
		if (!KeyArrow[i].m_validFlg) { continue; }

		int Num = i;
		if (GetAsyncKeyState(Num) & 0x8000)
		{
			KeyArrow[i].m_pushFlg = true;

			if (KeyArrow[i].m_useFlg)
			{
				KeyArrow[i].m_pushS++;
			}
		}
		else
		{
			KeyArrow[i].m_useFlg = false;
			KeyArrow[i].m_pushFlg = false;
			KeyArrow[i].m_pushS = 0;
		}
	}


	if (KeySpace.m_validFlg)
	{
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			KeySpace.m_pushFlg = true;

			if (KeySpace.m_useFlg)
			{
				KeySpace.m_pushS++;
			}
		}
		else
		{
			KeySpace.m_useFlg = false;
			KeySpace.m_pushFlg = false;
			KeySpace.m_pushS = 0;
		}
	}	
}

bool KeyInfo::GetValidKeyPush(int _key, bool _useFlg, bool _flg)
{
	bool flg = false;

	if (_key >= 'A' && _key <= 'Z')//アルファベット
	{
		int Num = _key - 'A';
		flg = GetValidKeyPush(&KeyAlphabet[Num], _useFlg, _flg);
	}
	else if (_key >= VK_LEFT && _key <= VK_DOWN)//矢印
	{
		int Num = _key - VK_LEFT;
		flg = GetValidKeyPush(&KeyArrow[Num], _useFlg, _flg);
	}
	else if (_key == VK_SPACE)//スペース
	{
		flg = GetValidKeyPush(&KeySpace, _useFlg, _flg);
	}
	else if (_key == VK_RETURN) //ENTER
	{
		flg = GetValidKeyPush(&KeyEnter, _useFlg, _flg);
	}
	else if (_key == VK_LBUTTON)//左クリック
	{
		flg = GetValidKeyPush(&Mousebutton[0], _useFlg, _flg);
	}
	else if (_key == VK_RBUTTON)//右クリック
	{
		flg = GetValidKeyPush(&Mousebutton[1], _useFlg, _flg);
	}
	else if (_key >= '0' && _key <= '9')
	{
		int Num = _key - '0';
		flg = GetValidKeyPush(&KeyNum[Num], _useFlg, _flg);
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
			if (_key->m_pushS > m_pushMax)
			{
				_key->m_useFlg = false;
				_key->m_pushS = 0;
			} 
		}

	}



	return flg;
}

