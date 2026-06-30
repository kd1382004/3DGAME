#pragma once

class DebugInfo
{
public:

	bool m_SceneManagerImGUIFlg = true;

private:

private:
	DebugInfo() {};
	~DebugInfo() {};

public:
	static DebugInfo& Instance() {
		static DebugInfo Instance;
		return Instance;
	}
};
