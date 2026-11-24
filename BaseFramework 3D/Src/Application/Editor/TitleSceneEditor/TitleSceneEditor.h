#pragma once

//タイトルシーンの配置編集エディター
class  TitleSceneEditor
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	TitleSceneEditor();

	//デストラクタ
	~ TitleSceneEditor();

	//---- 基本の処理 ----

	//タイトルシーンのオブジェクトを編集するImGuiを表示する
	void ImGui();

private:

	//---- メンバ変数 ----

	//名前からオブジェクトを作る工場
	KdGameObjectFactory m_factory;

	//---- その他の処理 ----

	//カメラを編集するImGui
	// _obj ... カメラのオブジェクトのリスト
	void CameraImGUI(std::list<std::shared_ptr<KdGameObject>>_obj);

	//ボタンを編集するImGui
	// _obj ... ボタンのオブジェクトのリスト
	void ButtonImGUI(std::list<std::shared_ptr<KdGameObject>>_obj);

	//地形を編集するImGui
	// _obj ... 地形のオブジェクトのリスト
	void TerrainsImGUI(std::list<std::shared_ptr<KdGameObject>>_obj);
};
