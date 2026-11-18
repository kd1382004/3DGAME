#pragma once

class VOLInfo
{
public:

	//---- コンストラクタ・デストラクタ ----

	//デストラクタ(音量を保存する)
	~VOLInfo() { Save(); };

	//---- 取得 ----

	//全体の音量を取得
	//戻り値 ... 音量(0〜1)
	float GetMSVol() { return m_MSVol; }

	//効果音の音量を取得
	//戻り値 ... 音量(0〜1)
	float GetSEVol() { return m_SEVol; }

	//BGMの音量を取得
	//戻り値 ... 音量(0〜1)
	float GetBGMVol() { return m_BGMVol; }

	//全体の音量をかけた効果音の音量を取得
	//戻り値 ... 実際に鳴らす効果音の音量
	float GetMSSEVol() { return m_SEVol* m_MSVol; }

	//全体の音量をかけたBGMの音量を取得
	//戻り値 ... 実際に鳴らすBGMの音量
	float GetMSBGMVol() { return m_BGMVol * m_MSVol; }

	//---- 設定 ----

	//全体の音量をセット
	// _vol ... 音量(0〜1)
	void SetMSVol(float _vol) { m_MSVol = _vol; }

	//効果音の音量をセット
	// _vol ... 音量(0〜1)
	void SetSEVol(float _vol) { m_SEVol = _vol; }

	//BGMの音量をセット
	// _vol ... 音量(0〜1)
	void SetBGMVol(float _vol) { m_BGMVol = _vol; }

	//---- シングルトン ----

	// シングルトンパターン
	static VOLInfo& Instance()
	{
		static VOLInfo instance;
		return instance;
	}

private:

	//---- メンバ変数 ----

	//全体の音量
	float m_MSVol = 0;

	//効果音の音量
	float m_SEVol = 0;

	//BGMの音量
	float m_BGMVol = 0;

	//音量を保存するファイルのパス
	std::string m_path = "Asset/Data/Vol/Volume.json";

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ(シングルトンなので外から作れない)
	VOLInfo() { Init(); };

	//---- 基本の処理 ----

	//初期化(音量をファイルから読み込む)
	void Init();

	//---- その他の処理 ----

	//音量をファイルに保存する
	void Save();
};
