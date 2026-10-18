#pragma once

class VOLInfo
{
public:

	~VOLInfo() { Save(); };

	void SetMSVol(float vol) { m_MSVol = vol; }
	void SetSEVol(float vol) { m_SEVol = vol; }
	void SetBGMVol(float vol) { m_BGMVol = vol; }


	float GetMSVol() { return m_MSVol; }
	float GetSEVol() { return m_SEVol; }
	float GetBGMVol() { return m_BGMVol; }

	float GetMSSEVol() { return m_SEVol* m_MSVol; }
	float GetMSBGMVol() { return m_BGMVol * m_MSVol; }



private:
	VOLInfo() { Init(); };

	void Init();
	void Save();

	float m_MSVol = 0;
	float m_SEVol = 0;
	float m_BGMVol = 0;


	std::string m_path = "Asset/Data/Vol/Volume.json";
public:

	// シングルトンパターン
	static VOLInfo& Instance()
	{
		static VOLInfo instance;
		return instance;
	}
};
