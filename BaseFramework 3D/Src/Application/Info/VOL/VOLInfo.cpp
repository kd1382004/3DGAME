//---- 自分のヘッダー ----
#include "VOLInfo.h"		//音量設定

//=============================================
// 基本の処理
//=============================================

void VOLInfo::Init()
{
	std::ifstream ifs(m_path);
	if (!ifs.is_open()) return;

	nlohmann::json jsonData;
	ifs >> jsonData;

	m_MSVol = jsonData.value("MSVol", 1.0f);
	m_SEVol = jsonData.value("SEVol", 1.0f);
	m_BGMVol = jsonData.value("BGMVol", 1.0f);
}

//=============================================
// その他の処理
//=============================================

void VOLInfo::Save()
{
	nlohmann::json jsonData;

	jsonData["MSVol"] = m_MSVol;
	jsonData["SEVol"] = m_SEVol;
	jsonData["BGMVol"] = m_BGMVol;

	std::ofstream ofs(m_path);
	ofs << jsonData.dump(4);
}
