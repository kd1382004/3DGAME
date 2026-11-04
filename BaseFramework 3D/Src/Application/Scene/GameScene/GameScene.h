#pragma once

#include"../BaseScene/BaseScene.h"
#include"../SceneManager.h"


class CharacterBase;
class MapManager;
class PlayerBase;
class EnemyManager;
class WeaponBase;
class PotionUseController;
class WarpGateManager;
class MapObjManager;
class TPSCamera;
class UIManager;


struct Inventory;

enum WeponStrengtheningID;

struct WeaponsStrengtheningInfo;


class GameScene : public BaseScene, public std::enable_shared_from_this<GameScene>
{
public:

	GameScene() {}
	~GameScene() override;

	void ImGUi()override;

	void GenerateMap();
	void Init()  override;


	std::shared_ptr<PotionUseController> GetPotionUseController() { return m_spPotionUseController; };
	std::shared_ptr<TPSCamera> GetCamera() const { return m_spCamera; }
	std::shared_ptr<UIManager> GetUIManager() const { return m_spUIManager; }
	int GetDisplayFloor() const { return m_displayFloor; }


	void WarpGateInit(Math::Vector3 _setNextGatePos, Math::Vector3 _setEacapeGatePos);

	//ゲーム結果の保存
	void SetGameResult()override;

	//リザルトシーンに移行
	void ChangeResultScene();

	void AddEnemyKill();
	void AddTreasureChestsNum();


	void PlayerDefeat();
private:

	void Event() override;


	std::list<std::shared_ptr<CharacterBase>>m_spCharacterStatus;
	std::shared_ptr<MapManager>m_spMapManager;
	std::shared_ptr<PlayerBase>m_spPlayer;
	std::shared_ptr<EnemyManager>m_spEnemyManager;
	std::shared_ptr<WeaponBase>m_spWeapon;
	std::shared_ptr<PotionUseController>m_spPotionUseController;
	std::shared_ptr<WarpGateManager>m_spWarpGateManager;
	std::shared_ptr<MapObjManager>m_spMapObjManager;
	std::shared_ptr<TPSCamera> m_spCamera;
	std::shared_ptr<UIManager> m_spUIManager;


	//階層
	int m_displayFloor;
	int m_mapGenerateDisplayFloor;
	int m_mapGenerateDisplayFloorMax = 7;


	//ボス戦になる倍数(割り切れたらボス戦)
	int m_bossInterval = 3;

	//マップ成長率
	int m_mapLinearGrowthPerFloor;
	float m_mapLinearGrowthPerFloorX;
	float m_mapLinearGrowthPerFloorY;

	//ゲーム結果
	int m_killEnemy = 0;
	int m_killBoss = 0;
	float m_playtime = 0;
	bool m_playerDefeatFlg = false;

	std::vector<DisplayFloorInfo>m_DisplayFloorInfo;

	//音

	std::shared_ptr<KdSoundInstance> m_spGameBGM;
	std::shared_ptr<KdSoundInstance> m_spBossBGM;


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//データセーブ

	///////////////////////////////////////
	//Inventoryの情報
	//_isSave...セーブするか(true)・リセットするか(false)
	void SaveInventoryData(bool _isSave, const std::vector<Inventory>& inventoryPotionsList, const std::vector<Inventory>& inventoryWeaponsList);

	//ロード
	void LoadInventoryData();
	std::vector<Inventory> ReadInventoryList(const nlohmann::json& _data, const std::string& _key);

	//パス
	const std::string m_InventoryFilePath = "Asset/Data/PlayerData/PlayerInventoryData.json";

	///////////////////////////////////////

	///////////////////////////////////////
	//武器の強化情報の保存
	//_isSave...セーブするか(true)・リセットするか(false)
	void SaveWeaponStrengtheningData(bool _isSave);

	//ロード
	void LoadWeaponStrengtheningData();
	std::vector <WeaponsStrengtheningInfo> ReadWeaponStrengtheningList(const nlohmann::json& _data, const std::string& _key);

	//パス
	const std::string m_WeaponStrengtheningFilePath = "Asset/Data/PlayerData/PlayerWeaponStrengtheningData.json";


	std::string GetWeaponKey(int _weaponType)
	{
		switch (_weaponType)
		{
		case PlayerWeaponInfo_Dagger: return "dagger";
		case PlayerWeaponInfo_Sword:  return "sword";
		default:                      return "";
		}
	}
	///////////////////////////////////////
	
	///////////////////////////////////////
	//プレイヤーのステータスの保存
		//パス
	const std::string m_PlayerStatusFilePath = "Asset/Data/PlayerData/PlayerStatusData.json";

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};
