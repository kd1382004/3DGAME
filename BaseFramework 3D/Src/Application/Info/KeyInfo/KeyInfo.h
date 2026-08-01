#pragma once

struct Key
{
	//このKeyが有効かどうか
	bool m_validFlg = false;

	//押されてるかどうか
	bool m_pushFlg = false;

	//使われてからどのくらいか
	int m_pushS = 0;

	//使用されたかどうか(長押し防止)
	bool m_useFlg = false;
};

class KeyInfo
{
public:
	~KeyInfo() {};

	//keyを使用できるかどうかの設定
	void SetKeyValid(int _key);

	void KeyvalidReset();


	void UpdateKey();


	//2D描画
	// _key ... Key番号
	// _2DPos ... 2D座標
	// _siz ... サイズ
	// _alpha ... 不透明度
	// _pivot ... ピボット
	void Draw2Dkey(int _key, Math::Vector2 _2DPos, Math::Vector2 _siz, float _alpha = 1, Math::Vector2 _pivot = { 0.5,0.5 });

	//このフレーム中に有効なKeyが押されたかどうか
	// _key ... Key番号
	// _useFlg ...使われてるかどうかを気にする(true)
	// _flg 
	bool GetValidKeyPush(int _key, bool _useFlg = false, bool _flg = false);
private:

	bool GetValidKeyPush(Key* _key, bool _useFlg = false, bool _flg = false);

	void Init();

	//大文字アルファベット
	static const int AlphabetNum = 26;
	Key KeyAlphabet[AlphabetNum];

	//数字
	static const int Num = 10;
	Key KeyNum[Num];

	//矢印
	static const int ArrowNum = 4;
	Key KeyArrow[ArrowNum];

	//スペースキー
	Key KeySpace;

	//エンターキー
	Key KeyEnter;

	//マウス
	Key Mousebutton[2];


	static const int m_pushMax = 20;


	//////////////////////////////////////////
	//Key描画用
	std::shared_ptr<KdTexture> m_spKeyTexture;

	//Key1つのテクスチャサイズ
	Math::Vector2 m_keyTexSiz;


	Math::Rectangle GetKeySrcRect(int _key);
private:

	KeyInfo() { Init(); };



public:

	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static KeyInfo& Instance()
	{
		static KeyInfo instance;
		return instance;
	}
};

