#pragma once
#include"../SceneManager.h"

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
	//_money … 所持金(PlayerInventoryの値)。-1のときは元のファイルの値を引き継ぐ
	void SaveInventoryData(bool _isSave, const std::vector<Inventory>& inventoryPotionsList, const std::vector<Inventory>& inventoryWeaponsList, std::string _filePath, long _money = -1);

	//ロード
	std::vector<std::vector<Inventory>> LoadInventoryData(std::string _filePath);
	std::vector<Inventory> ReadInventoryList(const nlohmann::json& _data, const std::string& _key);

	//パス
	const std::string m_InventoryFilePath = "Asset/Data/PlayerData/PlayerInventoryData.json";
	const std::string m_InventoryGameResultFilePath = "Asset/Data/GameResult/PlayerInventoryData.json";

	///////////////////////////////////////
	//武器の所持情報(PlayerInventoryData.json の "weapons")
	//戻り値・引数の[PlayerWeaponInfo] = その武器を持っているか
	std::vector<bool> LoadOwnedWeapons();
	void SaveOwnedWeapons(const std::vector<bool>& _owned);

	///////////////////////////////////////
	//所持金(PlayerInventoryData.json の "money")
	//ダンジョンの中では PlayerInventory が持つ。これはプレイヤーがいないシーン(準備画面など)と、
	//ゲーム開始時の読み込みで使う
	//力尽きたとき(SaveInventoryData で _isSave が false)は 0 になる
	long LoadMoney();
	void SaveMoney(long _money);

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

	///////////////////////////////////////
	//プレイ記録(最新1件 + 最高階層ベスト3)

	// GameRecordの定義はSceneManager.h(GameResultの隣)

	// 今回の結果を記録に反映して保存(最新を上書き・ベスト3を更新)
	void SaveGameRecord(const GameResult& _result);

	// 記録を読み込む
	GameRecord LoadGameRecord();

	//パス
	const std::string m_GameRecordFilePath = "Asset/Data/PlayerData/GameRecordData.json";

	//ショップの再入荷
	//在庫が0の商品だけ、ShopMasterData.json の startStock まで戻して保存する
	//ダンジョンから帰ってきたとき(ResultScene::Init)に呼ぶ
	void RestockShop();

	const std::string m_ShopMasterFilePath = "Asset/Data/Shop/ShopMasterData.json";
	const std::string m_ShopStockFilePath = "Asset/Data/PlayerData/ShopStockData.json";

private:
	nlohmann::json GameResultToJson(const GameResult& _result);
	GameResult JsonToGameResult(const nlohmann::json& _data);

	// _aが_bより上の順位か(到達階 → 脱出 → 時間の短さ)
	bool IsBetterResult(const GameResult& _a, const GameResult& _b);



};
