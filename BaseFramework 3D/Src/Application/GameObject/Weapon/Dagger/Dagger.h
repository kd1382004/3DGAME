#pragma once
#include"../WeaponBase.h"

class Dagger :public WeaponBase
{
public:
	Dagger() {};
	~Dagger()override {};

	void Init()override;
	void Update()override;
	void DrawLit()override;

	void ChargAttackPlay();


	
private:

	// 武器の長さ
	float m_weaponLength = 1.2f;
	Math::Vector3 tipLocalPos = { 0, m_weaponLength, 0 };
	Math::Vector3 baseLocalPos = { 0, 0, 0 };
	// --- 当たり判定用パラメータ (BOX判定用) ---
	// BOXのハーフサイズ (X:幅/2, Y:長さ/2, Z:厚み/2)
	Math::Vector3 m_hitBoxExtents = { 0.25f, 0.6f, 0.25f };
	// BOXのローカルオフセット (基部から刃の中心までの距離)
	Math::Vector3 m_hitBoxLocalOffset = { 0.0f, 0.6f, 0.0f };
	// 前フレームのワールド座標および行列
	Math::Vector3 m_prevTipPos = Math::Vector3::Zero;
	Math::Vector3 m_prevBasePos = Math::Vector3::Zero;
	Math::Matrix  m_prevWeponParentMat = Math::Matrix::Identity; // 追加：前フレーム行列
	bool m_isFirstFrame = true;
	bool IsAlreadyHit(const std::shared_ptr<CharacterBase>& _chara);


	//トレイルポリゴン (軌跡) ポリゴン
	std::shared_ptr<KdTrailPolygon> m_tPoly = nullptr;



};
