#pragma once

class AccessoryBase;

//アクセサリーの管理
class AccessoryManager:public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	AccessoryManager() {}

	//デストラクタ
	~AccessoryManager()override {}

	//---- 基本の処理 ----

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

	//---- 設定 ----

	//アクセサリーを追加する
	// _spAccessory ... 追加するアクセサリー
	void AddAccessoryList(std::shared_ptr<AccessoryBase> _spAccessory)
	{
		m_accessoryList.push_back(_spAccessory);
	}

private:

	//---- メンバ変数 ----

	//持っているアクセサリーのリスト
	std::list<std::shared_ptr<AccessoryBase>>m_accessoryList;
};
