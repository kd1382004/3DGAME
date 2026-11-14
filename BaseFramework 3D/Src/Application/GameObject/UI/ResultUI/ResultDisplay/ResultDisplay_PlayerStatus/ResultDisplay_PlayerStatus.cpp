#include "ResultDisplay_PlayerStatus.h"
#include"../../../../../Info/NumDraw/NumDraw.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Character/Player/PlayerBase.h"
#include"../../../../Weapon/WeaponBase.h"

void ResultDisplay_PlayerStatus::Init()
{
	Math::Vector2 iconBasePos = { -170, 80 };
	Math::Vector2 nameBasePos = { iconBasePos.x + 70,iconBasePos.y };
	Math::Vector2 numBasePos = { nameBasePos.x + 370, iconBasePos.y };

	for (int i = 0; i < PlayerStatusInfoSiz; i++)
	{
		m_playerStatusInfo[i].num = 0;
		m_playerStatusInfo[i].numPos = numBasePos;

		m_playerStatusInfo[i].numPos.y -= i * 70;

		m_playerStatusInfo[i].namePos = nameBasePos;
		m_playerStatusInfo[i].namePos.y = m_playerStatusInfo[i].numPos.y;


		m_playerStatusInfo[i].iconPos = iconBasePos;
		m_playerStatusInfo[i].iconPos.y = m_playerStatusInfo[i].numPos.y;

		if (!m_playerStatusInfo[i].nameTex)
		{
			m_playerStatusInfo[i].nameTex = std::make_shared<KdTexture>();

			std::string filePath = "Asset/Textures/GameUI/Item/PlayerInventory/PlayerStatusInfo/PlayerStatusName";
			filePath += std::to_string(i) + ".png";
			m_playerStatusInfo[i].nameTex->Load(filePath);
		}

		if (!m_playerStatusInfo[i].iconTex)
		{
			m_playerStatusInfo[i].iconTex = std::make_shared<KdTexture>();

			std::string filePath = "Asset/Textures/GameUI/Item/PlayerInventory/PlayerStatusInfo/PlayerStatusIcon";
			filePath += std::to_string(i) + ".png";
			m_playerStatusInfo[i].iconTex->Load(filePath);
		}

	}

	if (!m_playerStatusInfoBackTex)
	{
		m_playerStatusInfoBackTex = std::make_shared<KdTexture>();
		m_playerStatusInfoBackTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/PlayerStatusInfo/back.png");
	}

	if (!m_spRtTargetPack)
	{
		m_spRtTargetPack = std::make_shared<KdRenderTargetPack>();
		m_spRtTargetPack->CreateRenderTarget(1280, 720, true);
	}

	if (!m_spStatusCamera)
	{
		m_spStatusCamera = std::make_shared<KdCamera>();
		m_spStatusCamera->SetProjectionMatrix(45.0f, 2000.0f, 0.01f, 1280.0f / 720.0f);
	}


	if (!m_spPlayerModel)
	{
		m_spPlayerModel = std::make_shared<KdModelWork>();
		m_spPlayerModel->SetModelData("Asset/Models/Character/Player/Rogue.gltf");

		//アニメータの準備
		m_spAnimetor = std::make_shared<KdAnimator>();
		m_spAnimetor->SetAnimation(m_spPlayerModel->GetAnimation("Idle"), true);
	}


	if (!m_statusTitleTex)
	{
		m_statusTitleTex = std::make_shared<KdTexture>();
		m_statusTitleTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_PlayerStatus/StatusTex.png");
		m_statusTitleTexPos = { 0,250 };

	}
}

void ResultDisplay_PlayerStatus::Update()
{
	if (m_spPlayerModel)
	{
		m_spAnimetor->AdvanceTime(m_spPlayerModel->WorkNodes(), 100);
		m_spPlayerModel->CalcNodeMatrices();

		// 装備していた武器を右手に持たせる（PlayerBase と同じ付け方）
		if (m_spWeapon)
		{
			auto* handNode = m_spPlayerModel->FindNode("Weapon.R");
			if (handNode)
			{
				Math::Vector3 rot = { 0,0,70 };
				Math::Matrix rMat = Math::Matrix::CreateFromYawPitchRoll(
					DirectX::XMConvertToRadians(rot.y),
					DirectX::XMConvertToRadians(rot.x),
					DirectX::XMConvertToRadians(rot.z));

				m_spWeapon->SetParentMatrix(rMat * handNode->m_worldTransform * m_mWorld);

				// Sword/Dagger の Update は攻撃の当たり判定などもするので、
				// 位置の計算だけの WeaponBase::Update を呼ぶ
				m_spWeapon->WeaponBase::Update();
			}
		}
	}



	m_statusCamAngleY += m_statusCamRotateSpeed * DeltaTime::Instance().GetRealDeltaTime();
	if (m_statusCamAngleY >= 360.0f)
	{
		m_statusCamAngleY -= 360.0f;
	}
}

void ResultDisplay_PlayerStatus::PreDraw()
{
	if (!m_spRtTargetPack) { return; }
	// レンダーターゲットを切り替え
	SceneManager::Instance().ChangeRendertarget(m_spRtTargetPack);
	// 背景を完全透明(アルファ0)にクリアしたい場合 (UIの上にキャラだけ重ねる場合におすすめ)
	m_spRtTargetPack->ClearTexture(Math::Color(0.0f, 0.0f, 0.0f, 0.0f));
	// 1. メインシーン用のカメラ設定を退避
	const auto prevCameraCB = KdShaderManager::Instance().GetCameraCB();

	// 3. プレイヤーの中心(注視点)とカメラ行列を計算
	Math::Vector3 targetPos = Math::Vector3(0.0f, m_statusCamHeightOffset, 0.0f);
	Math::Matrix localPos = Math::Matrix::CreateTranslation(0.0f, 0.0f, -m_statusCamDistance);
	Math::Matrix rotX = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_statusCamPitch));
	Math::Matrix rotY = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_statusCamAngleY));
	Math::Matrix trans = Math::Matrix::CreateTranslation(targetPos);
	// カメラのワールド行列: (距離分後ろへ移動) * (ピッチ回転) * (Y軸回転) * (注視点へ移動)
	Math::Matrix camWorld = localPos * rotX * rotY * trans;
	// 4. ステータス用カメラをシェーダーに適用
	KdShaderManager::Instance().WriteCBCamera(camWorld, m_spStatusCamera->GetProjMatrix());
	// 5. 3Dシェーダーのパスを開始してプレイヤーを描画
	KdShaderManager::Instance().m_StandardShader.BeginLit();

	//描画
	if (m_spPlayerModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spPlayerModel, m_mWorld, { 2,2,2,2 });
	}

	if (m_spWeapon)
	{
		m_spWeapon->DrawLit();
	}

	KdShaderManager::Instance().m_StandardShader.EndLit();
	// レンダーターゲットを戻す
	SceneManager::Instance().UndoRenderTarget();
	// 6. メインカメラの定数バッファを復元 (メイン画面描画への影響を防止)
	KdShaderManager::Instance().WriteCBCamera(prevCameraCB.mView.Invert(), prevCameraCB.mProj);

	SceneManager::Instance().UnFogAmbient();

}

void ResultDisplay_PlayerStatus::DrawSprite()
{
	if (m_statusTitleTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_statusTitleTex, m_statusTitleTexPos.x, m_statusTitleTexPos.y);
	}


	if (!m_spRtTargetPack) { return; }

	float scal = 0.25;
	Math::Vector2 siz = { 1280 * scal ,720 * scal };

	Math::Vector2 pos = { -250,0 };

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spRtTargetPack->m_RTTexture, pos.x, pos.y, siz.x, siz.y);

	//ステータス描画
	Math::Vector2 pivo = { 0,0.5 };
	for (int i = 0; i < PlayerStatusInfoSiz; i++)
	{
		Math::Color color;
		Math::Vector2 backPos = m_playerStatusInfo[i].namePos;
		Math::Vector2 backSiz = { 400,60 };
		Math::Vector2 pivo = { 0,0.5 };
		if (i % 2 == 0)
		{
			color = { 1,1,1,0.3f };
		}
		else
		{
			color = { 0.5,0.5,0.5,0.3f };
		}

		KdShaderManager::Instance().m_spriteShader.DrawTex(m_playerStatusInfoBackTex, backPos.x, backPos.y, backSiz.x, backSiz.y, nullptr, &color, pivo);



		if (m_playerStatusInfo[i].nameTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_playerStatusInfo[i].nameTex, m_playerStatusInfo[i].namePos.x, m_playerStatusInfo[i].namePos.y, nullptr, nullptr, pivo);
		}
		NumDraw::GetInstance().Drow(m_playerStatusInfo[i].num, RAligned, m_playerStatusInfo[i].numPos, kWhiteColor, 4, true);

		if (m_playerStatusInfo[i].iconTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_playerStatusInfo[i].iconTex, m_playerStatusInfo[i].iconPos.x, m_playerStatusInfo[i].iconPos.y, nullptr, nullptr, pivo);
		}
	}
}
void ResultDisplay_PlayerStatus::SetPlayerStatusGameResultFilePath(std::string _path)
{
	m_spResultPlayer = std::make_shared<PlayerBase>();
	m_spResultPlayer->Init();
	m_spResultPlayer->LoadCharaStatus(_path);

	RefreshStatusNum();
}

void ResultDisplay_PlayerStatus::SetSpWeaponBase(std::shared_ptr<WeaponBase> _spWeapon)
{
	m_spWeapon = _spWeapon;

	RefreshStatusNum();
}

void ResultDisplay_PlayerStatus::RefreshStatusNum()
{
	//PlayerInventoryUI::PlayerStatusPreDraw と同じ計算
	if (!m_spResultPlayer) { return; }
	if (!m_spWeapon) { return; }

	for (int i = 0; i < PlayerStatusInfoSiz; i++)
	{
		int num = 0;

		switch (i)
		{
		case PlayerStatusInfo_HP:
			num = m_spResultPlayer->GetNowHP();
			break;
		case PlayerStatusInfo_MP:
			num = m_spResultPlayer->GetMaxMP();
			break;
		case PlayerStatusInfo_AttackPower:
			num = m_spResultPlayer->NowAttack() + m_spWeapon->GetAttack();
			break;
		case PlayerStatusInfo_DefensePower:
			num = m_spResultPlayer->NowDefense();
			break;
		case PlayerStatusInfo_Speed:
			num = m_spResultPlayer->GetBaseSpeed();
			break;
		default:
			break;
		}

		m_playerStatusInfo[i].num = num;
	}
}
