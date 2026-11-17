#pragma once
#include"../UIBase.h"

class CameraBase;

// ダメージ数字の種類（色と大きさが変わる）
enum class HitDamageType
{
	Normal,		// 普通に敵に当てた（白）
	Kill,		// とどめの一撃（金色・大きい）
	Critical,	// 会心（オレンジ・少し大きい）
	Player,		// プレイヤーが受けた（赤）
};

struct HitDamageDrawInfo {

	//出す位置(ワールド)
	Math::Vector3 m_3DPos = Math::Vector3::Zero;
	//出す位置(画面)
	Math::Vector2 m_2DPos = Math::Vector2::Zero;
	//ダメージ量
	int m_damage = 0;
	//表示時間
	float m_showTime = 3;
	//アルファ
	float m_alpha = 1;

	// 種類（色と大きさを決める）
	HitDamageType m_type = HitDamageType::Normal;

	// 出てからの経過時間（秒）
	float m_timer = 0.0f;

	// 今の大きさ（倍率。出た瞬間は大きく、すぐ 1 に戻る）
	float m_scale = 1.0f;

	// 浮き上がる量（画面のピクセル）
	float m_floatY = 0.0f;
};

class HitDamage :public UIBase
{
public:
	//コンストラクタ
	HitDamage() {};
	//デストラクタ
	~HitDamage()override {};


	//初期化
	void Init()override;

	//更新前の処理
	void PreUpdate()override;
	//更新
	void Update()override;

	//描画前の処理
	void PreDraw()override;
	//2Dの描画
	void DrawSprite()override;


	//ダメージ数字を出す
	// _damage ... ダメージ量
	// _3DPos ... 出す位置(ワールド)
	// _type ... 数字の種類
	void SetDamage(int _damage, Math::Vector3 _3DPos, HitDamageType _type= HitDamageType::Normal)
	{
		// 当たった位置から少しだけずらす（連続で当てても数字が重ならないように）
		float randX = KdRandom::GetFloat(-0.3f, 0.3f);
		float randY = KdRandom::GetFloat(-0.2f, 0.2f);
		float randZ = KdRandom::GetFloat(-0.3f, 0.3f);
		_3DPos += { randX, randY, randZ };

		HitDamageDrawInfo damageInfo;
		damageInfo.m_3DPos = _3DPos;
		damageInfo.m_damage = _damage;
		damageInfo.m_type = _type;
		m_hitDamageList.push_back(damageInfo);


		if (m_hitDamageList.size() >= m_maxNum)
		{
			m_hitDamageList.erase(m_hitDamageList.begin());
		}
	}

	//カメラをセット
	// _spCamera ... カメラ
	void SetCamera(const std::shared_ptr<CameraBase>& _spCamera) { m_wpCamera = _spCamera; }
private:

	//表示中のダメージ数字の一覧
	std::list<HitDamageDrawInfo>m_hitDamageList;

	//カメラ
	std::weak_ptr<CameraBase>m_wpCamera;


	//---------- 演出の時間 ----------
	const float m_popTime = 0.12f;	// 大きく出て縮むまで
	const float m_holdTime = 0.45f;	// 止まって見せる時間
	const float m_fadeTime = 0.4f;		// 浮きながら消える時間

	//---------- 大きさ ----------
	const float m_baseSize = 2.5f;		// 普通の数字の大きさ（NumDraw の倍率）
	const float m_popScale = 1.8f;		// 出た瞬間の大きさ（倍）
	const float m_floatDist = 30.0f;	// 消えるまでに浮く距離（ピクセル）

	// 一度に出しておける数字の数（多すぎると重く、見づらい）
	const int   m_maxNum = 20;
};
