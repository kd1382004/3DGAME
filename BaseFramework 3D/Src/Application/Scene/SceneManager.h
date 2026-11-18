#pragma once

//各階層での情報
struct DisplayFloorInfo
{
	//階層
	int m_displayFloor = 0;

	//敵を倒した数
	int m_enemyKillCount = 0;

	//開けた宝箱の数
	int m_treasureChestsNum = 0;

	//その階層にいた時間(秒)
	float m_playTimeSc = 0;
};

//ゲームリザルト
struct GameResult
{
	//最初の配列は、全体の情報
	std::vector<DisplayFloorInfo>m_displayFloorInfo;

	//プレイヤー死亡
	bool m_playerDead = false;
};

//プレイ記録(最新1件 + 最高階層ベスト3)
//Asset/Data/PlayerData/GameRecordData.json に保存される
struct GameRecord
{
	bool m_hasLatest = false;			// 最新の記録があるか(1回もプレイしていなければfalse)
	GameResult m_latest;				// 最新のプレイ
	std::vector<GameResult> m_best;	// 最高階層の上位3件(1位から順)
};


//プレイヤーの武器情報
enum PlayerWeaponInfo
{
	PlayerWeaponInfo_Dagger,
	PlayerWeaponInfo_Sword,
	PlayerWeaponInfoSize
};


class BaseScene;

class SceneManager
{
public:

	// シーン情報
	enum class SceneType
	{
		Title,
		Prepare,
		Game,
		Result,
	};

	//今のシーンの更新前の処理
	void PreUpdate();
	//今のシーンの更新
	void Update();
	//今のシーンの更新後の処理
	void PostUpdate();

	//今のシーンの描画前の処理
	void PreDraw();
	//今のシーンの描画
	void Draw();
	//今のシーンの2D描画
	void DrawSprite();
	//今のシーンのデバッグ描画
	void DrawDebug();

	// 次のシーンをセット (次のフレームから切り替わる)
	// _nextScene ... 次のシーン
	void SetNextScene(SceneType _nextScene)
	{
		m_nextSceneType = _nextScene;
	}

	// 現在のシーンのオブジェクトリストを取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList();

	// 現在のシーンにオブジェクトを追加
	// _spObj ... 追加するオブジェクト
	void AddObject(const std::shared_ptr<KdGameObject>& _spObj);

	//追加
	void ImGUI();

	//ゲーム結果を取得
	//戻り値 ... ゲーム結果
	GameResult GetGameResult() { return m_gameResult; }
	//ゲーム結果をセット
	// _gameResult ... ゲーム結果
	void SetGameResult(GameResult _gameResult) { m_gameResult = _gameResult; }

	//描画先を変更
	// _spRtPack ... 描画先
	void ChangeRendertarget(std::shared_ptr<KdRenderTargetPack> _spRtPack);
	//描画先を元に戻す
	void UndoRenderTarget();

	//初期に戻す
	void UnFogAmbient();


	//プレイヤーの武器を取得
	//戻り値 ... プレイヤーの武器
	const PlayerWeaponInfo GetPlayerWeaponInfo() { return m_playerWeaponInfo; }
	//プレイヤーの武器をセット
	// _playerWeaponInfo ... プレイヤーの武器
	//変わったときはファイルに保存して、次に起動したときも同じ武器にする
	void SetPlayerWeaponInfo(PlayerWeaponInfo _playerWeaponInfo)
	{
		if (m_playerWeaponInfo == _playerWeaponInfo) { return; }
		m_playerWeaponInfo = _playerWeaponInfo;
		PlayerWeaponJsonSave();
	}



	//フォグを使うかをセット
	// _fogFlg ... 使うならtrue
	void SetFogEnable(bool _fogFlg)
	{
		m_fogFlg = _fogFlg;
		KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogFlg, false);
		KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_FOGCol, m_FOGdensity);
	};

	//値を変えてアンビエントを変更
	// _ambient ... アンビエントの色
	void SetAmbient(const Math::Vector4& _ambient) 
	{
		m_ambient = _ambient;
		KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambient);
	};


	//このゲームにおけるDデフォルトのアンビエントの値に変更
	void SetDefaultAmbient()
	{
		m_ambient = m_defaultAmbient;
		KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_ambient);
	};


private:

	// マネージャーの初期化
	// インスタンス生成(アプリ起動)時にコンストラクタで自動実行
	void Init();


	// シーン切り替え関数
	// _sceneType ... 切り替えるシーン
	void ChangeScene(SceneType _sceneType);

	// 現在のシーンのインスタンスを保持しているポインタ
	std::shared_ptr<BaseScene> m_spCurrentScene = nullptr;

	// 現在のシーンの種類を保持している変数
	SceneType m_currentSceneType = SceneType::Title;

	// 次のシーンの種類を保持している変数
	SceneType m_nextSceneType = m_currentSceneType;


	//自分追加

	//ゲーム結果	
	GameResult m_gameResult;


	//描画先管理
	KdRenderTargetChanger m_rtChanger;


	/////////////////////////////////////
	//フォグ
	Math::Vector3 m_FOGCol = { 0.35,0.35,0.38 };
	//フォグの濃さ
	float m_FOGdensity = 0.02;

	//フォグを使うか
	bool m_fogFlg = false;

	//アンビエント
	Math::Vector4 m_ambient = { 1,1,1,1 };
	//このゲームのデフォルトのアンビエント
	Math::Vector4 m_defaultAmbient = { 1,1,1,1 };

	//フォグとアンビエントの設定をファイルに保存する
	void FogAmbientJsonSave();
	//フォグとアンビエントの設定をファイルから読み込む
	void FogAmbientJsonLoad();

	//フォグとアンビエントの設定のファイルのパス
	std::string m_fogAmbientJsonFilePath = "Asset/Data/ObjeData/FogAmbient/FogAmbientJsonSave.json";


	//プレイヤーの武器
	PlayerWeaponInfo m_playerWeaponInfo = PlayerWeaponInfo::PlayerWeaponInfo_Dagger;

	//装備中の武器をファイルに保存する
	void PlayerWeaponJsonSave();
	//装備中の武器をファイルから読み込む(ファイルが無いときはダガーのまま)
	void PlayerWeaponJsonLoad();

	//装備中の武器のファイルのパス
	std::string m_playerWeaponJsonFilePath = "Asset/Data/PlayerData/PlayerEquipData.json";

private:

	//コンストラクタ(シングルトンなので外から作れない)
	SceneManager() { Init(); }
	//デストラクタ
	~SceneManager() {}

public:

	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}
};
