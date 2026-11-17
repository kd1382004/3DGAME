#pragma once
#include"ComboStep/ComboStep.h"


class CharacterBase;
class MapBase;


enum WeaponStrengtheningID
{
	//攻撃力
	WeaponsStrengthening_Attack,

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

// WeaponStrengtheningIDと同じ順番にする
const char* const kStrengtheningKeys[WeaponsStrengtheningSiz] =
{
	"attack",               // WeaponsStrengthening_Attack
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
	int m_lv = 0;

	//最高レベル
	int m_lvMax = 1;

	//強化用アイテムID
	int m_strengtheningItemID = 0;

	// Lv0→1 に必要な個数
	int   m_initialNextLvNum = 3;

	// 1レベルごとの増加率
	float m_growthRate = 1.25f;


	//次の強化に必要なアイテム数
	int m_nextLvNum = m_initialNextLvNum;

	//現在持っている強化用アイテム数
	int m_strengtheningItemNum = 0;

	//強化が可能か
	bool m_strengtheningFlg = false;

	//自分のノードID（前提ノードからの参照用。自由に決めてよい）
	int m_myNodeID = 0;

	//前提ノードのMyNodeID（-1 = 前提なし）
	int m_preconditionNodeID = -1;
};

class WeaponBase :public KdGameObject
{
public:
	//コンストラクタ
	WeaponBase() {};
	//デストラクタ
	~WeaponBase() override {};


	//初期化
	void Init()override;
	//更新
	void Update()override;
	//ライトありの描画
	void DrawLit()override;

	//武器の当たり判定を作る
	void CreateWeaponHitCollider();

	//親(持ち手)の行列をセット
	// _mat ... 親の行列
	void SetParentMatrix(Math::Matrix _mat) { m_weaponParentMat = _mat; }
	//親の回転をセット
	// _rot ... 回転(度)
	void SetParentRotation(Math::Vector3 _rot) { m_weaponParentRot = _rot; }

	//攻撃判定を出すかをセット
	// _flg ... 出すか
	virtual	void SetAttackFlg(bool _flg);

	//当たり判定をするキャラを追加
	// _spCharacter ... 追加するキャラ
	void AddAttackHitCharacterList(std::shared_ptr<CharacterBase>_spCharacter);
	//当たり判定をするマップオブジェクトを追加
	// _spObj ... 追加するマップオブジェクト
	void AddObjList(std::shared_ptr<MapBase>_spObj);

	//当たり判定をするキャラを全部消す
	void ClearAttackHitCharacterList() { m_attackHitCharacterList.clear(); }
	//当たり判定をするマップオブジェクトを全部消す
	void ClearObjList() { m_objList.clear(); }

	//attackHitCharacterListの中から一番近い対象の座標を返す
	// _origin ... 探す中心の座標
	std::optional<Math::Vector3>FindAimAssistTarget(const Math::Vector3& _origin) const;

	//持ち主の攻撃力をセット
	// _power ... 攻撃力
	void SetCharacterAttackPower(float _power) { m_characterAttackPower = _power; }

	//溜めている時間をセット(最大で止める)
	// _time ... 溜めている時間
	virtual	void SetNowChargeTime(float _time)
	{
		m_chargeTime = _time;
		if (m_chargeTime >= m_chargeTimeMax) { m_chargeTime = m_chargeTimeMax; }

	}

	//攻撃の角度をセット
	// _angle ... 角度(度)
	void SetAngle(float _angle) { m_attackAngle = _angle; }

	//Charge攻撃をするときに呼び出す
	virtual void ChargeAttackPlay() {};

	//武器のSEを停止
	virtual void StopWeaponSE() {};

	//攻撃力を返す
	int GetAttack();

	// Charge攻撃が当たっていい敵の数
	// _hitNum ... 当たっていい敵の数
	void SetHitNum(int _hitNum)
	{
		m_hitNum = _hitNum;
	}

	// 進んで良い最大の距離（メートル）
	// _maxDistanceM ... 距離(メートル)
	void SetMaxDistance(float _maxDistanceM)
	{
		m_maxDistanceM = _maxDistanceM;
	}

	// 1秒あたりに進む距離（速度）
	// _speed ... 速度
	void SetChargeAttackSpeed(float _speed)
	{
		m_chargeAttackSpeed = _speed;
	}

	// 最大ダメージ
	// _maxDamage ... 最大ダメージ
	void SetChargeAttackMaxDamage(float _maxDamage)
	{
		m_chargeAttackMaxDamage = _maxDamage;
	}



	//////////////////////////////////////////////////////////////
	// 武器強化
	// 配列 m_weaponsStrengtheningInfo は常に「index = 種類(WeaponStrengtheningID)」
	// MyNodeID は前提ノードの参照にだけ使う（自由に変更してよい）
	//////////////////////////////////////////////////////////////

	//種類が範囲内か
	// _type ... 強化の種類
	bool IsValidType(int _type) const { return _type >= 0 && _type < WeaponsStrengtheningSiz; }

	// MyNodeID から配列index（=種類）を探す。見つからなければ -1
	// _nodeID ... ノードID
	int FindIndexByNodeID(int _nodeID) const
	{
		for (int i = 0; i < WeaponsStrengtheningSiz; ++i)
		{
			if (m_weaponsStrengtheningInfo[i].m_myNodeID == _nodeID) { return i; }
		}
		return -1;
	}

	// 前提ノードが解放済みか（前提なし(-1)なら true）
	// _type ... 強化の種類
	bool IsPreconditionCleared(int _type) const
	{
		if (!IsValidType(_type)) { return false; }

		const int pre = m_weaponsStrengtheningInfo[_type].m_preconditionNodeID;
		if (pre < 0) { return true; }

		const int preIndex = FindIndexByNodeID(pre);
		if (preIndex < 0) { return false; } // 参照先がない＝データのミス

		return m_weaponsStrengtheningInfo[preIndex].m_lv > 0;
	}

	//武器の強化ができたかどうか
	// _type ... 強化の種類
	// _lv ... 上げるレベル
	bool AddWeaponsStrengtheningInfo(int _type, int _lv = 1)
	{
		if (!IsWeaponsStrengtheningPossible(_type)) { return false; }

		WeaponsStrengtheningInfo& info = m_weaponsStrengtheningInfo[_type];
		info.m_lv += _lv;
		info.m_strengtheningItemNum -= info.m_nextLvNum;
		GetRequiredItemNum(_type);

		return true;
	}

	//強化レベルを返す
	// _type ... 強化の種類
	//戻り値 ... 強化レベル
	int GetWeaponsStrengtheningLv(int _type) const
	{
		if (!IsValidType(_type)) { return 0; }
		return m_weaponsStrengtheningInfo[_type].m_lv;
	}

	//次のレベルに必要なアイテム数を返す
	// _type ... 強化の種類
	//戻り値 ... 必要なアイテム数
	int GetWeaponsStrengtheningNextLvNum(int _type) const
	{
		if (!IsValidType(_type)) { return 0; }
		return m_weaponsStrengtheningInfo[_type].m_nextLvNum;
	}

	// 強化できるか（前提ノード・最大レベル・アイテム数をまとめて判定）
	// _type ... 強化の種類
	bool IsWeaponsStrengtheningPossible(int _type)
	{
		if (!IsValidType(_type)) { return false; }

		WeaponsStrengtheningInfo& info = m_weaponsStrengtheningInfo[_type];
		info.m_strengtheningFlg =
			IsPreconditionCleared(_type) &&
			info.m_lv < info.m_lvMax &&
			info.m_strengtheningItemNum >= info.m_nextLvNum;

		return info.m_strengtheningFlg;
	}

	//強化レベルが最大か
	// _type ... 強化の種類
	//戻り値 ... 最大ならtrue
	bool GetWeaponsStrengtheningLVMaxFlg(int _type) const
	{
		if (!IsValidType(_type)) { return false; }
		return m_weaponsStrengtheningInfo[_type].m_lv >= m_weaponsStrengtheningInfo[_type].m_lvMax;
	}

	//強化アイテムの所持数をセットして、強化できるか更新する
	// _type ... 強化の種類
	// _strengtheningItemNum ... アイテムの所持数
	void SetWeaponsStrengtheningNum(int _type, int _strengtheningItemNum)
	{
		if (!IsValidType(_type)) { return; }
		m_weaponsStrengtheningInfo[_type].m_strengtheningItemNum = _strengtheningItemNum;
		IsWeaponsStrengtheningPossible(_type);
	}

	//溜め攻撃の1秒当たりのスタミナコストを返す(スタミナ強化で減る)
	//戻り値 ... スタミナコスト
	float GetChargeAttackMaxCost()
	{
		float lv = (float)m_weaponsStrengtheningInfo[WeaponsStrengthening_Stamina].m_lv;
		float decay = powf(0.9f, lv);
		return m_chargeAttackMaxCost * decay;
	}

	//溜め時間が最大になったか
	//戻り値 ... 最大ならtrue
	bool GetChargeTimeMaxFlg() { return m_chargeTimeMaxFlg; }


	//1秒当たりにたまる溜め時間を返す(強化込み)
	//戻り値 ... 1秒当たりの溜め時間
	float GetAddChargeTime()
	{
		int lv = m_weaponsStrengtheningInfo[WeaponsStrengthening_ChargeTime].m_lv;

		return m_addChargeTime * powf(1.05f, lv);;
	}


	//強化情報を返す
	// _type ... 強化の種類
	//戻り値 ... 強化情報
	const WeaponsStrengtheningInfo& GetWeaponsStrengtheningInfo(int _type) const
	{
		if (!IsValidType(_type)) { return m_weaponsStrengtheningInfo[0]; }
		return m_weaponsStrengtheningInfo[_type];
	}

	// 種類の順（kStrengtheningKeys の順）で受け取る。並べ替えはしない
	// _weaponsStrengtheningInfo ... 強化情報の一覧
	void SetWeaponsStrengtheningInfo(std::vector<WeaponsStrengtheningInfo> _weaponsStrengtheningInfo)
	{
		for (int i = 0; i < WeaponsStrengtheningSiz; ++i)
		{
			if (i < _weaponsStrengtheningInfo.size())
			{
				m_weaponsStrengtheningInfo[i] = _weaponsStrengtheningInfo[i];
			}
			else
			{
				m_weaponsStrengtheningInfo[i] = WeaponsStrengtheningInfo();
			}
		}
	}

	//強化情報の一覧を返す
	//戻り値 ... 強化情報の一覧
	const std::vector<WeaponsStrengtheningInfo> GetWeaponsStrengtheningInfoList()
	{
		std::vector<WeaponsStrengtheningInfo> list;
		for (int i = 0; i < WeaponsStrengtheningSiz; ++i)
		{
			list.push_back(m_weaponsStrengtheningInfo[i]);
		}

		return list;
	}

	//今のアニメーションの進み具合をセット
	// _percent ... 進み具合(0〜1)
	void SetAnimePercentNow(float _percent) { m_animePercentNow = _percent; }

	//==============================
	// コンボの段の値（PlayerBase::StartComboStep から渡される）
	//==============================

	// 段が始まったときに呼ぶ。ダメージ・ノックバック・ヒットストップ・当たり判定と軌跡のタイミングに使う
	// _step ... 始まった段
	void SetComboStep(const ComboStep& _step)
	{
		// コンボ以外の攻撃のための軌跡の値を、最初の1回だけ覚えておく
		if (!m_hasComboStep)
		{
			m_defaultPolyStart = m_animePercentPolyStart;
			m_defaultPolyEnd = m_animePercentPolyEnd;
		}

		m_hasComboStep = true;
		m_comboStepNow = _step;

		// 軌跡のタイミングを段の値にする
		m_animePercentPolyStart = _step.m_trailStart;
		m_animePercentPolyEnd = _step.m_trailEnd;
		m_tPolyFlg = false;

		// 前の段の進行度が残っていると、一瞬だけ判定が出るので 0 に戻す
		m_animePercentNow = 0.0f;
	}

	// コンボが終わった・キャンセルされたときに呼ぶ（コンボ以外の攻撃は今までどおりに戻る）
	void ClearComboStep()
	{
		if (!m_hasComboStep) { return; }

		m_hasComboStep = false;
		m_animePercentPolyStart = m_defaultPolyStart;
		m_animePercentPolyEnd = m_defaultPolyEnd;
	}

	// 段の倍率（コンボ中でなければ 1 倍）
	float GetComboDamageRate() const { return m_hasComboStep ? m_comboStepNow.m_damageRate : 1.0f; }
	//段のノックバック倍率を返す(コンボ中でなければ1倍)
	//戻り値 ... ノックバック倍率
	float GetComboKnockbackRate() const { return m_hasComboStep ? m_comboStepNow.m_knockbackRate : 1.0f; }

	// 段のヒットストップ（コンボ中でなければ _default）
	float GetComboHitStopTime(float _default) const { return m_hasComboStep ? m_comboStepNow.m_hitStopTime : _default; }

	// 当たったときのヒットストップの長さ（強弱をつける）
	// ・元はコンボの段の時間（データの hitStopTime）
	// ・チャージ攻撃はためた段階ぶん長く
	// ・コンボの最後の段は 1.4 倍
	// ・とどめを刺した一撃は 2 倍
	// _chargeLV : チャージの段階（0 = ためていない）
	// _isKill   : この一撃で敵を倒したか
	float CalcHitStopTime(int _chargeLV, bool _isKill) const
	{
		float time = GetComboHitStopTime(0.03f);

		// チャージ攻撃
		time += _chargeLV * 0.015f;

		// コンボの最後の段（次の段の受付がない段 = acceptStart が 9.9 のまま）
		if (m_hasComboStep && m_comboStepNow.m_acceptStart >= 9.0f)
		{
			time *= 1.4f;
		}

		// とどめの一撃
		if (_isKill)
		{
			time *= 2.0f;
		}

		// 長すぎると操作が重く感じるので上限
		return std::min(time, 0.15f);
	}

	// 今、当たり判定を出してよいか（コンボ中なら hitStart〜hitEnd の間だけ。コンボ以外は今までどおり常に）
	bool IsInComboHitWindow() const
	{
		if (!m_hasComboStep) { return true; }
		return m_animePercentNow >= m_comboStepNow.m_hitStart && m_animePercentNow <= m_comboStepNow.m_hitEnd;
	}


	//コンボの一覧を返す
	//戻り値 ... コンボの一覧
	virtual const std::vector<ComboStep>& GetComboList() { return m_comboList; }

	// コンボのファイル名（"Sword.json" など。フォルダーは持ち主が決める）
	const std::string& GetComboFileName() const { return m_comboFileName; }

	// _path のコンボファイルを読んで m_comboList に入れる。失敗したら false（今のコンボはそのまま）
	bool LoadCombo(const std::string& _path);

	//持ち主のキャラをセット
	// _spParentChara ... 持ち主
	void SetParentChara(std::shared_ptr<CharacterBase>_spParentChara) { m_wpParentChara = _spParentChara; }

protected:

	//持ち主のキャラ
	std::weak_ptr<CharacterBase> m_wpParentChara;

	// 現在レベル lv から lv+1 に上げるのに必要な個数
	// _index ... 強化の種類
	void GetRequiredItemNum(int _index)
	{
		m_weaponsStrengtheningInfo[_index].m_nextLvNum = static_cast<int>(std::ceil(m_weaponsStrengtheningInfo[_index].m_initialNextLvNum * std::pow(m_weaponsStrengtheningInfo[_index].m_growthRate, m_weaponsStrengtheningInfo[_index].m_lv)));
	}

	//1秒当りにたまるCharge時間
	float m_addChargeTime = 1;

	//溜め時間が最大になったか
	bool m_chargeTimeMaxFlg = false;
	//攻撃の角度
	float m_attackAngle = 0;
	//今の溜め時間
	float m_chargeTime = 0;
	//溜め時間の最大
	float m_chargeTimeMax = 0;


	//持ち主の攻撃力
	float m_characterAttackPower = 0;

	//攻撃判定を出すか
	bool m_attackFlg = false;

	//あたり判定をするキャラリスト
	std::list<std::weak_ptr<CharacterBase>>m_attackHitCharacterList;

	//既に当たったキャラリスト
	std::list<std::weak_ptr<CharacterBase>>m_hitCharactersList;

	//あたり判定をするオブジェリスト
	std::list<std::weak_ptr<MapBase>>m_objList;



	//武器のモデル
	std::shared_ptr<KdModelWork> m_spWeaponModel = nullptr;

	//武器のローカル
	Math::Matrix m_localMat = Math::Matrix::Identity;
	//武器のローカル座標
	Math::Vector3 m_localPos = Math::Vector3::Zero;


	//武器の親
	Math::Matrix m_weaponParentMat = Math::Matrix::Identity;
	//親の回転
	Math::Vector3 m_weaponParentRot = Math::Vector3::Zero;

	//武器ステータスが入ってるファイル名
	std::string m_weaponStatusFilePath;

	//コンボのファイル名（"Sword.json" など。各武器の Init で入れる）
	std::string m_comboFileName;
	//武器のステータスを読み込む
	// _filePath ... jsonのパス
	void LoadWeaponStatus(const std::string& _filePath);
	//武器のステータスを保存する
	// _filePath ... jsonのパス
	void SaveWeaponStatus(const std::string& _filePath);

	struct WeaponStatus
	{
		// --- 基本ステータス ---
		float m_attackPower = 0.0f;       // 攻撃力
		float m_attackSpeed = 1.0f;       // 攻撃速度（モーションの速さ）
		float m_critRate = 0.0f;          // クリティカル率（0〜1）
		float m_critDamage = 0.0f;		// クリティカル倍率（1.0 = 100%）

		// --- 武器の性質 ---
		float m_range = 1.0f;              // 当たり判定の長さ
		float m_weight = 1.0f;             // 重さ（硬直に影響）
		float m_knockback = 0.0f;          // ノックバック力
		float m_poiseBreak = 0.0f;         // ガード崩し値
		float m_stunPower = 0.0f;          // スタン値

		// --- モーション補正 ---
		float m_startup = 0.05f;            // 攻撃開始フレーム補正
		float m_recovery = 0.0f;           // 攻撃後硬直補正
		float m_cancelWindow = 0.0f;       // キャンセル可能フレーム補正
	};

	//武器共通基本ステータス
	WeaponStatus m_baseWeaponStatus;


	//座標補正の距離
	float m_aimAssistRadius = 10;




	////////////////////////////////////////////////
	//Charge攻撃パラメーター

	//Charge攻撃を使うのに使う1秒当りのスタミナコスト値
	float m_chargeAttackMaxCost = 10;

	//Charge攻撃が当たっていい敵の数
	int m_hitNum = 1;

	//進んで良い最大の距離
	float m_maxDistanceM = 100;

	//1秒あたりに進む距離
	float m_chargeAttackSpeed = 30;

	//最大ダメージ
	float m_chargeAttackMaxDamage = 100;

	//強化情報(種類ごと)
	WeaponsStrengtheningInfo m_weaponsStrengtheningInfo[WeaponsStrengtheningSiz];

	//防御無視の値
	float m_defenseIgnore = 0;


	//トレイルポリゴン (軌跡) ポリゴン
	std::shared_ptr<KdTrailPolygon> m_spTPoly = nullptr;
	//軌跡を出しているか
	bool m_tPolyFlg = false;

	// コンボの段（SetComboStep で入る）
	bool      m_hasComboStep = false;
	//今の段
	ComboStep m_comboStepNow;
	float     m_defaultPolyStart = 0.0f;   // コンボ以外の攻撃の軌跡のタイミング
	float     m_defaultPolyEnd = 0.0f;



	//今のアニメーションの進み具合
	float m_animePercentNow = 0.0f;
	//軌跡を出し始める進み具合
	float m_animePercentPolyStart;
	//軌跡を出し終わる進み具合
	float m_animePercentPolyEnd;


	//コンボの一覧
	std::vector<ComboStep> m_comboList;


	//溜め斬りの斬撃を出したか
	bool m_finalSlashDoneFlg = false;
};

