#pragma once

class CameraBase;
class UIManager;
class HPBar;
class HitDamage;

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
	virtual void OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate);


	virtual void AddUIList(std::shared_ptr<UIManager>_spUIManager);

	void SetHitDamage(std::shared_ptr<HitDamage>_spHitDamage) { m_wpHitDamage = _spHitDamage; }


	/////////////////////////////////////
	//HP

	//MaxのHPを返す
	int GetMaxHP() { return m_status.HP.maxHP; }



	virtual void SetDead();

	/////////////////////////////////////
	//攻撃力

	//基礎攻撃力アップ
	void BoostAttackBase(int _Bosst) { m_status.attck.baseAttckPowe += _Bosst; }

	//基礎攻撃力を返す
	int GetAttackBase() { return m_status.attck.baseAttckPowe; }

	//増加攻撃力アップ
	void BoostAddAttack(int _Bosst) { m_status.attck.addAttack += _Bosst; }

	/////////////////////////////////////
	//防御力

	//基礎防御力アップ
	void BoostDefenseBase(int _Bosst) { m_status.defense.baseDefensePowe += _Bosst; }

	//基礎防御力を返す
	int GetDefenseBase() { return m_status.defense.baseDefensePowe; }

	//増加防御力アップ
	void BoostAddDefense(int _Bosst) { m_status.defense.addDefense += _Bosst; }

	////////////////////////////////////////
	//速度

	float GetNowSpeed() { return m_status.moveSpeed.nowSpeed; }


	//HPバー
	std::shared_ptr<HPBar> GetHPBar() { return m_wpHPBar.lock(); }
private:
	// 衝突判定とそれに伴う座標の更新
	void CollisionUpdate();

	// 解放処理
	void Release();

	//当たり判定をする半径(この円ないならあたり判定)
	const float m_detectRange = 20;


protected:

	//ダメージ計算式
	//ダメージ量を返す
	float DamagecClculationFormula(float _damage, float _ignoreRate);

	std::shared_ptr<KdModelWork> m_spCharaModel = nullptr;
	std::shared_ptr<KdAnimator>	m_spAnimetor = nullptr;


	std::weak_ptr<HitDamage>m_wpHitDamage;


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

		//増加攻撃力(期間限定の攻撃力アップの数値などを入れる)
		float addAttack = 0;
	};

	struct StatusDefense
	{
		//基礎防御力
		float baseDefensePowe = 0;

		//現在防御力
		float nowDefense = 0;

		//増加防御力(期間限定の防御力アップの数値などを入れる)
		float addDefense = 0;
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

	/////////////////////////////
	//ふっとばし
	Math::Vector3 m_knockbackStartPos;
	Math::Vector3 m_knockbackEndPos;
	bool m_isKnockbackFlg = false;


	//何秒で飛ばしきるか
	float m_knockbackSpeed = 1;
	void UpdateKnockback();

	float m_knockbackProgress = 0;


	///////////////////////////////////

	//スイープ判定
	//当たったかどうかを返す(座標の補正もおこなっている)
	bool RaycastFromTo(Math::Vector3 _nextPos);

	///////////////////////////////////
	//UI
	//HPBar
	std::weak_ptr<HPBar>m_wpHPBar;

	bool m_isDead = false;
};
