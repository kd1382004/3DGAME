#pragma once

class CameraBase;

class CharacterBase : public KdGameObject
{
public:
	CharacterBase() {};
	~CharacterBase()	override { Release(); };

	void Init()			override;
	void Update()		override;
	void PostUpdate()	override;

	void GenerateDepthMapFromLight()override;
	void DrawLit()		override;

	void ImGUI()override;


	void SetPos(const Math::Vector3& pos)override
	{
		m_pos = pos;
		m_mWorld.Translation(m_pos);
	}


	Math::Matrix GetWeaponParentMatrix()
	{
		return m_weponParentMatrix;
	}

	void SetCamera(const std::shared_ptr<CameraBase>& _camera) { m_wpCamera = _camera; }


	//攻撃をくらった処理
	//_damage ... ダメージ量
	//_knockbackDistance ... ふっとばし距離
	//_knockbackDir ... ふっとばし方向
	// _hitStunTime ... のけぞり時間
	//_isCritical ... クリティカルかどうか
	//__ignoreRate ... 防御無視
	void OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate);
private:
	// 衝突判定とそれに伴う座標の更新
	void CollisionUpdate();

	// 解放処理
	void Release();

	//当たり判定をする半径(この円ないならあたり判定)
	const float m_detectRange = 10;


protected:

	//ダメージ計算式
	//ダメージ量を返す
	float DamagecClculationFormula(float _damage,float _ignoreRate);

	std::shared_ptr<KdModelWork> m_spCharaModel = nullptr;
	std::shared_ptr<KdAnimator>	m_spAnimetor = nullptr;



	float m_Gravity = 0;

	float m_gravityPower = 0.12f;

	///////////////////////////////////////////
	//キャラステータス

	//キャラステータスが入ってるファイル名
	std::string m_charaStatusFilePath;

	struct StatusHP
	{
		//基礎体力
		float baseHP = 0;

		//最大体力
		float maxHP = 0;

		//現在体力
		float nowHP = 0;
	};

	struct StatusAttck
	{
		//基礎攻撃力
		float baseAttckPowe = 0;

		//現在攻撃力
		float nowAttck = 0;
	};

	struct StatusDefense
	{
		//基礎防御力
		float baseDefensePowe = 0;

		//現在防御力
		float nowDefense = 0;
	};

	struct StatusMoveSpeed
	{
		//基礎速度
		float baseSpeed = 0;

		//歩き速度(基礎速度に足す値)
		float walkMovePowe = 0;

		//ダッシュ速度(基礎速度に足す値)
		float runMovePowe = 0;

		//現在の速度
		float nowSpeed = 0;
	};

	struct Status
	{
		//体力
		StatusHP HP;

		// 攻撃力
		StatusAttck attck;

		//防御力
		StatusDefense defense;

		//スピード
		StatusMoveSpeed moveSpeed;
	};

	//ステータス
	Status m_status;

	//_filePath ... ファイルパスを書く
	void LoadCharaStatus(std::string _filePath);
	void SaveCharaStatus(std::string _filePath);
	///////////////////////////////////////////

	///////////////////////////////////////////

	//座標
	Math::Vector3 m_pos;

	//サイズ
	float m_siz;

	//移動方向
	Math::Vector3 m_moveVec;

	///////////////////////////////////////////

	///////////////////////////////////////////
	//キャラの回転
	//角度
	float m_angle = 0;

	void AngeleUpdate();

	///////////////////////////////////////////

	///////////////////////////////////////////
	//ステータスエディターに表示する
	virtual void StatusEditor();

	bool m_statusEditorFlf = false;

	std::string m_statusEditorName = "キャラクター";

	///////////////////////////////////////////

	///////////////////////////////////////////

	//地面と当たってるか
	bool m_groundHit = false;

	//壁と当たってるか
	bool m_wallHit = false;

	///////////////////////////////////////////
	//武器用

	Math::Matrix m_weponParentMatrix;

	///////////////////////////////////////////
	//カメラ
	std::weak_ptr<CameraBase>m_wpCamera;



	///////////////////////////
	//のけぞり時間
	float m_hitStunTimer;
	bool m_hitStunFlg = false;
};
