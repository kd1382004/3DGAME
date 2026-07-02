#pragma once

class  TitleSceneEditor
{
public:
	 TitleSceneEditor();
	~ TitleSceneEditor();

	void ImGui();
private:

	void CameraImGUI(std::list<std::shared_ptr<KdGameObject>>_obj);
	void ButtonImGUI(std::list<std::shared_ptr<KdGameObject>>_obj);
	void TerrainsImGUI(std::list<std::shared_ptr<KdGameObject>>_obj);

};
