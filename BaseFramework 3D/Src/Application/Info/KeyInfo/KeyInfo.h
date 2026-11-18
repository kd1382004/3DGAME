#pragma once

//キー1つ分の入力の状態
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

	//---- コンストラクタ・デストラクタ ----

	//デストラクタ
	~KeyInfo() {};

	//---- 基本の処理 ----

	//2D描画
	// _key ... Key番号
	// _2DPos ... 2D座標
	// _siz ... サイズ
	// _alpha ... 不透明度
	// _pivot ... ピボット
	void Draw2Dkey(int _key, Math::Vector2 _2DPos, Math::Vector2 _siz, float _alpha = 1, Math::Vector2 _pivot = { 0.5,0.5 });

	//---- 取得 ----

	//このフレーム中に有効なKeyが押されたかどうか
	// _key ... Key番号
	// _useFlg ...使われてるかどうかを気にする(true)
	// _flg
	bool GetValidKeyPush(int _key, bool _useFlg = false, bool _flg = false);

	//---- 設定 ----

	//keyを使用できるかどうかの設定
	// _key ... キー番号('A'、VK_LEFTなど)
	void SetKeyValid(int _key);

	//---- その他の処理 ----

	//全部のキーを使えない状態に戻す
	void KeyvalidReset();

	//キーの状態を更新する(毎フレーム呼ぶ)
	void UpdateKey();

	//---- シングルトン ----

	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static KeyInfo& Instance()
	{
		static KeyInfo instance;
		return instance;
	}

private:

	//---- 定数 ----

	//大文字アルファベット
	static const int kAlphabetNum = 26;

	//数字
	static const int kNum = 10;

	//矢印
	static const int kArrowNum = 4;

	//マウス
	static const int kMousebuttonNum = 2;

	//押しっぱなしで使えるようになるまでのフレーム数
	static const int kPushMax = 20;

	//---- メンバ変数 ----

	//アルファベットのキーの状態('A'〜'Z')
	Key m_keyAlphabet[kAlphabetNum];

	//数字のキーの状態('0'〜'9')
	Key m_keyNum[kNum];

	//矢印のキーの状態
	Key m_keyArrow[kArrowNum];

	//スペースキー
	Key m_keySpace;

	//タブ
	Key m_keyTAB;

	//エンターキー
	Key m_keyEnter;

	//マウスボタンの状態(左・右)
	Key m_mousebutton[kMousebuttonNum];

	//シフト
	Key m_shift;

	//Key描画用
	std::shared_ptr<KdTexture> m_spKeyTexture;

	//矢印などの特殊キーの画像
	std::shared_ptr<KdTexture> m_spKeyTexture2;

	//Key1つのテクスチャサイズ
	Math::Vector2 m_keyTexSiz;

	//特殊キーの画像1つ分のサイズ
	Math::Vector2 m_keyTex2Siz;

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ(シングルトンなので外から作れない)
	KeyInfo() { Init(); };

	//---- 基本の処理 ----

	//初期化(画像の読み込み)
	void Init();

	//---- 取得 ----

	//このフレーム中にキーが押されたかどうか(Keyを直接渡す版)
	// _key ... 調べるキー
	// _useFlg ... 使われたかどうかを気にする(true:押しっぱなしでは反応しない)
	// _flg ... 押された瞬間に使ったことにするか
	//戻り値 ... 押されたらtrue
	bool GetValidKeyPush(Key* _key, bool _useFlg = false, bool _flg = false);

	//キー画像の中で、そのキーが描かれている範囲を返す
	// _key ... キー番号
	//戻り値 ... 画像の切り取り範囲
	Math::Rectangle GetKeySrcRect(int _key);
};

