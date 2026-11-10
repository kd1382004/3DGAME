#pragma once

class CharacterBase;
class MapBase;


enum WeponStrengtheningID
{
	//攻撃力
	WeaponsStrengthening_Attck,

	//スタミナ
	WeaponsStrengthening_Stamina,

	//Chargeタイム
	WeaponsStrengthening_ChargeTime,

	//防御無視
	WeaponsStrengthening_DefenseIgnore,

	//武器特有(Lv1になったら他の特有の強化を許す)
	WeaponsStrengthening_Characteristic,

	//武器特有1
	WeaponsStrengthening_Characteristic1,

	//武器特有2
	WeaponsStrengthening_Characteristic2,

	//武器特有3
	WeaponsStrengthening_Characteristic3,

	//武器本体
	WeaponsStrengthening_Body,

	WeaponsStrengtheningSiz
};

// WeponStrengtheningIDと同じ順番にする
const char* const kStrengtheningKeys[WeaponsStrengtheningSiz] =
{
	"attack",               // WeaponsStrengthening_Attck
	"stamina",              // WeaponsStrengthening_Stamina
	"chargeTime",           // WeaponsStrengthening_ChargeTime
	"defenseIgnore",        // WeaponsStrengthening_DefenseIgnore
	"characteristic",       // WeaponsStrengthening_Characteristic
	"characteristic1",      // WeaponsStrengthening_Characteristic1
	"characteristic2",      // WeaponsStrengthening_Characteristic2
	"characteristic3",      // WeaponsStrengthening_Characteristic3
	"body"					// WeaponsStrengthening_Body
};

struct WeaponsStrengtheningInfo
{
	//現在のレベル
	int Lv = 0;

	//最高レベル
	int LvMax = 1;

	//強化用アイテムID
	int strengtheningItem_ID = 0;

	// Lv0→1 に必要な個数
	int   InitialNextLvNum = 3;

	// 1レベルごとの増加率
	float GrowthRate = 1.25f;


	//次の強化に必要なアイテム数
	int nextLvNum = InitialNextLvNum;

	//現在持っている強化用アイテム数
	int strengtheningItemNum = 0;

	//強化が可能か
	bool strengtheningFlg = false;

	//自分のノードID（前提ノードからの参照用。自由に決めてよい）
	int MyNodeID = 0;

	//前提ノードのMyNodeID（-1 = 前提なし）
	int PreconditionNodeID = -1;
};

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

	//武器のSEを停止
	virtual void StopWeaponSE() {};




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



	//////////////////////////////////////////////////////////////
	// 武器強化
	// 配列 m_weaponsStrengtheningInfo は常に「index = 種類(WeponStrengtheningID)」
	// MyNodeID は前提ノードの参照にだけ使う（自由に変更してよい）
	//////////////////////////////////////////////////////////////

	//種類が範囲内か
	bool IsValidType(int _type) const { return _type >= 0 && _type < WeaponsStrengtheningSiz; }

	// MyNodeID から配列index（=種類）を探す。見つからなければ -1
	int FindIndexByNodeID(int _nodeID) const
	{
		for (int i = 0; i < WeaponsStrengtheningSiz; ++i)
		{
			if (m_weaponsStrengtheningInfo[i].MyNodeID == _nodeID) { return i; }
		}
		return -1;
	}

	// 前提ノードが解放済みか（前提なし(-1)なら true）
	bool IsPreconditionCleared(int _type) const
	{
		if (!IsValidType(_type)) { return false; }

		const int pre = m_weaponsStrengtheningInfo[_type].PreconditionNodeID;
		if (pre < 0) { return true; }

		const int preIndex = FindIndexByNodeID(pre);
		if (preIndex < 0) { return false; } // 参照先がない＝データのミス

		return m_weaponsStrengtheningInfo[preIndex].Lv > 0;
	}

	//武器の強化ができたかどうか
	bool AddWeaponsStrengtheningInfo(int _type, int _lv = 1)
	{
		if (!IsWeaponsStrengtheningPossible(_type)) { return false; }

		WeaponsStrengtheningInfo& info = m_weaponsStrengtheningInfo[_type];
		info.Lv += _lv;
		info.strengtheningItemNum -= info.nextLvNum;
		GetRequiredItemNum(_type);

		return true;
	}

	int GetWeaponsStrengtheningLv(int _type) const
	{
		if (!IsValidType(_type)) { return 0; }
		return m_weaponsStrengtheningInfo[_type].Lv;
	}

	int GetWeaponsStrengtheningNextLvNum(int _type) const
	{
		if (!IsValidType(_type)) { return 0; }
		return m_weaponsStrengtheningInfo[_type].nextLvNum;
	}

	// 強化できるか（前提ノード・最大レベル・アイテム数をまとめて判定）
	bool IsWeaponsStrengtheningPossible(int _type)
	{
		if (!IsValidType(_type)) { return false; }

		WeaponsStrengtheningInfo& info = m_weaponsStrengtheningInfo[_type];
		info.strengtheningFlg =
			IsPreconditionCleared(_type) &&
			info.Lv < info.LvMax &&
			info.strengtheningItemNum >= info.nextLvNum;

		return info.strengtheningFlg;
	}

	bool GetWeaponsStrengtheningLVMaxFlg(int _type) const
	{
		if (!IsValidType(_type)) { return false; }
		return m_weaponsStrengtheningInfo[_type].Lv >= m_weaponsStrengtheningInfo[_type].LvMax;
	}

	void SetWeaponsStrengtheningNum(int _type, int _strengtheningItemNum)
	{
		if (!IsValidType(_type)) { return; }
		m_weaponsStrengtheningInfo[_type].strengtheningItemNum = _strengtheningItemNum;
		IsWeaponsStrengtheningPossible(_type);
	}

	float GetChargeAttackMaxCost()
	{
		float lv = (float)m_weaponsStrengtheningInfo[WeaponsStrengthening_Stamina].Lv;
		float decay = powf(0.9f, lv);
		return m_m_chargeAttackMaxCost * decay;
	}

	bool GetChargeTimeMaxFlg() { return m_chargeTimeMaxFlg; }


	float GetAddCargeTime()
	{
		int lv = m_weaponsStrengtheningInfo[WeaponsStrengthening_ChargeTime].Lv;

		return m_addCargeTime * powf(1.05f, lv);;
	}


	const WeaponsStrengtheningInfo& GetWeaponsStrengtheningInfo(int _type) const
	{
		if (!IsValidType(_type)) { return m_weaponsStrengtheningInfo[0]; }
		return m_weaponsStrengtheningInfo[_type];
	}

	// 種類の順（kStrengtheningKeys の順）で受け取る。並べ替えはしない
	void SetWeaponsStrengtheningInfo(std::vector<WeaponsStrengtheningInfo> weaponsStrengtheningInfo)
	{
		for (int i = 0; i < WeaponsStrengtheningSiz; ++i)
		{
			if (i < weaponsStrengtheningInfo.size())
			{
				m_weaponsStrengtheningInfo[i] = weaponsStrengtheningInfo[i];
			}
			else
			{
				m_weaponsStrengtheningInfo[i] = WeaponsStrengtheningInfo();
			}
		}
	}

	const std::vector<WeaponsStrengtheningInfo> GetWeaponsStrengtheningInfoList()
	{
		std::vector<WeaponsStrengtheningInfo> list;
		for (int i = 0; i < WeaponsStrengtheningSiz; ++i)
		{
			list.push_back(m_weaponsStrengtheningInfo[i]);
		}

		return list;
	}

	void SetAnimePercentNow(float _percent) { m_animePercentNow = _percent; }

protected:

	// 現在レベル lv から lv+1 に上げるのに必要な個数
	void GetRequiredItemNum(int index)
	{
		m_weaponsStrengtheningInfo[index].nextLvNum = static_cast<int>(std::ceil(m_weaponsStrengtheningInfo[index].InitialNextLvNum * std::pow(m_weaponsStrengtheningInfo[index].GrowthRate, m_weaponsStrengtheningInfo[index].Lv)));
	}

	//1秒当りにたまるCharge時間
	float m_addCargeTime = 1;

	bool m_chargeTimeMaxFlg = false;
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

	//Charge攻撃を使うのに使う1秒当りのスタミナコスト値
	float m_m_chargeAttackMaxCost = 10;

	//Charge攻撃が当たっていい敵の数
	int m_hitNum = 1;

	//進んで良い最大の距離
	float m_maxDistanceM = 100;

	//1秒あたりに進む距離
	float m_chargeAttackSpeed = 30;

	//最大ダメージ
	float m_chargeAttackMaxdamage = 100;

	WeaponsStrengtheningInfo m_weaponsStrengtheningInfo[WeaponsStrengtheningSiz];

	//防御無視の値
	float m_defenseIgnore = 0;


	//トレイルポリゴン (軌跡) ポリゴン
	std::shared_ptr<KdTrailPolygon> m_tPoly = nullptr;
	bool m_tPolyFlg = false;



	float m_animePercentNow;
	float m_animePercentPolyStart;
	float m_animePercentPolyEnd;
};

