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

class GameScene : public BaseScene, public std::enable_shared_from_this<GameScene>
{
public:

	GameScene() {}
	~GameScene() override{}

	void ImGUi()override;

	void GenerateMap();
	void Init()  override;


	std::shared_ptr<PotionUseController> GetPotionUseController() { return m_spPotionUseController; };


	void WarpGateInit(Math::Vector3 _setNextGatePos, Math::Vector3 _setEacapeGatePos);

	//ゲーム結果の保存
	void SetGameResult()override;

	//リザルトシーンに移行
	void ChangeResultScene();

	void AddEnemyKill();


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
	float m_playtime=0;
	bool m_playerDefeatFlg = false;

	std::vector<DisplayFloorInfo>m_DisplayFloorInfo;
};
