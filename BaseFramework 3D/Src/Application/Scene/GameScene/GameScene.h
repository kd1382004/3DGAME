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


class WeaponStrengtheningEditor;
class TreasureChestTableEditor;
class ComboStepEditor;
class CharaStatusEditor;
class EnemyAttackPatternEditor;


class GameScene : public BaseScene, public std::enable_shared_from_this<GameScene>
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	GameScene() {}

	//デストラクタ
	~GameScene() override;

	//---- 基本の処理 ----

	//ImGuiの表示
	void ImGUI()override;

	//シーンの初期化
	void Init()  override;

	//---- 取得 ----

	//ポーションを使う処理を取得
	//戻り値 ... ポーションを使う処理
	std::shared_ptr<PotionUseController> GetPotionUseController() { return m_spPotionUseController; };

	//カメラを取得
	//戻り値 ... カメラ
	std::shared_ptr<TPSCamera> GetCamera() const { return m_spCamera; }

	//UIの管理を取得
	//戻り値 ... UIの管理
	std::shared_ptr<UIManager> GetUIManager() const { return m_spUIManager; }

	//今の階層を取得
	//戻り値 ... 今の階層
	int GetDisplayFloor() const { return m_displayFloor; }

	//---- 設定 ----

	//ゲーム結果の保存
	void SetGameResult()override;

	//倒した敵の数を1増やす
	void AddEnemyKill();

	//開けた宝箱の数を1増やす
	void AddTreasureChestsNum();

	//---- その他の処理 ----

	//今の階層のマップを作る
	void GenerateMap();

	//ワープゲート(次の階・脱出)を置く
	// _setNextGatePos ... 次の階へのゲートの位置
	// _setEscapeGatePos ... 脱出のゲートの位置
	void WarpGateInit(Math::Vector3 _setNextGatePos, Math::Vector3 _setEscapeGatePos);

	//リザルトシーンに移行
	void ChangeResultScene();

	//プレイヤーが倒されたときの処理
	void PlayerDefeat();

private:

	//---- 型 ----

	//デバッグ用エディタ1つ分(表示名・開いているか・描画処理)
	struct EditorEntry
	{
		const char* m_name = "";              // チェックボックスの表示名
		bool m_open = false;                   // 開いているか
		std::function<void(bool*)> m_draw;     // 描画処理
	};

	//---- メンバ変数 ----

	//ステータスを持つキャラクターのリスト
	std::list<std::shared_ptr<CharacterBase>>m_spCharacterStatus;

	//マップの管理
	std::shared_ptr<MapManager>m_spMapManager;

	//プレイヤー
	std::shared_ptr<PlayerBase>m_spPlayer;

	//敵の管理
	std::shared_ptr<EnemyManager>m_spEnemyManager;

	//プレイヤーの武器
	std::shared_ptr<WeaponBase>m_spWeapon;

	//ポーションを使う処理
	std::shared_ptr<PotionUseController>m_spPotionUseController;

	//ワープゲートの管理
	std::shared_ptr<WarpGateManager>m_spWarpGateManager;

	//マップ上のオブジェクト(たいまつ・宝箱など)の管理
	std::shared_ptr<MapObjManager>m_spMapObjManager;

	//カメラ
	std::shared_ptr<TPSCamera> m_spCamera;

	//UIの管理
	std::shared_ptr<UIManager> m_spUIManager;

	//階層
	int m_displayFloor = 0;

	//マップを作るときに使う階層
	int m_mapGenerateDisplayFloor = 0;

	//マップの大きさが大きくなる最大の階層
	int m_mapGenerateDisplayFloorMax = 9;

	//ボス戦になる倍数(割り切れたらボス戦)
	int m_bossInterval = 3;

	//マップ成長率
	int m_mapLinearGrowthPerFloor = 0;

	//1階ごとにマップの横の大きさが増える量
	float m_mapLinearGrowthPerFloorX = 0.0f;

	//1階ごとにマップの縦の大きさが増える量
	float m_mapLinearGrowthPerFloorY = 0.0f;

	//ゲーム結果
	int m_killEnemy = 0;

	//倒したボスの数
	int m_killBoss = 0;

	//プレイ時間(秒)
	float m_playtime = 0;

	//プレイヤーが倒されたか
	bool m_playerDefeatFlg = false;

	//全体の情報(到達階・倒した敵・開けた宝箱・プレイ時間)
	DisplayFloorInfo m_displayFloorInfo;

	//出発したときの持ち込み(Initで覚える)
	CarryInInfo m_carryIn;

	//プレイした日時(Initで覚える。time_tの値)
	long long m_playDate = 0;

	//音

	//通常の階のBGM
	std::shared_ptr<KdSoundInstance> m_spGameBGM;

	//ボスの階のBGM
	std::shared_ptr<KdSoundInstance> m_spBossBGM;

	//武器用のImGUI
	// メンバに持つ
	std::shared_ptr<WeaponStrengtheningEditor> m_spWSEditor;

	//宝箱のドロップテーブル用のImGUI
	std::shared_ptr<TreasureChestTableEditor> m_spTCTableEditor;

	//コンボのImGui
	std::shared_ptr<ComboStepEditor> m_spComboStepEditor;

	//キャラの初期ステータスのImGui
	std::shared_ptr<CharaStatusEditor> m_spCharaStatusEditor;

	//敵の攻撃パターンのImGui
	std::shared_ptr<EnemyAttackPatternEditor> m_spEnemyAttackPatternEditor;

	//ImGuiで開けるエディタの一覧
	std::vector<EditorEntry> m_editors;

	//---- その他の処理 ----

	//シーンのイベント(毎フレーム)
	void Event() override;
};
