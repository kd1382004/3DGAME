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

	void SetRotation(Math::Matrix _rMat);

protected:

	std::shared_ptr<KdModelWork>m_spModel = nullptr;
};

