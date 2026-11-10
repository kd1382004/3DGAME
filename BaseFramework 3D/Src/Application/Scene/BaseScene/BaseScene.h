#pragma once
struct Inventory;

enum WeponStrengtheningID;

struct WeaponsStrengtheningInfo;

class BaseScene
{
public :

	BaseScene()			 {}
	virtual ~BaseScene() {}

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// オブジェクトリストを取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList()
	{
		return m_objList;
	}
	
	// オブジェクトリストに追加
	void AddObject(const std::shared_ptr<KdGameObject>& _obj)
	{
		m_objList.push_back(_obj);
	}


	//追加
	//ImGUIをいじるならこの中で
	virtual void ImGUi();
	virtual void Init();
	void SetPoseFlg(bool _flg) { m_poseFlg = _flg; }

	//ゲーム結果を保存
	virtual void SetGameResult() {};
protected :

	// 継承先シーンで必要ならオーバーライドする
	virtual void Event();


	// 全オブジェクトのアドレスをリストで管理
	std::list<std::shared_ptr<KdGameObject>> m_objList;


	bool m_poseFlg=false;





	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//データセーブ

	///////////////////////////////////////
	//Inventoryの情報
	//_isSave...セーブするか(true)・リセットするか(false)
	void SaveInventoryData(bool _isSave, const std::vector<Inventory>& inventoryPotionsList, const std::vector<Inventory>& inventoryWeaponsList, std::string _filePath);

	//ロード
	std::vector<std::vector<Inventory>> LoadInventoryData(std::string _filePath);
	std::vector<Inventory> ReadInventoryList(const nlohmann::json& _data, const std::string& _key);

	//パス
	const std::string m_InventoryFilePath = "Asset/Data/PlayerData/PlayerInventoryData.json";
	const std::string m_InventoryGameResultFilePath = "Asset/Data/GameResult/PlayerInventoryData.json";

	///////////////////////////////////////

	///////////////////////////////////////
	//武器の強化情報の保存
	//_isSave...セーブするか(true)・リセットするか(false)
	void SaveWeaponStrengtheningData(bool _isSave, std::string _filePath, std::vector<WeaponsStrengtheningInfo> weaponStrengtheningList);

	//ロード
	std::vector<WeaponsStrengtheningInfo> LoadWeaponStrengtheningData(std::string _filePath);
	std::vector <WeaponsStrengtheningInfo> ReadWeaponStrengtheningList(const nlohmann::json& _data, const std::string& _key);

	//マスタ（MsWeaponsStrengtheningInfo.json）を土台に、セーブの進行状況を重ねて返す
	std::vector<WeaponsStrengtheningInfo> LoadMergedWeaponStrengtheningData(const std::string& _saveFilePath);

	//パス
	const std::string m_WeaponStrengtheningFilePath = "Asset/Data/PlayerData/PlayerWeaponStrengtheningData.json";
	const std::string m_WeaponStrengtheningGameResultFilePath = "Asset/Data/GameResult/PlayerWeaponStrengtheningData.json";
	const std::string m_WeaponStrengtheningMSDateFilePath = "Asset/Data/ObjeData/Weapon/MsWeaponsStrengtheningInfo.json";


	std::string GetWeaponKey(int _weaponType);

	///////////////////////////////////////

	///////////////////////////////////////
	//プレイヤーのステータスの保存
	//パス
	const std::string m_PlayerStatusFilePath = "Asset/Data/PlayerData/PlayerStatusData.json";
	const std::string m_PlayerStatusGameResultFilePath = "Asset/Data/GameResult/PlayerStatusData.json";

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
