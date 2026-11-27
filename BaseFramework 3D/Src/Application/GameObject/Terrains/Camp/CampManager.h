#pragma once

#include"../../../Scene/SceneManager.h"		//武器の種類(PlayerWeaponInfo)
#include"CampLayoutData/CampLayoutData.h"	//キャンプの配置データ
#include"MerchantReaction.h"					//商人の反応

class PrepareCamera;
class CampObject;
class CampLight;


//////////////////////////////////////////////////////////////
// 準備画面の背景(キャンプ)の管理
//  ・CampLayout.json / CampLayoutTune.json を読んで、置物・ライト・カメラを作る
//  ・焚き火の炎と音、入口の奥のエフェクト
//  ・武器立ての武器の表示
//  ・出発の演出(プレイヤーが入口へ歩く → 暗転)
//  ・エディタから置物・ライトを足す / 消す / 保存する
//
// キャンプのオブジェクト(カメラ・置物・ライト)は自分の m_objList で持ち、
// 更新と描画はここから呼ぶ(EnemyManager・MapManager と同じ形)
// シーンの m_objList にはこのマネージャーだけを入れる
//////////////////////////////////////////////////////////////
class CampManager :public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	CampManager() {}

	//デストラクタ(焚き火の音とエフェクトを止める)
	~CampManager()override;

	//---- 基本の処理 ----

	//キャンプを作る(配置データを読み、カメラ・置物・ライト・エフェクトを出す)
	void Init()override;

	//更新前の処理(消えたオブジェクトを一覧から外す)
	void PreUpdate()override;

	//更新
	void Update()override;

	//更新後の処理(ライトの登録など)
	void PostUpdate()override;

	//描画前の処理(カメラをシェーダーに渡す)
	void PreDraw()override;

	//影の描画
	void GenerateDepthMapFromLight()override;

	//ライトなしの描画
	void DrawUnLit()override;

	//ライトありの描画
	void DrawLit()override;

	//エフェクトの描画
	void DrawEffect()override;

	//光る部分の描画
	void DrawBright()override;

	//2Dの描画
	void DrawSprite()override;

	//デバッグ表示(当たり判定など)
	void DrawDebug()override;

	//---- 出発の演出 ----

	//出発の演出を始める(プレイヤーを入口へ向けて走らせる)
	void StartDepart();

	//出発の演出を進める(毎フレーム)
	//戻り値 ... 真っ暗になってゲームへ移るときtrue
	bool UpdateDepart();

	//出発の演出中か
	//戻り値 ... 演出中ならtrue
	bool IsDeparting() const { return m_departState != DepartState::None; }

	//出発の暗転に合わせた音量の倍率(暗転していなければ1、真っ暗で0)
	//戻り値 ... 0〜1
	float GetDepartVolumeRate() const;

	//---- その他の処理 ----

	//カメラを動かす
	// _name      ... 行き先のカメラの名前("Explore" など)
	// _immediate ... trueなら瞬間移動
	void MoveCamera(const std::string& _name, bool _immediate = false);

	//武器立ての武器の表示を更新する
	//持っている武器だけを出し、装備中の武器は(プレイヤーが持っているので)隠す
	// _owned ... 武器の種類ごとに持っているか
	// _equip ... 装備中の武器
	void UpdateRackWeapons(const std::vector<bool>& _owned, int _equip);

	//焚き火の音の音量を合わせる
	// _rate ... 効果音の音量に掛ける倍率
	void ApplyCampfireVolume(float _rate);

	//---- エディタ用 ----

	//置物を足す(焚き火なら炎と音も出す)
	// _data ... 置物のデータ
	//戻り値 ... 足した置物
	std::shared_ptr<CampObject> AddCampObject(const CampObjectData& _data);

	//置物を消す
	// _spObj ... 消す置物
	void RemoveCampObject(const std::shared_ptr<CampObject>& _spObj);

	//置物を動かした後に呼ぶ(焚き火なら炎と音もついてくる)
	// _spObj ... 動かした置物
	void OnCampObjectMoved(const std::shared_ptr<CampObject>& _spObj);

	//ライトを足す
	// _data ... ライトのデータ
	//戻り値 ... 足したライト
	std::shared_ptr<CampLight> AddCampLight(const CampLightData& _data);

	//ライトを消す
	// _spLight ... 消すライト
	void RemoveCampLight(const std::shared_ptr<CampLight>& _spLight);

	//置物とライトの配置を CampLayout.json に保存する(Blender を使わずに配置を決めるため)
	//戻り値 ... 保存できたらtrue
	bool SaveLayout();

	//---- 取得 ----

	//準備画面のカメラ
	const std::shared_ptr<PrepareCamera>& GetCamera() const { return m_spCamera; }

	//置物の一覧
	const std::vector<std::shared_ptr<CampObject>>& GetCampObjects() const { return m_campObjects; }

	//ライトの一覧
	const std::vector<std::shared_ptr<CampLight>>& GetCampLights() const { return m_campLights; }

	//出発の演出のデータ(エディタで書き換える)
	CampDepartData& WorkDepartData() { return m_layout.m_depart; }

	//入口の奥のエフェクト
	const std::weak_ptr<KdEffekseerObject>& GetEntranceEffect() const { return m_wpEntranceEffect; }

	//キャンプのプレイヤー(種類 Player の置物)
	std::shared_ptr<CampObject> GetCampPlayer() const { return m_wpCampPlayer.lock(); }

	//配置データのパス
	const std::string& GetLayoutFilePath() const { return m_layoutFilePath; }

	//エディタで調整した値(カメラ・ライトの明るさ・出発)のパス
	const std::string& GetTuneFilePath() const { return m_tuneFilePath; }


	//商人に、買った・売った・お金が足りないときの動きをさせる(終わったら待機に戻る)
	// _reaction ... 何が起きたか
	void PlayMerchantReaction(MerchantReaction _reaction);
private:

	//---- 型 ----

	//出発演出の状態(None:していない / Walk:入口へ歩いている / Fade:暗転中)
	enum class DepartState { None, Walk, Fade };

	//焚き火1個分の炎と音(置物を動かしたらついていく)
	struct Campfire
	{
		std::weak_ptr<CampObject>             m_wpObj;	//焚き火の置物
		std::weak_ptr<KdEffekseerObject>      m_wpEffect;	//炎
		std::shared_ptr<KdSoundInstance3D>    m_spSE;		//パチパチ音
	};

	//---- メンバ変数 ----

	//置物とライトの配置データのパス(エディタの「配置を保存」で上書きする)
	const std::string m_layoutFilePath = "Asset/Data/ObjeData/PrepareObjeData/CampLayout.json";

	//エディタで調整した値(カメラ・ライトの明るさ・出発演出)のパス
	const std::string m_tuneFilePath = "Asset/Data/ObjeData/PrepareObjeData/CampLayoutTune.json";

	//キャンプの配置データ
	CampLayoutData m_layout;

	//キャンプのオブジェクト全部(カメラ・置物・ライト)。更新と描画はこの一覧で回す
	std::list<std::shared_ptr<KdGameObject>> m_objList;

	//準備画面のカメラ
	std::shared_ptr<PrepareCamera> m_spCamera;

	//置物(m_objList にも入っている。エディタ・武器立てで種類ごとに使う)
	std::vector<std::shared_ptr<CampObject>> m_campObjects;

	//ライト(m_objList にも入っている。エディタで使う)
	std::vector<std::shared_ptr<CampLight>> m_campLights;

	//出さないけれど保存のときに残す置物(松明は自分で光るので出していない)
	std::vector<CampObjectData> m_hiddenObjects;

	//出さないけれど保存のときに残すライト(松明のライト)
	std::vector<CampLightData> m_hiddenLights;

	//焚き火の炎と音
	std::vector<Campfire> m_campfires;

	//入口の奥のエフェクト(ダンジョンの階段と同じもの)
	std::weak_ptr<KdEffekseerObject> m_wpEntranceEffect;

	//武器立てに置いてある武器(武器の種類ごと。置いていない武器は空)
	std::weak_ptr<CampObject> m_wpRackWeapons[PlayerWeaponInfoSize];

	//キャンプに立っているプレイヤーの置物(出発の演出で歩かせる)
	std::weak_ptr<CampObject> m_wpCampPlayer;

	//ショップの商人(種類 Merchant の置物。買った・売ったときに動かす)
	std::weak_ptr<CampObject> m_wpMerchant;

	//焚き火の音の今の倍率(動かして鳴らし直すときに使う)
	float m_campfireVolRate = 1.0f;

	// 準備画面(屋外・夕方)の霧と環境光
	// ダンジョン用の値(霧 0.3 灰色・濃さ 0.02 / 環境光 0.5)のままだと暗く濁るので、準備画面だけ変える
	// 次のシーンに移るときは SceneManager::ChangeScene がダンジョン用の値に戻すので、戻す処理は要らない
	Math::Vector3 m_campFogColor   = { 0.32f, 0.36f, 0.48f };		// 夕方の青っぽい空気
	float         m_campFogDensity = 0.012f;						// 奥の崖がうっすら霞むくらい
	Math::Vector4 m_campAmbient    = { 0.6f, 0.62f, 0.75f, 0.5f };	// 少し青寄り。オレンジの光と対比させる

	//---- 出発演出のメンバ変数 ----

	DepartState   m_departState     = DepartState::None;
	float         m_departTimer     = 0.0f;					//歩き始めてからの秒数
	float         m_departFadeTimer = 0.0f;					//暗転を始めてからの秒数
	float         m_departStepTimer = 0.0f;					//次の足音までの秒数
	Math::Vector3 m_departStartPos  = Math::Vector3::Zero;	//歩き始めた位置

	//足音の間隔(秒)。走るアニメーションに合わせる
	static constexpr float kDepartStepInterval = 0.32f;

	//---- その他の処理 ----

	//焚き火の炎と音を出す
	// _spObj ... 焚き火の置物
	void StartCampfire(const std::shared_ptr<CampObject>& _spObj);

	//焚き火の炎と音を止める
	// _fire ... 止める焚き火
	void StopCampfire(Campfire& _fire);

	//武器立ての武器・プレイヤーなら覚えておく
	// _spObj ... 置物
	void RegisterSpecialObject(const std::shared_ptr<CampObject>& _spObj);
};
