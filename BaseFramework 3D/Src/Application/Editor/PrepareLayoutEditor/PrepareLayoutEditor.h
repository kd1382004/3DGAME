#pragma once

class PrepareScene;
class PrepareCamera;
class CampLight;
class CampObject;

//////////////////////////////////////////////////////////////
// 準備画面の背景(キャンプ)を調整する ImGui ウィンドウ
//  ・カメラ(Explore / Equip / Shop / Records)の位置・注視点・画角
//  ・ライトの色・明るさ・半径
//  ・置物の位置・回転・拡大(確認用。保存はしない)
// カメラとライトは CampLayoutTune.json に保存する
// (CampLayout.json は Blender から書き出し直すと上書きされるので分けている)
//////////////////////////////////////////////////////////////
class PrepareLayoutEditor
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	// _tuneFilePath ... 保存先(CampLayoutTune.json)のパス
	explicit PrepareLayoutEditor(const std::string& _tuneFilePath) : m_tuneFilePath(_tuneFilePath) {}

	//---- 設定 ----

	//編集する相手をセット(PrepareScene::Init の最後で呼ぶ)
	// _spScene   ... 準備シーン(タブの切り替えに使う)
	// _spCamera  ... 準備画面のカメラ
	// _lights    ... ライトのリスト
	// _objects   ... 置物のリスト
	void SetTargets(const std::shared_ptr<PrepareScene>& _spScene,
		const std::shared_ptr<PrepareCamera>& _spCamera,
		const std::vector<std::shared_ptr<CampLight>>& _lights,
		const std::vector<std::shared_ptr<CampObject>>& _objects);

	//---- 基本の処理 ----

	// 毎フレーム ImGui のフレーム内で呼ぶ
	// _open ... ウィンドウの×ボタンで閉じたときにfalseを入れるフラグ
	void Draw(bool* _open = nullptr);

	//---- その他の処理 ----

	//カメラとライトを CampLayoutTune.json に保存する
	//戻り値 ... 保存できたらtrue
	bool SaveTune();

private:

	//---- メンバ変数 ----

	//保存先のパス
	std::string m_tuneFilePath;

	//準備シーン
	std::weak_ptr<PrepareScene> m_wpScene;

	//カメラ
	std::weak_ptr<PrepareCamera> m_wpCamera;

	//ライト
	std::vector<std::weak_ptr<CampLight>> m_wpLights;

	//置物
	std::vector<std::weak_ptr<CampObject>> m_wpObjects;

	//選んでいる置物の番号(-1 なら何も選んでいない)
	int m_selectObject = -1;

	//未保存の変更があるか
	bool m_dirty = false;

	//画面に出すメッセージ(保存したときなど)
	std::string m_message;

	//---- 基本の処理 ----

	//カメラの欄
	void CameraImGui();

	//ライトの欄
	void LightImGui();

	//置物の欄
	void ObjectImGui();
};
