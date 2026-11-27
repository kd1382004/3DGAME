#pragma once

class PrepareScene;
class CampManager;
class CampObject;

//////////////////////////////////////////////////////////////
// 準備画面の背景(キャンプ)を調整する ImGui ウィンドウ
//  ・カメラ(Explore / Equip / Shop / Records)の位置・注視点・画角
//  ・置物の追加・複製・削除、位置・回転・拡大・モデル・種類
//  ・ライトの追加・削除、位置・色・明るさ・半径
//  ・出発の演出(行き先・歩く時間・暗転・入口の奥のエフェクト)
//
// 画面の置物はマウスでも動かせる
//  ・左クリック          … 選ぶ(何もない所で外す)
//  ・左ドラッグ          … 地面にそって動かす
//  ・Shift + 左ドラッグ  … 上下に動かす
//  ・ホイール            … 選んでいる置物を回す(15度ずつ / Shift で1度ずつ)
//  ・Delete              … 選んでいる置物を消す
//
// 保存先は2つ
//  ・置物とライトの配置           → CampLayout.json(Blender を使わずにここで決められる)
//  ・カメラ・ライトの明るさ・出発 → CampLayoutTune.json
//////////////////////////////////////////////////////////////
class PrepareLayoutEditor
{
public:

	//---- 設定 ----

	//編集する相手をセット(PrepareScene::Init で呼ぶ)
	// _spScene        ... 準備シーン(タブの切り替えに使う)
	// _spCampManager  ... キャンプの管理
	void SetTargets(const std::shared_ptr<PrepareScene>& _spScene, const std::shared_ptr<CampManager>& _spCampManager);

	//---- 基本の処理 ----

	// 毎フレーム ImGui のフレーム内で呼ぶ
	// _open ... ウィンドウの×ボタンで閉じたときにfalseを入れるフラグ
	void Draw(bool* _open = nullptr);

	//エディタを閉じたときに呼ぶ(選んでいる印を消して、ドラッグをやめる)
	void OnClose();

	//---- その他の処理 ----

	//カメラとライトの明るさと出発を CampLayoutTune.json に保存する
	//戻り値 ... 保存できたらtrue
	bool SaveTune();

	//置物とライトの配置を CampLayout.json に保存する
	//戻り値 ... 保存できたらtrue
	bool SaveLayout();

private:

	//---- メンバ変数 ----

	//準備シーン
	std::weak_ptr<PrepareScene> m_wpScene;

	//キャンプの管理
	std::weak_ptr<CampManager> m_wpCampManager;

	//プレイヤーを行き先に置いて見ているか
	bool m_departPreviewFlg = false;

	//行き先に置く前のプレイヤーの位置(見るのをやめたら戻す)
	Math::Vector3 m_departPreviewOrgPos = Math::Vector3::Zero;

	//選んでいる置物の番号(-1 なら何も選んでいない)
	int m_selectObject = -1;

	//選んでいるライトの番号(-1 なら何も選んでいない)
	int m_selectLight = -1;

	//置物の一覧の絞り込み(名前に含まれる文字)
	std::string m_objectFilter;

	//置物を追加するときのモデル(m_modelPaths の番号)
	int m_addModelIndex = 0;

	//選べるモデルの一覧(Asset/Models/Terrains/Camp の中と、今置いている物)
	std::vector<std::string> m_modelPaths;

	//マウスで置物を選ぶ・動かすか
	bool m_mousePickFlg = true;

	//地面・崖もマウスで選べるようにするか(大きくて、ほかの物を選ぶときに邪魔なので普段は外す)
	bool m_pickGroundFlg = false;

	//マウスで置物を動かしている最中か
	bool m_draggingFlg = false;

	//ドラッグしている置物
	std::weak_ptr<CampObject> m_wpDragObject;

	//ドラッグで地面にそって動かすときの高さ
	float m_dragPlaneY = 0.0f;

	//ドラッグを始めたときの、クリックした所から置物の位置までのずれ
	Math::Vector3 m_dragOffset = Math::Vector3::Zero;

	//前のフレームのマウスの位置(上下に動かすときに使う)
	ImVec2 m_dragLastMousePos = { 0.0f, 0.0f };

	//色を変えて描いている置物(選んでいる物)
	std::weak_ptr<CampObject> m_wpHighlight;

	//未保存の変更があるか(CampLayout.json)
	bool m_layoutDirty = false;

	//未保存の変更があるか(CampLayoutTune.json)
	bool m_tuneDirty = false;

	//画面に出すメッセージ(保存したときなど)
	std::string m_message;

	//---- 基本の処理 ----

	//カメラの欄
	void CameraImGui();

	//置物の欄
	void ObjectImGui();

	//ライトの欄
	void LightImGui();

	//出発の欄
	void DepartImGui();

	//マウスで置物を選ぶ・動かす(画面の上でのクリック・ドラッグ・ホイール)
	void UpdateMouse();

	//選んでいる置物だけ色を変えて描くようにする
	void UpdateHighlight();

	//---- その他の処理 ----

	//マウスの位置からレイ(カメラから奥へ向かう線)を作る
	// _rayPos ... レイの始まり
	// _rayDir ... レイの向き
	// _range  ... レイの長さ
	//戻り値   ... 作れたらtrue
	bool MakeMouseRay(Math::Vector3& _rayPos, Math::Vector3& _rayDir, float& _range) const;

	//マウスの下にある置物を探す
	//戻り値 ... 見つかった置物の番号と、当たった位置(無ければ番号 -1)
	std::pair<int, Math::Vector3> PickObject() const;

	//マウスのレイと、高さ _y の水平な面が交わる所
	// _y   ... 面の高さ
	// _out ... 交わる所
	//戻り値 ... 交わればtrue
	bool MouseOnPlaneY(float _y, Math::Vector3& _out) const;

	//選べるモデルの一覧を作り直す
	void RefreshModelPaths();

	//今のカメラが見ている地面の位置(置物を追加する位置)
	//戻り値 ... 注視点の高さを0にした位置
	Math::Vector3 GetCameraGroundPos() const;

	//ほかの置物と重ならない名前を作る("Barrel" → "Barrel_2" など)
	// _base ... 元の名前
	//戻り値 ... 使われていない名前
	std::string MakeUniqueObjectName(const std::string& _base) const;

	//ほかのライトと重ならない名前を作る
	// _base ... 元の名前
	//戻り値 ... 使われていない名前
	std::string MakeUniqueLightName(const std::string& _base) const;

	//選んでいる置物を作り直す(モデルや種類を変えたとき。焚き火の炎も付け直す)
	// _spObj ... 作り直す置物
	void RebuildObject(const std::shared_ptr<CampObject>& _spObj);
};
