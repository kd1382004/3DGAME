#pragma once

#include"../CampLayoutData/CampLayoutData.h"

class CampObject : public KdGameObject
{
public:
	void Init() override;
	void Update() override;
	void DrawLit() override;
	void GenerateDepthMapFromLight() override;

	// データを入れて、モデルと行列を作り直す
	void SetData(const CampObjectData& _data);

	// エディタから値を変えるときに使う
	CampObjectData& WorkData() { return m_data; }
	void ApplyData() { m_mWorld = m_data.MakeWorld(); }
	void PlayAnime(const std::string& _name, bool _loop);
private:
	CampObjectData m_data;
	std::shared_ptr<KdModelWork> m_spModel;
	std::shared_ptr<KdAnimator> m_spAnimator;
};