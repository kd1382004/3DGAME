//---- 自分のヘッダー ----
#include "Sword_FinalSlash.h"							//剣の必殺技

//---- カメラ ----
#include"../../../Camera/CameraBase.h"					//カメラの基底クラス

//---- キャラクター ----
#include"../../../Character/CharacterBase.h"			//キャラクターの基底クラス

//---- 便利機能 ----
#include"../../../../Info/DeltaTime/DeltaTime.h"		//デルタタイム

//=============================================
// 
//=============================================

void Sword_FinalSlash::Init()
{
	m_state = State::Charge;
	m_timer = 0.0f;



	//---------- 演出用の画像 ----------
	const std::string texPath = "Asset/Textures/Weapon/Sword/Sword_FinalSlash/";

	// 読み込み済みなら何もしない
	auto loadTex = [](std::shared_ptr<KdTexture>& _spTex, const std::string& _path)
		{
			if (_spTex) { return; }
			_spTex = std::make_shared<KdTexture>();
			_spTex->Load(_path);
		};

	// 斬撃の線・ぼんやりした光・黒いもや
	loadTex(m_spLineTex, texPath + "FinalSlash_Line.png");
	loadTex(m_spGlowTex, texPath + "FinalSlash_Glow.png");
	loadTex(m_spDarkHaloTex, texPath + "FinalSlash_DarkHalo.png");

	// 衝撃波リング
	loadTex(m_spRingTex, texPath + "FinalSlash_Ring.png");
	loadTex(m_spRingGlowTex, texPath + "FinalSlash_RingGlow.png");
	// 黒い稲妻（FinalSlash_Bolt0.png 〜 FinalSlash_Bolt2.png）
	
	for (int i = 0; i < kBoltTexNum; i++)
	{
		loadTex(m_spBoltTex[i], texPath + "FinalSlash_Bolt" + std::to_string(i) + ".png");
	}

	// 板ポリゴン（描くたびに SetMaterial で画像を差し替える）
	if (!m_spPoly)
	{
		m_spPoly = std::make_shared<KdSquarePolygon>();
		m_spPoly->SetMaterial(m_spLineTex);
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
	if (!m_spPoly) { return; }
	if (m_state != State::Charge && m_state != State::Fade) { return; }

	//今の段階の描く値
	const DrawParam param = CalcDrawParam();

	//==================================================
	// 行列を作る（共通）
	//==================================================
	const auto lineMats = MakeCrossMats(param.m_lineWidthRate, param.m_lineLengthRate);
	const auto glowMats = MakeCrossMats(param.m_glowWidthRate, param.m_lineLengthRate);
	const auto haloMats = MakeCrossMats(param.m_haloScale, param.m_haloScale);

	//==================================================
	// 描く（共通）… もや → 光 → 線 → 稲妻
	//==================================================
	auto& shader = KdShaderManager::Instance().m_StandardShader;

	//ブレンドを変えた回数(最後に同じ回数だけ戻す)
	int blendChangeNum = 0;

	//---------- 黒いもや（Alpha）… 描く時だけ ----------
	if (param.m_drawHalo)
	{
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
		blendChangeNum++;
		m_spPoly->SetMaterial(m_spDarkHaloTex);
		for (auto& mat : haloMats) { shader.DrawPolygon(*m_spPoly, mat, param.m_haloColor); }
	}

	//リングの光
	if (param.m_drawRing)
	{
		m_spPoly->SetMaterial(m_spRingGlowTex);
		shader.DrawPolygon(*m_spPoly, param.m_ringMat, param.m_ringGlowColor);
	}

	//---------- 光と線（加算）… いつも描く ----------
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
	blendChangeNum++;

	m_spPoly->SetMaterial(m_spGlowTex);
	for (auto& mat : glowMats) { shader.DrawPolygon(*m_spPoly, mat, param.m_glowColor); }

	m_spPoly->SetMaterial(m_spLineTex);
	for (auto& mat : lineMats) { shader.DrawPolygon(*m_spPoly, mat, param.m_lineColor); }

	//黒い輪
	if (param.m_drawRing)
	{
		m_spPoly->SetMaterial(m_spRingTex);
		shader.DrawPolygon(*m_spPoly, param.m_ringMat, param.m_ringColor);
	}

	//---------- 稲妻（Alpha）… 描く時だけ ----------
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
	blendChangeNum++;
	if (param.m_drawBolt)
	{
		// 何番目の稲妻から使うか（1本なら真ん中、5本なら全部）
		const int startIndex = (kBoltNum - param.m_boltNum) / 2;

		for (int i = 0; i < param.m_boltNum; i++)
		{
			const int n = startIndex + i;

			// 放射状の基本の角度 ＋ ランダムのずれ
			const float deg = m_boltBaseDeg[n] + m_boltRollDegs[n];

			const auto boltMats = MakeCrossMats(param.m_boltWidthRate, param.m_boltLengthRate, deg, m_boltFlipFlgs[n]);

			m_spPoly->SetMaterial(m_spBoltTex[m_boltIndices[n]]);
			for (auto& mat : boltMats) { shader.DrawPolygon(*m_spPoly, mat, param.m_boltColor); }
		}
	}

	//変えたブレンドを元に戻す(変えた回数だけ戻す)
	for (int i = 0; i < blendChangeNum; i++)
	{
		KdShaderManager::Instance().UndoBlendState();
	}
}

void Sword_FinalSlash::DrawBright()
{
	if (!m_spPoly) { return; }
	if (m_state != State::Charge && m_state != State::Fade) { return; }

	const DrawParam param = CalcDrawParam();
	auto& shader = KdShaderManager::Instance().m_StandardShader;

	//ブレンドは BeginBright で加算になっているので、ここでは変えない
	//(変えると EndBright で元に戻らなくなる)

	//光(Glow)
	m_spPoly->SetMaterial(m_spGlowTex);
	for (auto& mat : MakeCrossMats(param.m_glowWidthRate, param.m_lineLengthRate))
	{
		shader.DrawPolygon(*m_spPoly, mat, param.m_glowColor * m_bloomRate);
	}

	//線(Line)
	m_spPoly->SetMaterial(m_spLineTex);
	for (auto& mat : MakeCrossMats(param.m_lineWidthRate, param.m_lineLengthRate))
	{
		shader.DrawPolygon(*m_spPoly, mat, param.m_lineColor * m_bloomRate);
	}

	//衝撃波リングの光
	if (param.m_drawRing)
	{
		m_spPoly->SetMaterial(m_spRingGlowTex);
		shader.DrawPolygon(*m_spPoly, param.m_ringMat, param.m_ringGlowColor * m_bloomRate);
	}


	if (param.m_drawBolt)
	{
		// 何番目の稲妻から使うか（1本なら真ん中、5本なら全部）
		const int startIndex = (kBoltNum - param.m_boltNum) / 2;

		for (int i = 0; i < param.m_boltNum; i++)
		{
			const int n = startIndex + i;

			// 放射状の基本の角度 ＋ ランダムのずれ
			const float deg = m_boltBaseDeg[n] + m_boltRollDegs[n];

			const auto boltMats = MakeCrossMats(param.m_boltWidthRate, param.m_boltLengthRate, deg, m_boltFlipFlgs[n]);

			m_spPoly->SetMaterial(m_spBoltTex[m_boltIndices[n]]);
			for (auto& mat : boltMats) { shader.DrawPolygon(*m_spPoly, mat, param.m_boltColor); }
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

	// 溜めの音
	m_spChargeSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Wepon/Sword/FinalSlash/Charge.wav", false);
	if (m_spChargeSE)
	{
		m_spChargeSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 0.7f);
	}

}

void Sword_FinalSlash::Burst()
{
	// 溜めの音を止めて、弾ける音
	if (m_spChargeSE)
	{
		m_spChargeSE->Stop();
		m_spChargeSE = nullptr;
	}
	std::shared_ptr<KdSoundInstance> spBurstSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Wepon/Sword/FinalSlash/Burst.wav", false);
	if (spBurstSE)
	{
		spBurstSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
	}

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
			m_defenseIgnore,
			results.empty() ? nullptr : &results.front().m_hitPos	// 当たった位置
		);
	}

	if (hitFlg)
	{
		DeltaTime::Instance().HitStop(0.06);
	}

	// 弾けた瞬間に画面を大きく揺らす（当たらなくても揺らす）
	if (std::shared_ptr<CameraBase> spCamera = m_wpCamera.lock())
	{
		spCamera->Shake(0.6f, 1);
	}

}

//=============================================
// 1本だけのときの値（配列に置き換えたら消す）
//=============================================

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

//=============================================
// その他の処理
//=============================================

Sword_FinalSlash::DrawParam Sword_FinalSlash::CalcDrawParam() const
{
	DrawParam param;

	if (m_state == State::Charge)
	{
		//---------- 溜め（m_chargeRate：0〜1） ----------
		// 線の太さ：0.5 → 0.8 倍
		param.m_lineWidthRate = 0.5f + 0.3f * m_chargeRate;
		param.m_lineLengthRate = 1.0f;

		// 色（RGB は暗い赤、A は透明度）
		param.m_lineColor = m_chargeColor;
		param.m_lineColor.A(0.3f + 0.5f * m_chargeRate);			// 0.3 → 0.8

		param.m_glowWidthRate = param.m_lineWidthRate * 3.5f;
		param.m_glowColor = m_chargeColor;
		param.m_glowColor.A(0.5f * m_chargeRate);					// 0 → 0.5

		// 稲妻はチラ見せ中だけ、薄く
		param.m_drawBolt = m_boltPeekFlg;
		param.m_boltColor = { 1.0f, 1.0f, 1.0f, 0.4f };

		// 溜めではもやは出さない
		param.m_drawHalo = false;

		param.m_boltNum = 1;
		param.m_boltWidthRate = 3.0f;
		param.m_boltLengthRate = 1.0f;
	}
	else
	{
		//---------- 消える（m_fadeRate：0〜1） ----------
		// 弾けた瞬間か（Fade に入ってすぐの短い間だけ、一番派手にする）
		const bool isBurstFlash = (m_timer < m_burstFlashTime);

		// 残り具合（1 → 0）
		const float remain = 1.0f - m_fadeRate;

		// 線の太さ：弾けた瞬間は m_burstWidthRate、そのあと 1.6 → 1.2 倍に細く
		param.m_lineWidthRate = isBurstFlash ? m_burstWidthRate : std::lerp(m_burstWidthRate, 1.2f, m_fadeRate);

		// 線の長さ：1.0 → m_fadeLengthRate 倍に伸びる
		param.m_lineLengthRate = std::lerp(1.0f, m_fadeLengthRate, m_fadeRate);

		// 線の色：明るい赤。弾けた瞬間は真っ白に近いくらいはっきり、そのあと薄くなる
		param.m_lineColor = m_burstColor;
		param.m_lineColor.A(isBurstFlash ? 1.0f : remain);

		// 光：線の 4.5 倍の太さ。線より早く消える（2乗）
		param.m_glowWidthRate = param.m_lineWidthRate * 4.5f;
		param.m_glowColor = m_burstColor;
		param.m_glowColor.A(isBurstFlash ? 1.0f : remain * remain);

		// 稲妻：ずっと描く。線と同じだけ薄くなる
		param.m_drawBolt = true;
		param.m_boltColor = { 1.0f, 1.0f, 1.0f, isBurstFlash ? 1.0f : remain };

		// 黒いもや：弾けた瞬間から少し広がりながら引いていく
		param.m_drawHalo = true;
		param.m_haloScale = m_darkHaloScale * std::lerp(1.0f, 1.3f, m_fadeRate);
		param.m_haloColor = { 1.0f, 1.0f, 1.0f, 0.8f * (isBurstFlash ? 1.0f : remain) };

		param.m_boltNum = 5;
		param.m_boltWidthRate = m_burstBoltWidthRate;
		if (isBurstFlash)
		{
			param.m_boltLengthRate = m_burstBoltLengthRate;
		}
		else
		{
			param.m_boltLengthRate = m_burstBoltLengthRate * std::lerp(1.0f, m_fadeLengthRate, m_fadeRate);
		}

		// 衝撃波リング
		float ringRate = m_timer / m_ringTime;
		ringRate = std::clamp(ringRate, 0.0f, 1.0f);
		if (ringRate < 1)
		{
			param.m_drawRing = true;

			const float inv = 1.0f - ringRate;
			const float ringEase = 1.0f - inv * inv * inv;

			param.m_ringSize = std::lerp(m_ringStartSize, m_ringEndSize, ringEase);

			float ringAlpha = 1 - ringRate;
			param.m_ringGlowColor = m_ringGlowColor;
			param.m_ringGlowColor.A(ringAlpha);

			param.m_ringColor = { 1, 1, 1, ringAlpha };
		}
	}

	// 衝撃波リングの行列（地面と平行に寝かせて、箱の真ん中に置く）
	param.m_ringMat = Math::Matrix::CreateScale(param.m_ringSize, param.m_ringSize, 1.0f)
		* Math::Matrix::CreateRotationX(DirectX::XM_PIDIV2)
		* Math::Matrix::CreateTranslation(m_obb.Center);

	return param;
}

std::array<Math::Matrix, 2> Sword_FinalSlash::MakeCrossMats(float _widthRate, float _lengthRate, float _rollDeg, bool _flip) const
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
}
