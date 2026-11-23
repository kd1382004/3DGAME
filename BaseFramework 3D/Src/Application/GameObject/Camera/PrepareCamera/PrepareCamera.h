#pragma once

#include"../CameraBase.h"
#include"../../Terrains/Camp/CampLayoutData/CampLayoutData.h"

//////////////////////////////////////////////////////////////
// 準備画面のカメラ
// CampLayout.json のカメラ(Explore / Equip / Shop / Records)の位置へ、
// タブを切り替えるたびになめらかに動く
//////////////////////////////////////////////////////////////
class PrepareCamera : public CameraBase
{
public:

	//---- 基本の処理 ----

	//更新(行き先へ少しずつ動かす)
	void Update() override;

	//---- 設定 ----

	//JSONのカメラを入れる
	// _cams ... 名前("Explore"など)とカメラの値
	void SetCameraData(const std::map<std::string, CampCameraData>& _cams) { m_cams = _cams; }

	//行き先を決める
	// _name      ... 行き先のカメラの名前
	// _immediate ... trueなら動かさずに瞬間移動
	void MoveTo(const std::string& _name, bool _immediate = false);

	//---- 取得 ----

	//カメラの値を返す(エディタで書き換える用)
	//戻り値 ... 名前とカメラの値
	std::map<std::string, CampCameraData>& WorkCameraData() { return m_cams; }

	//今の行き先の名前を返す
	//戻り値 ... 名前
	const std::string& GetCurrentName() const { return m_toName; }

private:

	//---- メンバ変数 ----

	//名前ごとのカメラの値
	std::map<std::string, CampCameraData> m_cams;

	//動き始めたときの値
	CampCameraData m_from;

	//今の値
	CampCameraData m_now;

	//行き先の名前
	std::string m_toName = "Explore";

	//移動にかける時間(秒)
	float m_moveTime = 0.6f;

	//動き始めてからの時間(秒)
	float m_moveTimer = 0.0f;

	//---- その他の処理 ----

	//今の値からカメラの行列を作る
	void ApplyNow();
};