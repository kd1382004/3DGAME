#pragma once

class CharacterBase : public KdGameObject
{
public:
	CharacterBase() {};
	~CharacterBase()	override { Release(); };

	void Init()			override;
	void Update()		override;
	void PostUpdate()	override;

	void GenerateDepthMapFromLight()override;
	void DrawLit()		override;

	void RegistHitObject(const std::shared_ptr<KdGameObject>& object)
	{
		m_wpHitObjectList.push_back(object);
	}

private:
	// 衝突判定とそれに伴う座標の更新
	void CollisionUpdate();

	// 解放処理
	void Release();

protected:
	std::shared_ptr<KdModelWork> m_spCharaModel = nullptr;
	std::vector<std::weak_ptr<KdGameObject>> m_wpHitObjectList{};
	float m_Gravity = 0;
};
