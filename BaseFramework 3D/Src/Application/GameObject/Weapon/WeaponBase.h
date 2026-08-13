#pragma once

class CharacterBase;

class WeaponBase:public KdGameObject
{
public:
	WeaponBase() {};
	~WeaponBase() override{};


	void Init()override;
	void Update()override;
	void DrawLit()override;

	//武器の当たり判定を作る
	void CreateWeaponHitCollider();

	void SetParentMatrix(Math::Matrix _mat) { m_weponParentMat = _mat; }
	void SetParentRotation(Math::Vector3 _rot) { m_weponParentRot = _rot; }

	void SetAttackFlg(bool _flg);

	void AddAttackHitCharacterList(std::shared_ptr<CharacterBase>_character) { m_attackHitCharacterList.push_back(_character); }

	void ClearAttackHitCharacterList() { m_attackHitCharacterList.clear(); }
	
	//attackHitCharacterListの中から一番近い対象の座標を返す
	Math::Vector3 GetCloseAttackHitCharacter();

	void SetCharacterAttackPower(float _power) { m_characterAttackPower = _power; }
protected:

	float m_characterAttackPower = 0;

	//攻撃判定を出すか
	bool m_attackFlg = false;

	//あたり判定をするリスト
	std::list<std::weak_ptr<CharacterBase>>m_attackHitCharacterList;

	//既に当たったキャラリスト
	std::list<std::weak_ptr<CharacterBase>>m_hitCharactersList;



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
		float startup = 0.0f;            // 攻撃開始フレーム補正
		float recovery = 0.0f;           // 攻撃後硬直補正
		float cancelWindow = 0.0f;       // キャンセル可能フレーム補正
	};

	//武器共通基本ステータス
	WeaponStatus m_baseWeaponStatus;

};

