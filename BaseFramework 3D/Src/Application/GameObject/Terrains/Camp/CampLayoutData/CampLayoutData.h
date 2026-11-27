#pragma once

//////////////////////////////////////////////////////////////
// 準備画面の背景(キャンプ)の配置データ
// CampLayout.json を読む(Blender から書き出した物 / 準備シーンのエディタで保存した物)
//
// JSON の値は glTF の座標(右手系)なので、読むときに
// ゲームの座標(左手系)へ直す(ConvPos / ConvRot)
// KdGLTFLoader がモデルを読むときと同じ「Z反転」のルール
//////////////////////////////////////////////////////////////

// 置物1個分(ゲームの座標に変換済み)
struct CampObjectData
{
	std::string   m_name;					// Blenderでの名前(エディタの表示用)
	std::string   m_kind;					// Prop / Torch / Campfire / Player
	std::string   m_modelPath;				// モデルのパス
	Math::Vector3 m_pos    = Math::Vector3::Zero;	// 位置
	Math::Vector3 m_rotDeg = Math::Vector3::Zero;	// 度
	Math::Vector3 m_scale  = Math::Vector3::One;	// 大きさ

	// ワールド行列を作る(拡大 → X → Y → Z回転 → 移動)
	// 戻り値 ... ワールド行列
	Math::Matrix MakeWorld() const
	{
		return Math::Matrix::CreateScale(m_scale)
			* Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_rotDeg.x))
			* Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_rotDeg.y))
			* Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(m_rotDeg.z))
			* Math::Matrix::CreateTranslation(m_pos);
	}
};

// 点光源1個分
struct CampLightData
{
	std::string   m_name;						// Blenderでの名前(エディタの表示用)
	Math::Vector3 m_pos       = Math::Vector3::Zero;	// 位置
	Math::Vector3 m_color     = Math::Vector3::One;	// 0〜1
	float         m_power     = 0.0f;				// Blenderの値(参考)
	float         m_intensity = 10.0f;				// ゲーム用の明るさ(色に掛ける)
	float         m_radius    = 8.0f;				// ゲーム用の半径
};

// カメラ1台分
struct CampCameraData
{
	Math::Vector3 m_pos    = Math::Vector3::Zero;	// カメラの位置
	Math::Vector3 m_target = Math::Vector3::Zero;	// 注視点
	float         m_fovY   = 40.0f;					// 縦の画角(度)
};

// 出発の演出(出発ボタン → プレイヤーが入口へ歩く → 暗転 → ゲーム)
// CampLayoutTune.json の "Depart" に保存する
struct CampDepartData
{
	Math::Vector3 m_target         = Math::Vector3::Zero;	// 歩いていく先(入口の少し奥)
	float         m_walkTime       = 2.0f;					// そこまで何秒で歩くか
	float         m_fadeStartRate  = 0.6f;					// 歩きの何割で暗転を始めるか(0〜1)
	float         m_fadeTime       = 1.0f;					// 暗転にかける秒数
	bool          m_hasTarget      = false;					// Tune に行き先が書いてあったか

	// 入口の奥に出すエフェクト(ダンジョンの階段と同じ Stairs/Stairs.efkefc)
	Math::Vector3 m_effectPos      = Math::Vector3::Zero;	// 出す位置
	float         m_effectScale    = 1.0f;					// 大きさ
	bool          m_hasEffectPos   = false;					// Tune に位置が書いてあったか
};

// ファイル丸ごと
struct CampLayoutData
{
	std::vector<CampObjectData>           m_objects;	// 置物の一覧
	std::vector<CampLightData>            m_lights;		// 点光源の一覧
	std::map<std::string, CampCameraData> m_cameras;	// "Explore" "Equip" "Shop" "Records"
	CampDepartData                        m_depart;		// 出発の演出
};

//---- 座標の変換(glTFの右手系 ⇔ ゲームの左手系) ----
// 往復とも同じ計算(2回掛けると元に戻る)

// 位置 : z を反転
inline Math::Vector3 ConvPos(const Math::Vector3& _v) { return { _v.x, _v.y, -_v.z }; }

// 回転(度) : X と Y の角度を反転
inline Math::Vector3 ConvRot(const Math::Vector3& _v) { return { -_v.x, -_v.y, _v.z }; }

//---- JSON との変換 ----

// JSONの [x,y,z] → Vector3
inline Math::Vector3 JsonToVec3(const nlohmann::json& _j)
{
	return { _j[0].get<float>(), _j[1].get<float>(), _j[2].get<float>() };
}

// Vector3 → JSONの [x,y,z]
inline nlohmann::json Vec3ToJson(const Math::Vector3& _v)
{
	return nlohmann::json::array({ _v.x, _v.y, _v.z });
}

//---- 読み込み ----

// CampLayout.json を読んで、ゲームの座標に直して返す
// 読めないときは空で返す
// _filePath ... CampLayout.json のパス
// 戻り値    ... 配置データ
CampLayoutData LoadCampLayout(const std::string& _filePath);

// CampLayoutTune.json(エディタで調整した値)があれば上書きする
// ファイルが無ければ何もしない
// 出発の行き先が無いときは、Explore カメラの注視点(入口)をプレイヤーの高さにして仮で入れる
// _layout   ... 上書きする配置データ
// _filePath ... CampLayoutTune.json のパス
void ApplyCampLayoutTune(CampLayoutData& _layout, const std::string& _filePath);

//---- 書き出し ----

// 置物とライトを CampLayout.json と同じ形(glTFの座標)で保存する
// ファイルにあるほかの値(Cameras など)はそのまま残す
// 初めて上書きするときは、元のファイルを .bak として残す
// _layout   ... 保存する配置データ(m_objects と m_lights を使う)
// _filePath ... CampLayout.json のパス
// 戻り値    ... 保存できたらtrue
bool SaveCampLayout(const CampLayoutData& _layout, const std::string& _filePath);
