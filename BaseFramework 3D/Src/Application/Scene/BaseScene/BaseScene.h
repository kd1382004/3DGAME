#pragma once
#include"../SceneManager.h"

struct Inventory;

enum WeaponStrengtheningID;

struct WeaponsStrengtheningInfo;

class BaseScene
{
public :

	//コンストラクタ
	BaseScene()			 {}
	//デストラクタ
	virtual ~BaseScene() {}

	//全オブジェクトの更新前の処理
	void PreUpdate();
	//全オブジェクトの更新
	void Update();
	//全オブジェクトの更新後の処理
	void PostUpdate();

	//全オブジェクトの描画前の処理
	void PreDraw();
	//全オブジェクトの描画
	void Draw();
	//全オブジェクトの2D描画
	void DrawSprite();
	//全オブジェクトのデバッグ描画
	void DrawDebug();

	// オブジェクトリストを取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList()
	{
		return m_objList;
	}
	
	// オブジェクトリストに追加
	// _spObj ... 追加するオブジェクト
	void AddObject(const std::shared_ptr<KdGameObject>& _spObj)
	{
		m_objList.push_back(_spObj);
	}


	//追加
	//ImGUIをいじるならこの中で
	virtual void ImGUI();
	//シーンの初期化
	virtual void Init();
	//ポーズ中にするかをセット
	// _flg ... ポーズ中ならtrue
	void SetPoseFlg(bool _flg) { m_poseFlg = _flg; }

	//ゲーム結果を保存
	virtual void SetGameResult() {};



protected :

	// 継承先シーンで必要ならオーバーライドする
	virtual void Event();


	// 全オブジェクトのアドレスをリストで管理
	std::list<std::shared_ptr<KdGameObject>> m_objList;


	//ポーズ中か
	bool m_poseFlg=false;





	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//データセーブ

	///////////////////////////////////////
	//Inventoryの情報
	//_isSave...セーブするか(true)・リセットするか(false)
	//_money … 所持金(PlayerInventoryの値)。-1のときは元のファイルの値を引き継ぐ
	// _inventoryPotionsList ... 持っているポーションのリスト
	// _inventoryWeaponsList ... 持っている強化素材のリスト
	// _filePath ... 保存するファイルのパス
	void SaveInventoryData(bool _isSave, const std::vector<Inventory>& _inventoryPotionsList, const std::vector<Inventory>& _inventoryWeaponsList, std::string _filePath, long _money = -1);

	//ロード
	// _filePath ... 読み込むファイルのパス
	std::vector<std::vector<Inventory>> LoadInventoryData(std::string _filePath);
	//JSONから持ち物のリストを読み込む
	// _data ... 読み込んだJSON
	// _key ... 読むリストのキー("potions"など)
	//戻り値 ... 持ち物のリスト
	std::vector<Inventory> ReadInventoryList(const nlohmann::json& _data, const std::string& _key);

	//パス
	const std::string m_inventoryFilePath = "Asset/Data/PlayerData/PlayerInventoryData.json";
	//リザルト画面用の持ち物のファイルのパス
	const std::string m_inventoryGameResultFilePath = "Asset/Data/GameResult/PlayerInventoryData.json";

	///////////////////////////////////////
	//武器の所持情報(PlayerInventoryData.json の "weapons")
	//戻り値・引数の[PlayerWeaponInfo] = その武器を持っているか
	std::vector<bool> LoadOwnedWeapons();
	//武器の所持情報を保存する
	// _owned ... [PlayerWeaponInfo] = その武器を持っているか
	void SaveOwnedWeapons(const std::vector<bool>& _owned);

	///////////////////////////////////////
	//所持金(PlayerInventoryData.json の "money")
	//ダンジョンの中では PlayerInventory が持つ。これはプレイヤーがいないシーン(準備画面など)と、
	//ゲーム開始時の読み込みで使う
	//力尽きたとき(SaveInventoryData で _isSave が false)は 0 になる
	long LoadMoney();
	//所持金を保存する
	// _money ... 所持金
	void SaveMoney(long _money);

	///////////////////////////////////////

	///////////////////////////////////////
	//武器の強化情報の保存
	//_isSave...セーブするか(true)・リセットするか(false)
	// _filePath ... 保存するファイルのパス
	// _weaponStrengtheningList ... 武器の強化情報のリスト
	void SaveWeaponStrengtheningData(bool _isSave, std::string _filePath, std::vector<WeaponsStrengtheningInfo> _weaponStrengtheningList);

	//ロード
	// _filePath ... 読み込むファイルのパス
	std::vector<WeaponsStrengtheningInfo> LoadWeaponStrengtheningData(std::string _filePath);
	//JSONから武器の強化情報のリストを読み込む
	// _data ... 読み込んだJSON
	// _key ... 読む武器のキー("dagger"など)
	//戻り値 ... 武器の強化情報のリスト
	std::vector <WeaponsStrengtheningInfo> ReadWeaponStrengtheningList(const nlohmann::json& _data, const std::string& _key);

	//マスタ（MsWeaponsStrengtheningInfo.json）を土台に、セーブの進行状況を重ねて返す
	// _saveFilePath ... セーブデータのファイルのパス
	std::vector<WeaponsStrengtheningInfo> LoadMergedWeaponStrengtheningData(const std::string& _saveFilePath);

	//パス
	const std::string m_weaponStrengtheningFilePath = "Asset/Data/PlayerData/PlayerWeaponStrengtheningData.json";
	//リザルト画面用の武器の強化情報のファイルのパス
	const std::string m_weaponStrengtheningGameResultFilePath = "Asset/Data/GameResult/PlayerWeaponStrengtheningData.json";
	//武器の強化のマスターデータのファイルのパス
	const std::string m_weaponStrengtheningMSDateFilePath = "Asset/Data/ObjeData/Weapon/MsWeaponsStrengtheningInfo.json";


	//武器の種類から、JSONのキーを返す
	// _weaponType ... 武器の種類(PlayerWeaponInfo)
	//戻り値 ... JSONのキー("dagger"など)
	std::string GetWeaponKey(int _weaponType);

	///////////////////////////////////////

	///////////////////////////////////////
	//プレイヤーのステータスの保存
	//パス
	const std::string m_playerStatusFilePath = "Asset/Data/PlayerData/PlayerStatusData.json";
	//リザルト画面用のプレイヤーのステータスのファイルのパス
	const std::string m_playerStatusGameResultFilePath = "Asset/Data/GameResult/PlayerStatusData.json";

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////
	//プレイ記録(最新1件 + 最高階層ベスト3)

	// GameRecordの定義はSceneManager.h(GameResultの隣)

	// 今回の結果を記録に反映して保存(最新を上書き・ベスト3を更新)
	// _result ... 今回のプレイの結果
	void SaveGameRecord(const GameResult& _result);

	// 記録を読み込む
	GameRecord LoadGameRecord();

	//パス
	const std::string m_gameRecordFilePath = "Asset/Data/PlayerData/GameRecordData.json";

	//ショップの再入荷
	//在庫が0の商品だけ、ShopMasterData.json の startStock まで戻して保存する
	//ダンジョンから帰ってきたとき(ResultScene::Init)に呼ぶ
	void RestockShop();

	//ショップの値段・最初の在庫数のファイルのパス
	const std::string m_shopMasterFilePath = "Asset/Data/Shop/ShopMasterData.json";
	//ショップの今の在庫のファイルのパス
	const std::string m_shopStockFilePath = "Asset/Data/PlayerData/ShopStockData.json";

private:
	//ゲーム結果をJSONにする
	// _result ... ゲーム結果
	//戻り値 ... JSON
	nlohmann::json GameResultToJson(const GameResult& _result);
	//JSONからゲーム結果を作る
	// _data ... 読み込んだJSON
	//戻り値 ... ゲーム結果
	GameResult JsonToGameResult(const nlohmann::json& _data);

	// _aが_bより上の順位か(到達階 → 脱出 → 時間の短さ)
	bool IsBetterResult(const GameResult& _a, const GameResult& _b);



};
