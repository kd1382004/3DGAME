#pragma once

class PlayerBase;
class CameraBase;

class TreasureChest:public KdGameObject
{
public:
	TreasureChest(){};
	~TreasureChest() override{};

	void Init()override;
	void Update()override;
	void GenerateDepthMapFromLight()override;
	void PreDraw()override;
	void DrawLit()override;

	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayer) { m_wpPlayer = _spPlayer; };
	void SetCamera(std::shared_ptr<CameraBase> _spCamera) { m_wpCamera = _spCamera; };
private:
	std::weak_ptr<PlayerBase>m_wpPlayer;
	std::weak_ptr<CameraBase>m_wpCamera;


	std::shared_ptr<KdModelWork>m_treasureChestModel = nullptr;
	std::shared_ptr<KdAnimator>	m_treasureChestAnimetor = nullptr;

	//空いてるかどうか
	bool m_IsOpen = false;


	//視錐台用のBoxInfo
	KdCollider::BoxInfo m_frustumBox;

	bool m_isInView = false;
};
