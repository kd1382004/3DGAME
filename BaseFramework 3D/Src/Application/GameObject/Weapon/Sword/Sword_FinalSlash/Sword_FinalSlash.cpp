#include "Sword_FinalSlash.h"
#include"../../../../Info/DeltaTime/DeltaTime.h"
#include"../../../Character/CharacterBase.h"
void Sword_FinalSlash::Init()
{
	m_state = State::Charge;
	m_timer = 0.0f;



	//---------- 演出用の画像 ----------
	const std::string texPath = "Asset/Textures/Weapon/Sword/Sword_FinalSlash/";

	// 読み込み済みなら何もしない
	auto loadTex = [](std::shared_ptr<KdTexture>& _tex, const std::string& _path)
		{
			if (_tex) { return; }
			_tex = std::make_shared<KdTexture>();
			_tex->Load(_path);
		};

	// 斬撃の線・ぼんやりした光・黒いもや
	loadTex(m_lineTex, texPath + "FinalSlash_Line.png");
	loadTex(m_glowTex, texPath + "FinalSlash_Glow.png");
	loadTex(m_darkHaloTex, texPath + "FinalSlash_DarkHalo.png");

	// 衝撃波リング
	loadTex(m_ringTex, texPath + "FinalSlash_Ring.png");
	loadTex(m_ringGlowTex, texPath + "FinalSlash_RingGlow.png");
	// 黒い稲妻（FinalSlash_Bolt0.png 〜 FinalSlash_Bolt2.png）
	for (int i = 0; i < kBoltTexNum; i++)
	{
		loadTex(m_boltTex[i], texPath + "FinalSlash_Bolt" + std::to_string(i) + ".png");
	}

	// 板ポリゴン（描くたびに SetMaterial で画像を差し替える）
	if (!m_Poly)
	{
		m_Poly = std::make_shared<KdSquarePolygon>();
		m_Poly->SetMaterial(m_lineTex);
	}
}

void Sword_FinalSlash::Update()
{
	float dt = DeltaTime::Instance().GetGameDeltaTime();

	m_timer += dt;


	switch (m_state)
	{
	case Sword_FinalSlash::State::Charge:

		// 溜めの進み具合（0〜1）… 段階を変える前に計算する
		m_chargeRate = std::clamp(m_timer / m_chargeTime, 0.0f, 1.0f);

		// 稲妻の切り替え
		m_boltFlickerTimer += dt;
		if (m_boltFlickerTimer >= m_boltFlickerInterval)
		{
			m_boltFlickerTimer = 0.0f;

			RandomizeBolts();

			// チラ見せするか決める
			// 溜めの前半は見せない。後半は 3回に1回くらい一瞬だけ見せる
			if (m_chargeRate < m_boltPeekStartRate)
			{
				m_boltPeekFlg = false;
			}
			else
			{
				m_boltPeekFlg = (KdRandom::GetInt(0, 2) == 0);
			}
		}

		// 溜め終わったら弾ける
		if (m_timer >= m_chargeTime)
		{
			m_state = State::Burst;
			m_timer = 0.0f;
			m_boltPeekFlg = true;
		}
		break;
	case Sword_FinalSlash::State::Burst:
		Burst();
		m_state = State::Fade;	// すぐ次へ
		m_timer = 0;
		m_boltFlickerInterval = m_boltFlickerIntervalStart;
		break;
	case Sword_FinalSlash::State::Fade:
		// 消える進み具合（0〜1）
		m_fadeRate = std::clamp(m_timer / m_fadeTime, 0.0f, 1.0f);

		// 切り替えの間隔：最初は速く、最後はゆっくり（2乗で後半に一気に遅く）
		m_boltFlickerInterval = std::lerp(m_boltFlickerIntervalStart, m_boltFlickerIntervalEnd, m_fadeRate * m_fadeRate);

		// 稲妻の切り替え（Fade ではずっと表示するので、チラ見せの判定はしない）
		m_boltFlickerTimer += dt;
		if (m_boltFlickerTimer >= m_boltFlickerInterval)
		{
			m_boltFlickerTimer = 0.0f;

			RandomizeBolts();
		}

		// 消え終わったら自分を消す
		if (m_timer >= m_fadeTime)
		{
			m_isExpired = true;
		}
		break;
	default:
		break;
	}
}

void Sword_FinalSlash::DrawEffect()
{
	if (!m_Poly) { return; }
	if (m_state != State::Charge && m_state != State::Fade) { return; }
	//---------- 板を置く行列を2枚分（十字）作る ----------
	// _widthRate : 横幅の倍率（箱の横幅 × これ）
	// _lengthRate: 長さの倍率（箱の長さ × これ）
	// _rollDeg   : 板を傾ける角度（稲妻用。線と光は 0）
	// _flip      : 左右反転するか（稲妻用。線と光は false）
	// 戻り値     : [0] = 1枚目、[1] = 2枚目（1枚目を Y 軸で 90 度回したもの）
	auto makeCrossMats = [&](float _widthRate, float _lengthRate = 1.0f,
		float _rollDeg = 0.0f, bool _flip = false)
		{
			// 大きさ（箱の横幅・長さに合わせる。反転なら横幅をマイナス）
			const float w = m_obb.Extents.x * 2.0f * _widthRate * (_flip ? -1.0f : 1.0f);
			const float h = m_obb.Extents.y * 2.0f * _lengthRate;
			const Math::Matrix scale = Math::Matrix::CreateScale(w, h, 1.0f);

			// 板そのものを傾ける（板の面の中で回す）
			const Math::Matrix roll = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(_rollDeg));

			// 箱の向きと位置
			const Math::Matrix rot = Math::Matrix::CreateFromQuaternion(m_obb.Orientation);
			const Math::Matrix trans = Math::Matrix::CreateTranslation(m_obb.Center);

			// 2枚目用：Y 軸で 90 度回して十字にする
			const Math::Matrix cross = Math::Matrix::CreateRotationY(DirectX::XM_PIDIV2);

			std::array<Math::Matrix, 2> mats;
			mats[0] = scale * roll * rot * trans;
			mats[1] = scale * roll * cross * rot * trans;
			return mats;
		};

	//---------- 描くのに使う値（段階ごとに下で入れる） ----------
	const Math::Color clearColor = { 1.0f, 1.0f, 1.0f, 0.0f };	// 透明（初期値用）

	// 線の横幅の倍率（箱の横幅 × これ）
	float lineWidthRate = 1.0f;

	// 線の長さの倍率（箱の長さ × これ。光と稲妻も同じ長さにする）
	float lineLengthRate = 1.0f;

	// 線の色と透明度
	Math::Color lineColor = clearColor;

	// 光（Glow）の横幅の倍率
	float glowWidthRate = 1.0f;

	// 光の色と透明度
	Math::Color glowColor = clearColor;

	// 稲妻を描くか
	bool drawBolt = false;

	// 稲妻の色と透明度
	Math::Color boltColor = clearColor;

	// 黒いもやを描くか
	bool drawHalo = false;

	// 黒いもやの大きさの倍率（縦・横とも）
	float haloScale = 1.0f;

	// 黒いもやの色と透明度
	Math::Color haloColor = clearColor;

	//描く稲妻の本数
	int boltNum = 0;

	//稲妻の横幅の倍率
	float boltWidthRate = 3.0f;

	//稲妻の長さの倍率
	float boltLengthRate = 1.0f;

	// 衝撃波リングを描くか（Fade の最初の m_ringTime 秒だけ）
	bool drawRing = false;

	// 衝撃波リングの大きさ（m。直径）
	float ringSize = 0.0f;

	// 衝撃波リングの光の色と透明度（加算）
	Math::Color ringGlowColor = clearColor;

	// 衝撃波リングの黒い輪の色と透明度（Alpha）
	Math::Color ringColor = clearColor;

	//==================================================
	//  段階ごとに値を入れる
	//==================================================
	if (m_state == State::Charge)
	{
		//---------- 溜め（m_chargeRate：0〜1） ----------
		// 線の太さ：0.5 → 0.8 倍
		lineWidthRate = 0.5f + 0.3f * m_chargeRate;
		lineLengthRate = 1.0f;

		// 色（RGB は暗い赤、A は透明度）
		lineColor = m_chargeColor;
		lineColor.A(0.3f + 0.5f * m_chargeRate);			// 0.3 → 0.8

		glowWidthRate = lineWidthRate * 3.5f;
		glowColor = m_chargeColor;
		glowColor.A(0.5f * m_chargeRate);					// 0 → 0.5

		// 稲妻はチラ見せ中だけ、薄く
		drawBolt = m_boltPeekFlg;
		boltColor = { 1.0f, 1.0f, 1.0f, 0.4f };

		// 溜めではもやは出さない
		drawHalo = false;

		boltNum = 1;
		boltWidthRate = 3.0;
		boltLengthRate = 1.0;
	}
	else
	{
		//---------- 消える（m_fadeRate：0〜1） ----------
		// 弾けた瞬間か（Fade に入ってすぐの短い間だけ、一番派手にする）
		const bool isBurstFlash = (m_timer < m_burstFlashTime);

		// 残り具合（1 → 0）
		const float remain = 1.0f - m_fadeRate;

		// 線の太さ：弾けた瞬間は m_burstWidthRate、そのあと 1.6 → 1.2 倍に細く
		lineWidthRate = isBurstFlash ? m_burstWidthRate : std::lerp(m_burstWidthRate, 1.2f, m_fadeRate);

		// 線の長さ：1.0 → m_fadeLengthRate 倍に伸びる
		lineLengthRate = std::lerp(1.0f, m_fadeLengthRate, m_fadeRate);

		// 線の色：明るい赤。弾けた瞬間は真っ白に近いくらいはっきり、そのあと薄くなる
		lineColor = m_burstColor;
		lineColor.A(isBurstFlash ? 1.0f : remain);

		// 光：線の 4.5 倍の太さ。線より早く消える（2乗）
		glowWidthRate = lineWidthRate * 4.5f;
		glowColor = m_burstColor;
		glowColor.A(isBurstFlash ? 1.0f : remain * remain);

		// 稲妻：ずっと描く。線と同じだけ薄くなる
		drawBolt = true;
		boltColor = { 1.0f, 1.0f, 1.0f, isBurstFlash ? 1.0f : remain };

		// 黒いもや：弾けた瞬間から少し広がりながら引いていく
		drawHalo = true;
		haloScale = m_darkHaloScale * std::lerp(1.0f, 1.3f, m_fadeRate);
		haloColor = { 1.0f, 1.0f, 1.0f, 0.8f * (isBurstFlash ? 1.0f : remain) };


		boltNum = 5;
		boltWidthRate = m_burstBoltWidthRate;
		if (isBurstFlash)
		{
			boltLengthRate = m_burstBoltLengthRate;
		}
		else
		{
			boltLengthRate = m_burstBoltLengthRate * std::lerp(1.0, m_fadeLengthRate, m_fadeRate);
		}



		float ringRate = m_timer / m_ringTime;
		ringRate = std::clamp(ringRate, 0.0f, 1.0f);
		if (ringRate < 1)
		{
			drawRing = true;

			const float inv = 1.0f - ringRate;
			const float ringEase = 1.0f - inv * inv * inv;

			ringSize = std::lerp(m_ringStartSize, m_ringEndSize, ringEase);

			float ringAlpha = 1 - ringRate;
			ringGlowColor = m_ringGlowColor;
			ringGlowColor.A(ringAlpha);

			ringColor = { 1,1,1,ringAlpha };
		}
	}

	//==================================================
	// 行列を作る（共通）
	//==================================================
	const auto lineMats = makeCrossMats(lineWidthRate, lineLengthRate);
	const auto glowMats = makeCrossMats(glowWidthRate, lineLengthRate);
	const auto haloMats = makeCrossMats(haloScale, haloScale);
	const Math::Matrix ringMat = Math::Matrix::CreateScale(ringSize, ringSize, 1.0f) * Math::Matrix::CreateRotationX(DirectX::XM_PIDIV2) * Math::Matrix::CreateTranslation(m_obb.Center);
	//==================================================
	// 描く（共通）… もや → 光 → 線 → 稲妻
	//==================================================
	auto& shader = KdShaderManager::Instance().m_StandardShader;

	//---------- 黒いもや（Alpha）… 描く時だけ ----------
	if (drawHalo)
	{
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
		m_Poly->SetMaterial(m_darkHaloTex);
		for (auto& mat : haloMats) { shader.DrawPolygon(*m_Poly, mat, haloColor); }
	}

	//リングの光
	if (drawRing)
	{
		m_Poly->SetMaterial(m_ringGlowTex);
		shader.DrawPolygon(*m_Poly, ringMat, ringGlowColor);
	}

	//---------- 光と線（加算）… いつも描く ----------
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);

	m_Poly->SetMaterial(m_glowTex);
	for (auto& mat : glowMats) { shader.DrawPolygon(*m_Poly, mat, glowColor); }

	m_Poly->SetMaterial(m_lineTex);
	for (auto& mat : lineMats) { shader.DrawPolygon(*m_Poly, mat, lineColor); }

	//黒い輪 
	if (drawRing)
	{
		m_Poly->SetMaterial(m_ringTex);
		shader.DrawPolygon(*m_Poly, ringMat, ringColor);
	}


	//---------- 稲妻（Alpha）… 描く時だけ ----------
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
	if (drawBolt)
	{
		// 何番目の稲妻から使うか（1本なら真ん中、5本なら全部）
		const int startIndex = (kBoltNum - boltNum) / 2;

		for (int i = 0; i < boltNum; i++)
		{
			const int n = startIndex + i;

			// 放射状の基本の角度 ＋ ランダムのずれ
			const float deg = m_boltBaseDeg[n] + m_boltRollDegs[n];

			const auto boltMats = makeCrossMats(boltWidthRate, boltLengthRate, deg, m_boltFlipFlgs[n]);

			m_Poly->SetMaterial(m_boltTex[m_boltIndices[n]]);
			for (auto& mat : boltMats) { shader.DrawPolygon(*m_Poly, mat, boltColor); }
		}
	}
}

void Sword_FinalSlash::SetSlashStatus(const DirectX::BoundingOrientedBox& _obb, float _damage, float _knockback, const Math::Vector3& _dir, float _defenseIgnore, float _chargeTime)
{
	m_obb = _obb;					// 斬撃の跡の判定（ワールド）
	m_damage = _damage;				// 弾けたときのダメージ
	m_knockback = _knockback;		// ノックバックの強さ
	m_dir = _dir;					// ノックバックの向き
	m_defenseIgnore = _defenseIgnore;	// 防御無視の割合
	m_chargeTime = _chargeTime;		// 弾けるまでの溜めの時間

	// 出した瞬間から溜めを始める
	m_state = State::Charge;
	m_timer = 0.0f;
	RandomizeBolts();

}

void Sword_FinalSlash::Burst()
{
	// 斬撃の跡の箱
	KdCollider::BoxInfo box(KdCollider::TypeDamage, m_obb);


	bool hitFlg = false;
	for (auto& wpChara : m_attackHitCharacterList)
	{
		// もう消えた敵は飛ばす
		std::shared_ptr<CharacterBase> spChara = wpChara.lock();
		if (!spChara) { continue; }

		// もう死んでいる敵は飛ばす
		if (spChara->GetIsDead()) { continue; }

		// 箱と当たっているか
		std::list<KdCollider::CollisionResult> results;
		if (!spChara->Intersects(box, &results)) { continue; }
		hitFlg = true;
		// 当たったときの処理
		spChara->OnAttackHit(
			m_damage,
			m_knockback,   // コンボの段のノックバック倍率
			m_dir,
			0,
			false,
			m_defenseIgnore
		);
	}

	if (hitFlg)
	{
		DeltaTime::Instance().HitStop(0.06);
	}

}

void Sword_FinalSlash::RandomizeBolts()
{
	for (int i = 0; i < kBoltNum; i++)
	{
		// 今と違う絵の番号にする
		m_boltIndices[i] = (m_boltIndices[i] + KdRandom::GetInt(1, kBoltTexNum - 1)) % kBoltTexNum;

		// 基本の角度からのずれ（基本の角度は描くときに足す）
		m_boltRollDegs[i] = KdRandom::GetFloat(-10.0f, 10.0f);

		// 半分の確率で左右反転
		m_boltFlipFlgs[i] = (KdRandom::GetInt(0, 1) == 1);
	}
}
