#pragma once

class AccessoryBase;

class AccessoryManager:public KdGameObject
{
public:
	AccessoryManager() {}
	~AccessoryManager()override {}


	void Init() override;
	void PreUpdate() override;
	void Update() override;
	void PostUpdate() override;
	void GenerateDepthMapFromLight() override;
	void DrawLit() override;
	void DrawBright() override;

	void AddAccessoryList(std::shared_ptr<AccessoryBase> accessory)
	{
		m_accessoryList.push_back(accessory);
	}

private:


	std::list<std::shared_ptr<AccessoryBase>>m_accessoryList;


};
