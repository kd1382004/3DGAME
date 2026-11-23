#pragma once

//////////////////////////////////////////////////////////////
// 準備画面の背景(キャンプ)の配置データ
// Blender から書き出した CampLayout.json を読む
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
	Math::Vector3 m_pos    = Math::Vector3::Zero;
	Math::Vector3 m_rotDeg = Math::Vector3::Zero;	// 度
	Math::Vector3 m_scale  = Math::Vector3::One;

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
	std::string   m_name;
	Math::Vector3 m_pos       = Math::Vector3::Zero;
	Math::Vector3 m_color     = Math::Vector3::One;	// 0〜1
	float         m_power     = 0.0f;				// Blenderの値(参考)
	float         m_intensity = 10.0f;				// ゲーム用の明るさ(色に掛ける)
	float         m_radius    = 8.0f;				// ゲーム用の半径
};

// カメラ1台分
struct CampCameraData
{
	Math::Vector3 m_pos    = Math::Vector3::Zero;
	Math::Vector3 m_target = Math::Vector3::Zero;	// 注視点
	float         m_fovY   = 40.0f;					// 縦の画角(度)
};

// ファイル丸ごと
struct CampLayoutData
{
	std::vector<CampObjectData>           m_objects;
	std::vector<CampLightData>            m_lights;
	std::map<std::string, CampCameraData> m_cameras;	// "Explore" "Equip" "Shop" "Records"
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
// _layout   ... 上書きする配置データ
// _filePath ... CampLayoutTune.json のパス
void ApplyCampLayoutTune(CampLayoutData& _layout, const std::string& _filePath);
