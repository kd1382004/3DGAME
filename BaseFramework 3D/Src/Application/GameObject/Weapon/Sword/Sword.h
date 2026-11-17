#pragma once
#include"../WeaponBase.h"

class Sword :public WeaponBase
{
public:
	//コンストラクタ
	Sword() {};
	//デストラクタ
	~Sword()override {};

	//初期化
	void Init()override;
	//更新
	void Update()override;
	//ライトありの描画
	void DrawLit()override;

	//溜め攻撃を出す
	void ChargeAttackPlay();

	//溜めている時間をセット(溜め段階のエフェクトも切り替える)
	// _time ... 溜めている時間
	void SetNowChargeTime(float _time)override;
	
	//攻撃判定を出すかをセット
	// _flg ... 出すか
	void SetAttackFlg(bool _flg)override;

	//武器の音を止める
	void StopWeaponSE()override;

	//今の強化レベルのコンボを返す
	//戻り値 ... コンボの一覧
	const std::vector<ComboStep>& GetComboList()override;
private:

	// 武器の長さ
	float m_weaponLength = 1.2f;
	//切っ先の位置(ローカル)
	Math::Vector3 m_tipLocalPos = { 0, m_weaponLength, 0 };
	//根元の位置(ローカル)
	Math::Vector3 m_baseLocalPos = { 0, 0, 0 };
	// --- 当たり判定用パラメータ (BOX判定用) ---
	// BOXのハーフサイズ (X:幅/2, Y:長さ/2, Z:厚み/2)
	Math::Vector3 m_hitBoxExtents = { 0.25f, 0.6f, 0.25f };
	// BOXのローカルオフセット (基部から刃の中心までの距離)
	Math::Vector3 m_hitBoxLocalOffset = { 0.0f, 0.6f, 0.0f };
	// 前フレームのワールド座標および行列
	Math::Vector3 m_prevTipPos = Math::Vector3::Zero;
	//前フレームの根元の位置(ワールド)
	Math::Vector3 m_prevBasePos = Math::Vector3::Zero;
	Math::Matrix  m_prevWeaponParentMat = Math::Matrix::Identity; // 追加：前フレーム行列
	bool m_isFirstFrame = true;
	//このフレームですでに当たったキャラか
	// _spChara ... 調べるキャラ
	//戻り値 ... 当たっていたらtrue
	bool IsAlreadyHit(const std::shared_ptr<CharacterBase>& _spChara);


	//溜め段階0のエフェクト
	std::weak_ptr<KdEffekseerObject> m_wpChargeLV0Effect;
	//溜め段階1のエフェクト
	std::weak_ptr<KdEffekseerObject> m_wpChargeLV1Effect;
	//溜め段階2のエフェクト
	std::weak_ptr<KdEffekseerObject> m_wpChargeLV2Effect;

	//今の溜め段階
	int m_chargeLV = 0;


	//音
	std::shared_ptr<KdSoundInstance> m_spDaggerSE;
	//溜め攻撃の音
	std::shared_ptr<KdSoundInstance> m_spChargeSE;
	//当たったときの音
	std::shared_ptr<KdSoundInstance> m_spHitSE;


	//今の強化レベルのコンボ
	std::vector<ComboStep> m_nowLvComboList;

	//溜め斬りの斬撃を出す
	// _lv ... 溜め段階
	void SpawnFinalSlash(int _lv);
};
