#pragma once

class AccessoryBase;

class AccessoryManager:public KdGameObject
{
public:
	//コンストラクタ
	AccessoryManager() {}
	//デストラクタ
	~AccessoryManager()override {}


	//初期化
	void Init() override;
	//更新前の処理
	void PreUpdate() override;
	//更新
	void Update() override;
	//更新後の処理
	void PostUpdate() override;
	//影の描画
	void GenerateDepthMapFromLight() override;
	//ライトありの描画
	void DrawLit() override;
	//光る部分の描画
	void DrawBright() override;

	//アクセサリーを追加する
	// _spAccessory ... 追加するアクセサリー
	void AddAccessoryList(std::shared_ptr<AccessoryBase> _spAccessory)
	{
		m_accessoryList.push_back(_spAccessory);
	}

private:


	//持っているアクセサリーのリスト
	std::list<std::shared_ptr<AccessoryBase>>m_accessoryList;


};
