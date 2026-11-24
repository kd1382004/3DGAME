#pragma once

class PrepareScene;
class PrepareCamera;
class CampLight;
class CampObject;
struct CampDepartData;

//////////////////////////////////////////////////////////////
// 準備画面の背景(キャンプ)を調整する ImGui ウィンドウ
//  ・カメラ(Explore / Equip / Shop / Records)の位置・注視点・画角
//  ・ライトの色・明るさ・半径
//  ・置物の位置・回転・拡大(確認用。保存はしない)
//  ・出発の演出(行き先・歩く時間・暗転)
// カメラとライトと出発は CampLayoutTune.json に保存する
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

	//出発の演出のデータをセット(PrepareScene の m_campLayout.m_depart を渡す)
	// _pDepart ... 出発のデータ(シーンが持っているもの。エディタより長く生きる)
	void SetDepartData(CampDepartData* _pDepart) { m_pDepart = _pDepart; }

	//入口の奥のエフェクトをセット(位置・大きさをいじったらその場で動かす)
	// _wpEffect ... PrepareScene が出したエフェクト
	void SetEntranceEffect(const std::weak_ptr<KdEffekseerObject>& _wpEffect) { m_wpEntranceEffect = _wpEffect; }

	//---- 基本の処理 ----

	// 毎フレーム ImGui のフレーム内で呼ぶ
	// _open ... ウィンドウの×ボタンで閉じたときにfalseを入れるフラグ
	void Draw(bool* _open = nullptr);

	//---- その他の処理 ----

	//カメラとライトと出発を CampLayoutTune.json に保存する
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

	//出発の演出のデータ(PrepareScene が持っている)
	CampDepartData* m_pDepart = nullptr;

	//入口の奥のエフェクト
	std::weak_ptr<KdEffekseerObject> m_wpEntranceEffect;

	//プレイヤーを行き先に置いて見ているか
	bool m_departPreviewFlg = false;

	//行き先に置く前のプレイヤーの位置(見るのをやめたら戻す)
	Math::Vector3 m_departPreviewOrgPos = Math::Vector3::Zero;

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

	//出発の欄
	void DepartImGui();

	//---- その他の処理 ----

	//キャンプのプレイヤー(m_kind が "Player" の置物)を探す
	//戻り値 ... 見つからなければ nullptr
	std::shared_ptr<CampObject> FindCampPlayer() const;
};
