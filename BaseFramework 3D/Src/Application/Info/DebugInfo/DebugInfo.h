#pragma once

class DebugInfo
{
public:



	void SetSceneManagerImGUIFlg(bool _flg) { m_SceneManagerImGUIFlg = _flg; }
	bool GetSceneManagerImGUIFlg() { return m_SceneManagerImGUIFlg; }

private:
	bool m_SceneManagerImGUIFlg = true;
private:
	DebugInfo() {};
	~DebugInfo() {};

public:
	static DebugInfo& Instance() {
		static DebugInfo Instance;
		return Instance;
	}
};
