//---- 自分のヘッダー ----
#include "PlayerStatusInventoryUI.h"										//インベントリ画面(ステータスタブ)

//---- シーン ----
#include"../../../../Scene/SceneManager.h"									//シーンの管理
#include"../../../../Scene/GameScene/GameScene.h"							//ゲームシーン

//---- プレイヤー ----
#include"../../../Character/Player/PlayerBase.h"							//プレイヤー
#include"../../../Character/Player/PlayerInventory/PlayerInventory.h"		//プレイヤーの所持品

//---- 武器 ----
#include"../../../Weapon/WeaponBase.h"										//武器の基底クラス

//---- ポーション ----
#include"../../../Potions/PotionsType.h"									//ポーションの種類
#include"../../../Potions/PotionUseController.h"							//ポーションの使用処理
#include"../../../Potions/PotionTexInfo/PotionTexInfo.h"					//アイテムの名前・画像情報

//---- 便利機能 ----
#include"../../../../Info/KeyInfo/KeyInfo.h"								//キー入力
#include"../../../../Info/MouseInfo/MouseInfo.h"							//マウス入力
#include"../../../../Info/NumDraw/NumDraw.h"								//数字の画像描画

void PlayerStatusInventoryUI::Init()
{
	//////////////////////////////////////////////
	//プレイヤーステータス用
	if (!m_spRtTargetPack)
	{
		m_spRtTargetPack = std::make_shared<KdRenderTargetPack>();
		m_spRtTargetPack->CreateRenderTarget(1280, 720, true);
	}


	Math::Vector2 iconBasePos = { -170, 65 };
	Math::Vector2 nameBasePos = { iconBasePos.x + 70,iconBasePos.y };
	Math::Vector2 numBasePos = { nameBasePos.x + 370, iconBasePos.y };

	for (int i = 0; i < PlayerStatusInfoSize; i++)
	{
		m_playerStatusInfo[i].m_num = 0;
		m_playerStatusInfo[i].m_numPos = numBasePos;

		m_playerStatusInfo[i].m_numPos.y -= i * 65;

		m_playerStatusInfo[i].m_namePos = nameBasePos;
		m_playerStatusInfo[i].m_namePos.y = m_playerStatusInfo[i].m_numPos.y;


		m_playerStatusInfo[i].m_iconPos = iconBasePos;
		m_playerStatusInfo[i].m_iconPos.y = m_playerStatusInfo[i].m_numPos.y;

		if (!m_playerStatusInfo[i].m_spNameTex)
		{
			m_playerStatusInfo[i].m_spNameTex = std::make_shared<KdTexture>();

			std::string filePath = "Asset/Textures/GameUI/Item/PlayerInventory/PlayerStatusInfo/PlayerStatusName";
			filePath += std::to_string(i) + ".png";
			m_playerStatusInfo[i].m_spNameTex->Load(filePath);
		}

		if (!m_playerStatusInfo[i].m_spIconTex)
		{
			m_playerStatusInfo[i].m_spIconTex = std::make_shared<KdTexture>();

			std::string filePath = "Asset/Textures/GameUI/Item/PlayerInventory/PlayerStatusInfo/PlayerStatusIcon";
			filePath += std::to_string(i) + ".png";
			m_playerStatusInfo[i].m_spIconTex->Load(filePath);
		}

	}

	if (!m_spPlayerStatusInfoBackTex)
	{
		m_spPlayerStatusInfoBackTex = std::make_shared<KdTexture>();
		m_spPlayerStatusInfoBackTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/PlayerStatusInfo/back.png");
	}

	if (!m_spStatusCamera)
	{
		m_spStatusCamera = std::make_shared<KdCamera>();
		m_spStatusCamera->SetProjectionMatrix(45.0f, 2000.0f, 0.01f, 1280.0f / 720.0f);
	}
}

void PlayerStatusInventoryUI::Update()
{
	m_statusCamAngleY += m_statusCamRotateSpeed * DeltaTime::Instance().GetRealDeltaTime();
	if (m_statusCamAngleY >= 360.0f)
	{
		m_statusCamAngleY -= 360.0f;
	}
}

void PlayerStatusInventoryUI::PreDraw()
{
	if (!m_spRtTargetPack) { return; }
	std::shared_ptr<PlayerBase> spPlayer = m_spContext->m_wpPlayer.lock();
	if (spPlayer)
	{


		// レンダーターゲットを切り替え
		SceneManager::Instance().ChangeRendertarget(m_spRtTargetPack);
		// 背景を完全透明(アルファ0)にクリアしたい場合 (UIの上にキャラだけ重ねる場合におすすめ)
		m_spRtTargetPack->ClearTexture(Math::Color(0.0f, 0.0f, 0.0f, 0.0f));
		// 1. メインシーン用のカメラ設定を退避
		const auto prevCameraCB = KdShaderManager::Instance().GetCameraCB();

		// 3. プレイヤーの中心(注視点)とカメラ行列を計算
		Math::Vector3 targetPos = spPlayer->GetPos() + Math::Vector3(0.0f, m_statusCamHeightOffset, 0.0f);
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
		spPlayer->DrawLit();
		spPlayer->GetWeapon().lock()->DrawLit();

		KdShaderManager::Instance().m_StandardShader.EndLit();
		// レンダーターゲットを戻す
		SceneManager::Instance().UndoRenderTarget();
		// 6. メインカメラの定数バッファを復元 (メイン画面描画への影響を防止)
		KdShaderManager::Instance().WriteCBCamera(prevCameraCB.mView.Invert(), prevCameraCB.mProj);

		SceneManager::Instance().UnFogAmbient();
	}


	if (spPlayer)
	{

		std::shared_ptr<WeaponBase>spWeapon = spPlayer->GetWeapon().lock();
		if (!spWeapon) { return; }
		for (int i = 0; i < PlayerStatusInfoSize; i++)
		{
			int num = 0;

			switch (i)
			{

			case PlayerStatusInfo_HP:
				num = spPlayer->GetNowHP();
				break;
			case PlayerStatusInfo_MP:
				num = spPlayer->GetMaxMP();
				break;
			case PlayerStatusInfo_AttackPower:
				num = spPlayer->NowAttack() + spWeapon->GetAttack();
				break;
			case PlayerStatusInfo_DefensePower:
				num = spPlayer->NowDefense();
				break;
			case PlayerStatusInfo_Speed:
				num = spPlayer->GetBaseSpeed();
				break;
			default:
				break;
			}

			m_playerStatusInfo[i].m_num = num;
		}

	}


}

void PlayerStatusInventoryUI::DrawSprite()
{
	if (!m_spRtTargetPack) { return; }

	float scale = 0.25;
	Math::Vector2 size = { 1280 * scale ,720 * scale };


	Math::Vector2 pos = { m_spContext->m_back1Tex2DPos.x - m_spContext->m_back1TexWidth / 2 + size.x / 2,m_spContext->m_back1Tex2DPos.y };


	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spRtTargetPack->m_RTTexture, pos.x, pos.y, size.x, size.y);


	//ステータス描画
	Math::Vector2 pivot = { 0,0.5 };
	for (int i = 0; i < PlayerStatusInfoSize; i++)
	{
		Math::Color color;
		Math::Vector2 backPos = m_playerStatusInfo[i].m_namePos;
		Math::Vector2 backSize = { 380,55 };
		Math::Vector2 pivot = { 0,0.5 };
		if (i % 2 == 0)
		{
			color = { 1,1,1,0.3f };
		}
		else
		{
			color = { 0.5,0.5,0.5,0.3f };
		}

		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPlayerStatusInfoBackTex, backPos.x, backPos.y, backSize.x, backSize.y, nullptr, &color, pivot);



		if (m_playerStatusInfo[i].m_spNameTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_playerStatusInfo[i].m_spNameTex, m_playerStatusInfo[i].m_namePos.x, m_playerStatusInfo[i].m_namePos.y, nullptr, nullptr, pivot);
		}
		NumDraw::GetInstance().Draw(m_playerStatusInfo[i].m_num, RAligned, m_playerStatusInfo[i].m_numPos, kWhiteColor, 3, true);

		if (m_playerStatusInfo[i].m_spIconTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_playerStatusInfo[i].m_spIconTex, m_playerStatusInfo[i].m_iconPos.x, m_playerStatusInfo[i].m_iconPos.y, nullptr, nullptr, pivot);
		}
	}
}
