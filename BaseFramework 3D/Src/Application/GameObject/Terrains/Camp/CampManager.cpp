//---- 自分のヘッダー ----
#include "CampManager.h"											//キャンプの管理

//---- カメラ ----
#include"../../Camera/PrepareCamera/PrepareCamera.h"				//準備シーン用カメラ

//---- キャンプ ----
#include"CampLight/CampLight.h"										//キャンプのライト
#include"CampObject/CampObject.h"									//キャンプの置物

//---- 便利機能 ----
#include"../../../Info/DeltaTime/DeltaTime.h"						//デルタタイム
#include"../../../Info/FadeDraw/FadeDraw.h"							//画面フェードの描画
#include"../../../Info/SE/SEPlayer.h"								//効果音

namespace
{
	//武器立ての置物の名前の先頭と、武器の種類の対応
	//武器を増やしたら、ここに1行足す
	struct RackWeaponName
	{
		const char*      m_prefix;	//置物の名前の先頭("RackSword.001" なら "RackSword")
		PlayerWeaponInfo m_weapon;	//武器の種類
	};

	const RackWeaponName kRackWeaponNames[] =
	{
		{ "RackDagger", PlayerWeaponInfo_Dagger },
		{ "RackSword",  PlayerWeaponInfo_Sword  },
	};

	//焚き火の炎のエフェクトと音
	const char* kCampfireEffectPath = "Campfire/Campfire.efkefc";
	const char* kCampfireSEPath     = "Asset/Sound/SE/Prepare/Campfire.wav";

	//商人の待機アニメーションの名前(モデルの中の名前と合わせる)
	const char* kMerchantIdleAnime = "Idle";

	//商人の反応ごとのアニメーションの名前(モデルの中の名前と合わせる)
	// _reaction ... 何が起きたか
	//戻り値     ... アニメーションの名前
	const char* MerchantReactionAnime(MerchantReaction _reaction)
	{
		switch (_reaction)
		{
		case MerchantReaction::Buy:     return "Buy";
		case MerchantReaction::Sell:    return "Sell";
		case MerchantReaction::NoMoney: return "NoMoney";
		}
		return kMerchantIdleAnime;
	}
}

//=============================================
// コンストラクタ・デストラクタ
//=============================================

CampManager::~CampManager()
{
	for (auto& fire : m_campfires) { StopCampfire(fire); }

	if (auto spEf = m_wpEntranceEffect.lock())
	{
		spEf->SetLoop(false);
		spEf->StopEffect();
	}
}

//=============================================
// 基本の処理
//=============================================

void CampManager::Init()
{
	// 背景の配置を読む
	m_layout = LoadCampLayout(m_layoutFilePath);
	ApplyCampLayoutTune(m_layout, m_tuneFilePath);

	// 霧と環境光を準備画面用にする
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(true, false);
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_campFogColor, m_campFogDensity);
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_campAmbient);

	// カメラ(タブごとに動く)
	m_spCamera = std::make_shared<PrepareCamera>();
	m_spCamera->Init();
	m_spCamera->SetCameraData(m_layout.m_cameras);
	m_spCamera->MoveTo("Explore", true);	// 最初から入口を映す
	m_objList.push_back(m_spCamera);
	KdEffekseerManager::GetInstance().SetCamera(m_spCamera->GetCamera());

	// 置物
	for (const auto& data : m_layout.m_objects)
	{
		// 松明は自分で光るので出さない(保存のときのために残しておく)
		if (data.m_kind == "Torch") { m_hiddenObjects.push_back(data); continue; }

		AddCampObject(data);
	}

	// ライト
	for (const auto& data : m_layout.m_lights)
	{
		// 松明は自分で光るので出さない(保存のときのために残しておく)
		if (data.m_name.rfind("Light_Torch", 0) == 0) { m_hiddenLights.push_back(data); continue; }

		AddCampLight(data);
	}

	// 入口の奥のエフェクト(ダンジョンの階段と同じもの)。ループ再生
	// 位置と大きさはエディタの「出発」タブで合わせて CampLayoutTune.json に保存する
	m_wpEntranceEffect = KdEffekseerManager::GetInstance().Play("Stairs/Stairs.efkefc",
		m_layout.m_depart.m_effectPos, m_layout.m_depart.m_effectScale, 1.0f, true);
}

void CampManager::PreUpdate()
{
	// 消えたオブジェクト(エディタで消した置物・ライト)を一覧から外す
	m_objList.remove_if([](const std::shared_ptr<KdGameObject>& _spObj) { return !_spObj || _spObj->IsExpired(); });

	for (const auto& spObj : m_objList) { spObj->PreUpdate(); }
}

void CampManager::Update()
{
	for (const auto& spObj : m_objList) { spObj->Update(); }
}

void CampManager::PostUpdate()
{
	for (const auto& spObj : m_objList) { spObj->PostUpdate(); }
}

void CampManager::PreDraw()
{
	for (const auto& spObj : m_objList) { spObj->PreDraw(); }
}

void CampManager::GenerateDepthMapFromLight()
{
	for (const auto& spObj : m_objList) { spObj->GenerateDepthMapFromLight(); }
}

void CampManager::DrawUnLit()
{
	for (const auto& spObj : m_objList) { spObj->DrawUnLit(); }
}

void CampManager::DrawLit()
{
	for (const auto& spObj : m_objList) { spObj->DrawLit(); }
}

void CampManager::DrawEffect()
{
	for (const auto& spObj : m_objList) { spObj->DrawEffect(); }
}

void CampManager::DrawBright()
{
	for (const auto& spObj : m_objList) { spObj->DrawBright(); }
}

void CampManager::DrawSprite()
{
	for (const auto& spObj : m_objList) { spObj->DrawSprite(); }
}

void CampManager::DrawDebug()
{
	for (const auto& spObj : m_objList) { spObj->DrawDebug(); }
}

//=============================================
// 出発の演出
//=============================================

void CampManager::StartDepart()
{
	if (m_departState != DepartState::None) { return; }

	m_departState     = DepartState::Walk;
	m_departTimer     = 0.0f;
	m_departFadeTimer = 0.0f;
	m_departStepTimer = 0.0f;

	auto spPlayer = m_wpCampPlayer.lock();
	if (!spPlayer) { return; }

	spPlayer->PlayAnime("Run", true);
	m_departStartPos = spPlayer->WorkData().m_pos;

	// 行き先の方を向かせる
	Math::Vector3 dir = m_layout.m_depart.m_target - m_departStartPos;
	dir.y = 0.0f;
	if (dir.LengthSquared() > 0.0001f)
	{
		spPlayer->WorkData().m_rotDeg.y = DirectX::XMConvertToDegrees(atan2(dir.x, dir.z));
		spPlayer->ApplyData();
	}
}

bool CampManager::UpdateDepart()
{
	if (m_departState == DepartState::None) { return false; }

	const CampDepartData& dep = m_layout.m_depart;
	auto spPlayer = m_wpCampPlayer.lock();

	//---- 歩く(Walk でも Fade でも) ----
	m_departTimer += DeltaTime::Instance().GetGameDeltaTime();

	float t = 1.0f;
	if (dep.m_walkTime > 0.0f) { t = m_departTimer / dep.m_walkTime; }
	if (t > 1.0f) { t = 1.0f; }

	if (spPlayer)
	{
		spPlayer->WorkData().m_pos = Math::Vector3::Lerp(m_departStartPos, dep.m_target, t);
		spPlayer->ApplyData();

		//---- 足音(目的地に着くまで一定の間隔で) ----
		if (t < 1.0f)
		{
			m_departStepTimer -= DeltaTime::Instance().GetGameDeltaTime();
			if (m_departStepTimer <= 0.0f)
			{
				m_departStepTimer += kDepartStepInterval;
				SEPlayer::Play2D("Asset/Sound/SE/Prepare/DepartStep.wav", 0.7f);
			}
		}
	}

	//---- 状態ごと ----
	switch (m_departState)
	{
	case DepartState::Walk:
		if (t >= dep.m_fadeStartRate)
		{
			FadeDraw::Instance().SetFadeType(FadeOut, dep.m_fadeTime);	//暗転してからゲームへ
			m_departState = DepartState::Fade;

			//入口に吸い込まれる音
			SEPlayer::Play2D("Asset/Sound/SE/Prepare/DepartWhoosh.wav", 0.9f);

			// FadeDraw はこのフレームから時間を進めるので、こちらも同じフレームから数える
			m_departFadeTimer = DeltaTime::Instance().GetRealDeltaTime();
		}
		break;

	case DepartState::Fade:
		// FadeDraw と同じ Real の時間で数えて、真っ暗になったらゲームへ
		m_departFadeTimer += DeltaTime::Instance().GetRealDeltaTime();

		//暗転に合わせて焚き火の音を小さくしていく(BGM はシーンが GetDepartVolumeRate で合わせる)
		ApplyCampfireVolume(GetDepartVolumeRate());

		if (m_departFadeTimer >= dep.m_fadeTime) { return true; }
		break;

	default:
		break;
	}

	return false;
}

float CampManager::GetDepartVolumeRate() const
{
	if (m_departState != DepartState::Fade) { return 1.0f; }

	const float fadeTime = m_layout.m_depart.m_fadeTime;
	if (fadeTime <= 0.0f) { return 0.0f; }

	return std::clamp(1.0f - m_departFadeTimer / fadeTime, 0.0f, 1.0f);
}

//=============================================
// その他の処理
//=============================================

void CampManager::MoveCamera(const std::string& _name, bool _immediate)
{
	if (!m_spCamera) { return; }
	m_spCamera->MoveTo(_name, _immediate);
}

void CampManager::UpdateRackWeapons(const std::vector<bool>& _owned, int _equip)
{
	for (int id = 0; id < PlayerWeaponInfoSize; id++)
	{
		std::shared_ptr<CampObject> spWeapon = m_wpRackWeapons[id].lock();
		if (!spWeapon) { continue; }	// 武器立てに置いていない武器

		// 持っていて、装備していない武器だけを出す
		const bool isOwned = id < static_cast<int>(_owned.size()) && _owned[id];
		spWeapon->SetDrawFlg(isOwned && id != _equip);
	}
}

void CampManager::ApplyCampfireVolume(float _rate)
{
	m_campfireVolRate = _rate;

	for (const auto& fire : m_campfires)
	{
		if (fire.m_spSE) { fire.m_spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * _rate); }
	}
}

//=============================================
// エディタ用
//=============================================

std::shared_ptr<CampObject> CampManager::AddCampObject(const CampObjectData& _data)
{
	auto spObj = std::make_shared<CampObject>();
	spObj->Init();
	spObj->SetData(_data);
	m_objList.push_back(spObj);
	m_campObjects.push_back(spObj);

	RegisterSpecialObject(spObj);

	if (_data.m_kind == "Campfire") { StartCampfire(spObj); }

	return spObj;
}

void CampManager::RemoveCampObject(const std::shared_ptr<CampObject>& _spObj)
{
	if (!_spObj) { return; }

	// 焚き火なら炎と音も止める
	for (auto it = m_campfires.begin(); it != m_campfires.end(); )
	{
		if (it->m_wpObj.lock() == _spObj) { StopCampfire(*it); it = m_campfires.erase(it); }
		else                              { ++it; }
	}

	_spObj->Remove();	// m_objList からは次の PreUpdate で外れる
	std::erase(m_campObjects, _spObj);
}

void CampManager::OnCampObjectMoved(const std::shared_ptr<CampObject>& _spObj)
{
	if (!_spObj) { return; }
	_spObj->ApplyData();

	// 焚き火なら炎と音を同じ位置に出し直す
	for (auto& fire : m_campfires)
	{
		if (fire.m_wpObj.lock() != _spObj) { continue; }

		if (auto spEf = fire.m_wpEffect.lock())
		{
			spEf->SetPos(_spObj->WorkData().m_pos);
			spEf->WorkPlayEfkInfo().Pos = _spObj->WorkData().m_pos;
		}

		// 音は位置を変えられないので鳴らし直す
		if (fire.m_spSE) { fire.m_spSE->Stop(); }
		fire.m_spSE = SEPlayer::Play3D(kCampfireSEPath, _spObj->WorkData().m_pos, m_campfireVolRate, true);
	}
}

std::shared_ptr<CampLight> CampManager::AddCampLight(const CampLightData& _data)
{
	auto spLight = std::make_shared<CampLight>();
	spLight->SetData(_data);
	m_objList.push_back(spLight);
	m_campLights.push_back(spLight);
	return spLight;
}

void CampManager::RemoveCampLight(const std::shared_ptr<CampLight>& _spLight)
{
	if (!_spLight) { return; }

	_spLight->Remove();	// m_objList からは次の PreUpdate で外れる
	std::erase(m_campLights, _spLight);
}

bool CampManager::SaveLayout()
{
	CampLayoutData save;

	// 置物(出していない松明も残す)
	for (const auto& spObj : m_campObjects)
	{
		if (spObj) { save.m_objects.push_back(spObj->WorkData()); }
	}
	save.m_objects.insert(save.m_objects.end(), m_hiddenObjects.begin(), m_hiddenObjects.end());

	// ライト(出していない松明のライトも残す)
	for (const auto& spLight : m_campLights)
	{
		if (spLight) { save.m_lights.push_back(spLight->WorkData()); }
	}
	save.m_lights.insert(save.m_lights.end(), m_hiddenLights.begin(), m_hiddenLights.end());

	return SaveCampLayout(save, m_layoutFilePath);
}

void CampManager::PlayMerchantReaction(MerchantReaction _reaction)
{
	auto spMerchant = m_wpMerchant.lock();
	if (!spMerchant) { return; }	// 商人を置いていない

	spMerchant->PlayAnimeOnce(MerchantReactionAnime(_reaction), kMerchantIdleAnime);
}

//=============================================
// その他の処理
//=============================================

void CampManager::StartCampfire(const std::shared_ptr<CampObject>& _spObj)
{
	Campfire fire;
	fire.m_wpObj = _spObj;

	const Math::Vector3& pos = _spObj->WorkData().m_pos;

	// 炎。ループ再生
	fire.m_wpEffect = KdEffekseerManager::GetInstance().Play(kCampfireEffectPath, pos, 1.0f, 1.0f, true);

	// パチパチ音(ループ・焚き火の位置から聞こえる)
	fire.m_spSE = SEPlayer::Play3D(kCampfireSEPath, pos, m_campfireVolRate, true);

	m_campfires.push_back(fire);
}

void CampManager::StopCampfire(Campfire& _fire)
{
	if (_fire.m_spSE) { _fire.m_spSE->Stop(); _fire.m_spSE.reset(); }

	if (auto spEf = _fire.m_wpEffect.lock())
	{
		spEf->SetLoop(false);
		spEf->StopEffect();
	}
}

void CampManager::RegisterSpecialObject(const std::shared_ptr<CampObject>& _spObj)
{
	const CampObjectData& data = _spObj->WorkData();

	// 武器立ての武器なら、武器の種類ごとに覚えておく(表示を切り替えるため)
	for (const auto& rack : kRackWeaponNames)
	{
		if (data.m_name.rfind(rack.m_prefix, 0) == 0)
		{
			m_wpRackWeapons[rack.m_weapon] = _spObj;
			break;
		}
	}

	// プレイヤー(出発の演出で歩かせる)
	if (data.m_kind == "Player")
	{
		m_wpCampPlayer = _spObj;
		_spObj->PlayAnime("Idle", true);
	}

	// 商人(買った・売ったときの演出で動かす)
	if (data.m_kind == "Merchant")
	{
		m_wpMerchant = _spObj;
		_spObj->PlayAnime(kMerchantIdleAnime, true);
	}
}

