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

private:

	//武器の長さ
	float m_weaponLength = 1.2f;
	//武器の先端のローカル座標
	Math::Vector3 tipLocalPos = {0,m_weaponLength,0};
	Math::Vector3 baseLocalPos = {0,0,0};

	// 当たり判定用パラメータ
	float m_hitSphereRadius = 0.25f; // 球の半径（判定の厚み）
	// 前フレームのワールド座標
	Math::Vector3 m_prevTipPos = Math::Vector3::Zero;
	Math::Vector3 m_prevBasePos = Math::Vector3::Zero;
	bool m_isFirstFrame = true;
	bool IsAlreadyHit(const std::shared_ptr<CharacterBase>& _chara);


	//トレイルポリゴン (軌跡) ポリゴン
	std::shared_ptr<KdTrailPolygon> m_tPoly = nullptr;
};
