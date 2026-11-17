#pragma once

class  TitleSceneEditor
{
public:
	 //コンストラクタ
	 TitleSceneEditor();
	//デストラクタ
	~ TitleSceneEditor();

	//タイトルシーンのオブジェクトを編集するImGuiを表示する
	void ImGui();
private:

	//カメラを編集するImGui
	// _obj ... カメラのオブジェクトのリスト
	void CameraImGUI(std::list<std::shared_ptr<KdGameObject>>_obj);
	//ボタンを編集するImGui
	// _obj ... ボタンのオブジェクトのリスト
	void ButtonImGUI(std::list<std::shared_ptr<KdGameObject>>_obj);
	//地形を編集するImGui
	// _obj ... 地形のオブジェクトのリスト
	void TerrainsImGUI(std::list<std::shared_ptr<KdGameObject>>_obj);


	//名前からオブジェクトを作る工場
	KdGameObjectFactory m_factory;
};
