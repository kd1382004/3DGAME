#pragma once

class MapBase:public KdGameObject
{
public:
	MapBase() {};
	~MapBase() {};


	void Init()override;
	void Update()override;
	void DrawLit();
	void GenerateDepthMapFromLight();



protected:

	std::shared_ptr<KdModelWork>m_spModel = nullptr;
};

