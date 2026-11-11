#pragma once

class AccessoryBase:public KdGameObject
{
public:
	AccessoryBase() {};
	~AccessoryBase()override {};

	virtual void Init() override;
	virtual void Update() override;
	virtual void PostUpdate() override;

	virtual void GenerateDepthMapFromLight() override;
	virtual void DrawLit() override;
	virtual void DrawBright() override;

	void SetParentMatrix(Math::Matrix _mat) { m_accessoryParentMat = _mat; }
	void SetParentRotation(Math::Vector3 _rot) { m_accessoryParentRot = _rot; }

	void SetOwnerMatrix(const Math::Matrix& _mat) { m_ownerMat = _mat; }

protected:

	//ローカル
	Math::Matrix m_localMat = Math::Matrix::Identity;
	Math::Vector3 m_localPos = Math::Vector3::Zero;

	//親
	Math::Matrix m_accessoryParentMat = Math::Matrix::Identity;
	Math::Vector3 m_accessoryParentRot = Math::Vector3::Zero;

	Math::Matrix m_ownerMat = Math::Matrix::Identity;
};
