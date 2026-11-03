#pragma once

class  CharacterBase;
class  MapBase;


struct StatusShockwave
{
	// 何体までヒットしていいか
	int maxHitNum = 0;

	// 最大飛距離（メートル）
	float maxDistanceM = 0;

	//進んだ距離
	float DistanceM = 0;

	// 飛ぶ速度（m/s）
	float speed = 0;

	// 最大ダメージ
	float maxDamage = 0;

	// 現在の飛行距離
	float nowDistance = 0.0f;

};

class  Dagger_ChargeAttack :public KdGameObject
{
public:
	Dagger_ChargeAttack() {};
	~Dagger_ChargeAttack()override {};

	void Init()override;
	void Update()override;


	void DrawEffect()override;


	//Statusセット
	//_hitNum...何体まで当たっていいか
	//_maxDistanceM ...　どれだけの距離(メートル)飛んでいいか
	//_speed ... 速度
	// _maxDamage ..最大ダメージ
	void SetShockwaveStatus(int _hitNum, float _maxDistanceM, float _speed, float _maxDamage, Math::Vector3 _pos, float _Angle)
	{
		m_shockwaveStatus.maxHitNum = _hitNum;
		m_shockwaveStatus.maxDistanceM = _maxDistanceM;
		m_shockwaveStatus.speed = _speed;
		m_shockwaveStatus.maxDamage = _maxDamage;
		m_pos = _pos;
		m_angle = _Angle;

		m_startPos = m_pos;
		m_prevTipPos = m_pos;

		Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);
		Math::Matrix r1Mat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(90));
		Math::Matrix r2Mat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
		m_mWorld = r1Mat * r2Mat * tMat;
		m_prevWorldMat = m_mWorld;

		m_isFirstFrame = true;
	}

	// 攻撃対象キャラクターリストを親から受け取る関数
	void SetAttackHitCharacterList(const std::list<std::weak_ptr<CharacterBase>>& _list)
	{
		m_attackHitCharacterList = _list;
	}

	// ポリゴンスケール（サイズ）の設定 (X: 横幅, Y: 進行方向の長さ)
	void SetPolyScale(const Math::Vector2& _scale)
	{
		m_polyScale = _scale;
		if (m_Poly)
		{
			m_Poly->SetScale(m_polyScale);
		}
	}

	void SetMapObjList(std::list<std::weak_ptr<MapBase>> _list)
	{ 
		m_MapObjList = _list;
	}

private:

	//あたり判定をするリスト
	std::list<std::weak_ptr<CharacterBase>>m_attackHitCharacterList;

	std::list<std::weak_ptr<MapBase>>m_MapObjList;

	//既に当たったキャラリスト
	std::list<std::weak_ptr<CharacterBase>>m_hitCharactersList;

	StatusShockwave m_shockwaveStatus;


	// ポリゴン
	std::shared_ptr<KdSquarePolygon> m_Poly = nullptr;

	// ポリゴンスケール変数 (X: 幅, Y: 長さ)
	Math::Vector2 m_polyScale = { 2.0f, 1.0f };
	// 上下の厚み（全高）
	float m_polyThickness = 0.6f;


	// 座標
	Math::Vector3 m_pos = Math::Vector3::Zero;
	Math::Vector3 m_startPos = Math::Vector3::Zero; // 発射地点座標

	//進む角度
	float m_angle = 0;


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

	// 前フレームの位置およびワールド行列
	Math::Matrix  m_prevWorldMat = Math::Matrix::Identity;


	//残像用
	struct GhostTrail
	{
		Math::Vector3 pos;
		float alpha;      // 透明度
		float lifeTime;   // 残像の寿命
	};

	std::vector<GhostTrail> m_trails;
	float m_trailSpawnInterval = 0.002f; //ごとに残像生成
	float m_trailTimer = 0.0f;

	void GhostTrailUpdate();
};
