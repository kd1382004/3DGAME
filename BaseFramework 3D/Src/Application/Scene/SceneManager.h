#pragma once

//各階層での情報
struct DisplayFloorInfo
{
	//階層
	int DisplayFloor = 0;

	//敵を倒した数
	int enemyKillCount = 0;

	//開けた宝箱の数
	int TreasureChestsNum = 0;

	//その階層にいた時間(秒)
	float playTimeSc = 0;
};

//ゲームリザルト
struct GameResult
{
	std::vector<DisplayFloorInfo>displayFloorInfo;

	//プレイヤー死亡
	bool playerDead = false;
};

class BaseScene;

class SceneManager
{
public:

	// シーン情報
	enum class SceneType
	{
		Title,
		Game,
		Result,
	};

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// 次のシーンをセット (次のフレームから切り替わる)
	void SetNextScene(SceneType _nextScene)
	{
		m_nextSceneType = _nextScene;
	}

	// 現在のシーンのオブジェクトリストを取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList();

	// 現在のシーンにオブジェクトを追加
	void AddObject(const std::shared_ptr<KdGameObject>& _obj);

	//追加
	void ImGUI();

	GameResult GetGameResult() { return m_gameResult; }
	void SetGameResult(GameResult gameResult) { m_gameResult = gameResult; }

	//描画先を変更
	void ChangeRendertarget(std::shared_ptr<KdRenderTargetPack> _spRtPack);
	void UndoRenderTarget();

	//初期に戻す
	void UnFogAmbient();
private:

	// マネージャーの初期化
	// インスタンス生成(アプリ起動)時にコンストラクタで自動実行
	void Init();


	// シーン切り替え関数
	void ChangeScene(SceneType _sceneType);

	// 現在のシーンのインスタンスを保持しているポインタ
	std::shared_ptr<BaseScene> m_currentScene = nullptr;

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
	float m_FOGdensity = 0.02;

	bool m_fogFlg = false;

	//アンビエント
	Math::Vector4 m_ambient = { 1,1,1,1 };

	void FogAmbientJsonSave();
	void FogAmbientJsonLode();

	std::string m_FogAmbientJsonFilePath = "Asset/Data/ObjeData/FogAmbient/FogAmbientJsonSave.json";

private:

	SceneManager() { Init(); }
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
