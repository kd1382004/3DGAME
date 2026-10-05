#pragma once

class CharacterBase;
class MapBase;

class WeaponBase :public KdGameObject
{
public:
	WeaponBase() {};
	~WeaponBase() override {};


	void Init()override;
	void Update()override;
	void DrawLit()override;

	//武器の当たり判定を作る
	void CreateWeaponHitCollider();

	void SetParentMatrix(Math::Matrix _mat) { m_weponParentMat = _mat; }
	void SetParentRotation(Math::Vector3 _rot) { m_weponParentRot = _rot; }

	virtual	void SetAttackFlg(bool _flg);

	void AddAttackHitCharacterList(std::shared_ptr<CharacterBase>_character);
	void AddObjList(std::shared_ptr<MapBase>_character);

	void ClearAttackHitCharacterList() { m_attackHitCharacterList.clear(); }
	void ClearObjList() { m_objList.clear(); }

	//attackHitCharacterListの中から一番近い対象の座標を返す
	Math::Vector3 GetCloseAttackHitCharacter();

	void SetCharacterAttackPower(float _power) { m_characterAttackPower = _power; }

	virtual	void SetNowChargeTime(float _time)
	{
		m_chargeTime = _time;
		if (m_chargeTime >= m_chargeTimeMax) { m_chargeTime = m_chargeTimeMax; }

	}

	void SetAngle(float _angle) { m_attackAngle = _angle; }

	//Charge攻撃をするときに呼び出す
	virtual void ChargAttackPlay() {};




	// Charge攻撃が当たっていい敵の数
	void SetHitNum(int hitNum)
	{
		m_hitNum = hitNum;
	}

	// 進んで良い最大の距離（メートル）
	void SetMaxDistance(float maxDistanceM)
	{
		m_maxDistanceM = maxDistanceM;
	}

	// 1秒あたりに進む距離（速度）
	void SetChargeAttackSpeed(float speed)
	{
		m_chargeAttackSpeed = speed;
	}

	// 最大ダメージ
	void SetChargeAttackMaxDamage(float maxDamage)
	{
		m_chargeAttackMaxdamage = maxDamage;
	}



	//武器の情報
	//武器の強化ができたかどうか
	bool AddWeaponsStrengtheningInfo(int id, int lv = 1)
	{
		if (id < 0 || id >= WeaponsStrengtheningSiz){ return false; }
		if (!m_weaponsStrengtheningInfo[id].strengtheningFlg) { return false; }
		m_weaponsStrengtheningInfo[id].Lv += lv;
		m_weaponsStrengtheningInfo[id].strengtheningItemNum -= m_weaponsStrengtheningInfo[id].nextLvNum;
		m_weaponsStrengtheningInfo[id].nextLvNum++;
		return true;
	}

	int GetWeaponsStrengtheningLv(int id)
	{
		if (id < 0 || id >= WeaponsStrengtheningSiz) return 0;
		return m_weaponsStrengtheningInfo[id].Lv;
	}

	int GetWeaponsStrengtheningNextLvNum(int id)
	{
		if (id < 0 || id >= WeaponsStrengtheningSiz) return 0;
		return m_weaponsStrengtheningInfo[id].nextLvNum;
	}

	bool IsWeaponsStrengtheningPossible(int id)
	{
		if (id < 0 || id >= WeaponsStrengtheningSiz) return false;

		if (m_weaponsStrengtheningInfo[id].strengtheningItemNum >= m_weaponsStrengtheningInfo[id].nextLvNum)
		{
			m_weaponsStrengtheningInfo[id].strengtheningFlg = true;
		}
		else
		{
			m_weaponsStrengtheningInfo[id].strengtheningFlg = false;
		}

		return m_weaponsStrengtheningInfo[id].strengtheningFlg;
	}

	void SetWeaponsStrengtheningNum(int id,int strengtheningItemNum)
	{
		if (id < 0 || id >= WeaponsStrengtheningSiz) return;
		m_weaponsStrengtheningInfo[id].strengtheningItemNum = strengtheningItemNum;
		IsWeaponsStrengtheningPossible(id);
	}

protected:


	float m_attackAngle = 0;
	float m_chargeTime = 0;
	float m_chargeTimeMax = 0;


	float m_characterAttackPower = 0;

	//攻撃判定を出すか
	bool m_attackFlg = false;

	//あたり判定をするキャラリスト
	std::list<std::weak_ptr<CharacterBase>>m_attackHitCharacterList;

	//既に当たったキャラリスト
	std::list<std::weak_ptr<CharacterBase>>m_hitCharactersList;

	//あたり判定をするオブジェリスト
	std::list<std::weak_ptr<MapBase>>m_objList;



	std::shared_ptr<KdModelWork> m_spWeaponModel = nullptr;

	//武器のローカル
	Math::Matrix m_localMat = Math::Matrix::Identity;
	Math::Vector3 m_localPos = Math::Vector3::Zero;


	//武器の親
	Math::Matrix m_weponParentMat = Math::Matrix::Identity;
	Math::Vector3 m_weponParentRot = Math::Vector3::Zero;

	//武器ステータスが入ってるファイル名
	std::string m_WeaponStatusFilePath;
	void LoadWeaponStatus(const std::string& filePath);
	void SaveWeaponStatus(const std::string& filePath);

	struct WeaponStatus
	{
		// --- 基本ステータス ---
		float attackPower = 0.0f;       // 攻撃力
		float attackSpeed = 1.0f;       // 攻撃速度（モーションの速さ）
		float critRate = 0.0f;          // クリティカル率（0〜1）
		float critDamage = 0.0f;		// クリティカル倍率（1.0 = 100%）

		// --- 武器の性質 ---
		float range = 1.0f;              // 当たり判定の長さ
		float weight = 1.0f;             // 重さ（硬直に影響）
		float knockback = 0.0f;          // ノックバック力
		float poiseBreak = 0.0f;         // ガード崩し値
		float stunPower = 0.0f;          // スタン値

		// --- モーション補正 ---
		float startup = 0.05f;            // 攻撃開始フレーム補正
		float recovery = 0.0f;           // 攻撃後硬直補正
		float cancelWindow = 0.0f;       // キャンセル可能フレーム補正
	};

	//武器共通基本ステータス
	WeaponStatus m_baseWeaponStatus;


	//座標補正の距離
	float m_aimAssistRadius = 10;




	////////////////////////////////////////////////
	//Charge攻撃パラメーター

	//Charge攻撃を使うのに使うスタミナコスト
	float m_chargeAttackCost = 0;


	//Charge攻撃を使うのに使うスタミナコストのマックス値
	float m_m_chargeAttackMaxCost = 20;

	//Charge攻撃が当たっていい敵の数
	int m_hitNum = 1;

	//進んで良い最大の距離
	float m_maxDistanceM = 100;

	//1秒あたりに進む距離
	float m_chargeAttackSpeed = 30;

	//最大ダメージ
	float m_chargeAttackMaxdamage = 100;

	enum WeponStrengtheningID
	{
		//攻撃力
		WeaponsStrengthening_Attck,

		//スタミナ
		WeaponsStrengthening_Stamina,

		//Chargeタイム
		WeaponsStrengthening_ChargeTime,

		//衝撃波攻撃(これはLV1がマックスでLv1になったらこれ以下の強化を許す)
		WeaponsStrengthening_ShockWave,

		//衝撃波の飛距離
		WeaponsStrengthening_ShockWave_maxDistanceM,

		//衝撃波の速度
		WeaponsStrengthening_ShockWave_speed,

		//衝撃波の貫通力(何体まで当たっていいか)
		WeaponsStrengthening_ShockWave_hitNum,

		WeaponsStrengtheningSiz
	};

	struct WeaponsStrengtheningInfo
	{
		int Lv = 0;
		int LvMax = 0;
		int nextLvNum = 1;
		int strengtheningItemNum = 0;
		bool strengtheningFlg = false;
	};

	WeaponsStrengtheningInfo m_weaponsStrengtheningInfo[WeaponsStrengtheningSiz];
};

