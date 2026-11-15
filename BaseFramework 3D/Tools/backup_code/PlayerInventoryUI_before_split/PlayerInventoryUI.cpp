#include "PlayerInventoryUI.h"
#include"../../../Scene/SceneManager.h"



#include"../../../Info/KeyInfo/KeyInfo.h"
#include"../../../Scene/GameScene/GameScene.h"
#include"../../../Info/MouseInfo/MouseInfo.h"

#include"../../Potions/PotionsType.h"
#include"../../Potions/PotionUseController.h"


#include"../../Character/Player/PlayerBase.h"
#include"../../Character/Player/PlayerInventory/PlayerInventory.h"

#include"../../Potions/PotionTexInfo/PotionTexInfo.h"

#include"../../../Info/NumDraw/NumDraw.h"

#include"../../Weapon/WeaponBase.h"
void PlayerInventoryUI::Init()
{
	KeyInfo::Instance().SetKeyValid(VK_TAB);
	m_playerInventoryUIFlg = false;

	if (!m_back1Tex)
	{
		m_back1Tex = std::make_shared<KdTexture>();
		m_back1Tex->Load("Asset/Textures/GameUI/Item/PlayerInventory/BagBackTex1.png");
		m_back1Tex2DPos = { -100,0 };

	}


	if (!m_inventoryTypeTex)
	{
		m_inventoryTypeTex = std::make_shared<KdTexture>();
		m_inventoryTypeTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/inventoryType/inventoryType.png");
		m_inventoryTypeSiz = { (float)m_inventoryTypeTex->GetWidth(),(float)m_inventoryTypeTex->GetHeight() / 3 };
		m_inventoryType2DPos.x = m_back1Tex2DPos.x - m_back1Tex->GetWidth() / 2;
		m_inventoryType2DPos.y = m_back1Tex2DPos.y + 150;

	}


	if (!m_notSelsect)
	{
		m_notSelsect = std::make_shared<KdTexture>();
		m_notSelsect->Load("Asset/Textures/GameUI/Item/PlayerInventory/BagBackTex2.png");
		m_back2Tex = m_notSelsect;
		m_back2Tex2DPos = m_back1Tex2DPos + Math::Vector2{ (float)m_back1Tex->GetWidth() / 2,0 } + Math::Vector2{ (float)m_back2Tex->GetWidth(),0 };
		m_back2Tex2DSiz = { (float)m_back2Tex->GetWidth() , (float)m_back2Tex->GetHeight() };
	}


	if (!m_NotselectNameTex)
	{
		m_NotselectNameTex = std::make_shared<KdTexture>();
		m_NotselectNameTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/SelectNameBase.png");
		m_selectNameTex = m_NotselectNameTex;
		m_selectNameTexPos = m_back2Tex2DPos;
		m_selectNameTexPos.y = 223;
	}


	if (!m_selectExplanationTex)
	{
		m_NotselectExplanationTex = std::make_shared<KdTexture>();
		m_NotselectExplanationTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/SelectExplanationBase.png");
		m_selectExplanationTex = m_NotselectExplanationTex;
		m_selectExplanationTexPos = m_back2Tex2DPos;
		m_selectExplanationTexPos.y = 50;
	}

	if (!m_UseTex)
	{
		m_UseTex = std::make_shared<KdTexture>();
		m_UseTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/UseTex.png");

		float x = m_back2Tex2DPos.x;
		float y = m_back2Tex2DPos.y - m_back2Tex->GetHeight() / 2 + m_UseTex->GetHeight() / 2 + 20;
		m_UseTex2DPos = { x,y };
	}

	if (!m_potionSelectTex)
	{
		m_potionSelectTex = std::make_shared<KdTexture>();
		m_potionSelectTex->Load("Asset/Textures/GameUI/Item/Potion/PotionSelect.png");
	}

	m_iconDimensions = { 40,40 };
	m_layerPriority = 1;



	Math::Vector2 basePos = { -420, 225 };
	for (int i = 0; i < InventoryTypeSiz; i++)
	{
		if (m_inventoryTypeChange[i].m_IconTex)
		{
			continue;
		}

		m_inventoryTypeChange[i].m_IconTex = std::make_shared<KdTexture>();


		switch (i)
		{
		case PlayerInventoryUI::PotionInventory:
			m_inventoryTypeChange[i].m_IconTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/inventoryType/Potion.png");
			m_inventoryTypeChange[i].m_ID = PotionInventory;
			break;
		case PlayerInventoryUI::WeponInventory:
			m_inventoryTypeChange[i].m_IconTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/inventoryType/Wepon.png");
			m_inventoryTypeChange[i].m_ID = WeponInventory;
			break;
		case PlayerInventoryUI::PlayerStatus:
			m_inventoryTypeChange[i].m_IconTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/inventoryType/PlayerStatus.png");
			m_inventoryTypeChange[i].m_ID = PlayerStatus;
			break;
		default:
			break;
		}


		float w = m_inventoryTypeChange[i].m_IconTex->GetWidth();
		float h = m_inventoryTypeChange[i].m_IconTex->GetHeight();
		m_inventoryTypeChange[i].m_IconTexSiz = { w,h };


		m_inventoryTypeChange[i].m_2DPos = basePos;
		m_inventoryTypeChange[i].m_2DPos.x += i * w + i * 10;
	}


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

	for (int i = 0; i < PlayerStatusInfoSiz; i++)
	{
		m_playerStatusInfo[i].num = 0;
		m_playerStatusInfo[i].numPos = numBasePos;

		m_playerStatusInfo[i].numPos.y -= i * 65;

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

	if (!m_spStatusCamera)
	{
		m_spStatusCamera = std::make_shared<KdCamera>();
		m_spStatusCamera->SetProjectionMatrix(45.0f, 2000.0f, 0.01f, 1280.0f / 720.0f);
	}

	////////////////////////////////////////////////////////////////////////////
	//武器強化用
	Math::Vector2 weaponsStrengtheningBasePos = { m_back1Tex2DPos.x,-200 };
	float weaponsStrengtheningOffset = 150.0f;
	float vecL = 150.0f;

	if (!m_weaponsStrengtheningButtonTex)
	{
		m_weaponsStrengtheningButtonTex2DPos = m_UseTex2DPos;
		m_weaponsStrengtheningButtonTex = std::make_shared<KdTexture>();
		m_weaponsStrengtheningButtonTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/StrengtheningButton.png");
	}
	if (!m_PreconditionNodeTex)
	{
		m_weaponsStrengtheningButtonTex2DPos = m_UseTex2DPos;
		m_PreconditionNodeTex = std::make_shared<KdTexture>();
		m_PreconditionNodeTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Not.png");
	}

	if (!m_PreconditionNodeIconTex)
	{
		m_PreconditionNodeIconTex = std::make_shared<KdTexture>();
		m_PreconditionNodeIconTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/NotIcon.png");
	}

	if (!m_PreconditionNodeExplanationTex)
	{
		m_PreconditionNodeExplanationTex = std::make_shared<KdTexture>();
		m_PreconditionNodeExplanationTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/NotSelectExplanationBase.png");
	}
	if (!m_PreconditionNodeNameTex)
	{
		m_PreconditionNodeNameTex = std::make_shared<KdTexture>();
		m_PreconditionNodeNameTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/NotSelectNameBase.png");
	}

	if (!m_LvMaxButtonTex)
	{
		m_LvMaxButtonTex = std::make_shared<KdTexture>();
		m_LvMaxButtonTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Max.png");
	}

	if (!m_LVTex)
	{
		m_LVTex = std::make_shared<KdTexture>();
		m_LVTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/LV.png");
		Math::Vector2 pos = m_back2Tex2DPos;
		pos.y += 150;
		pos.x -= 80;
		m_LVTex2DPos = pos;

	}

	if (!m_LineTex)
	{
		m_LineTex = std::make_shared<KdTexture>();
		m_LineTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Line.png");
	}

	float rad = DirectX::XMConvertToRadians(90.0f); // 0°,90°,180°,270°

	m_selectWeaponsStrengtheningIconSiz = { 64,64 };

	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		Math::Vector2 pos = weaponsStrengtheningBasePos;

		m_weaponsStrengtheningInfo[i].Lv = 0;

		float addY = 100;

		switch (i)
		{
		case PlayerInventoryUI::WeaponsStrengthening_Attck:
			pos.x -= weaponsStrengtheningOffset * 2;
			pos.y += addY;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_Stamina:
			pos.x -= weaponsStrengtheningOffset;
			pos.y += addY;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_ChargeTime:
			pos.x += weaponsStrengtheningOffset * 2;
			pos.y += addY;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_Characteristic:
			pos.x = m_back1Tex2DPos.x;
			pos.y += addY;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_DefenseIgnore:
			pos.x += weaponsStrengtheningOffset;
			pos.y += addY;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_Characteristic1:
			rad = DirectX::XMConvertToRadians(45.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.y + cos(rad) * vecL;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_Characteristic2:
			rad = DirectX::XMConvertToRadians(0.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.y + cos(rad) * vecL;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_Characteristic3:
			rad = DirectX::XMConvertToRadians(-45.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.y + cos(rad) * vecL;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_Icon:
			break;
		case PlayerInventoryUI::WeaponsStrengtheningSiz:
			break;
		default:
			break;
		}

		// 前提ノードはマスタから読む（WeaponsStrengtheningInfoInit）
		m_weaponsStrengtheningInfo[i].iconPos = pos;
		m_weaponsStrengtheningInfo[i].iconSiz = m_selectWeaponsStrengtheningIconSiz;
		m_weaponsStrengtheningInfo[i].nextLvNum = 0;

		if (!m_weaponsStrengtheningInfo[i].iconTex)
		{
			m_weaponsStrengtheningInfo[i].iconTex = std::make_shared<KdTexture>();
			std::string filePath = "Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon";
			switch (i)
			{
			case PlayerInventoryUI::WeaponsStrengthening_Characteristic:
			case PlayerInventoryUI::WeaponsStrengthening_Characteristic1:
			case PlayerInventoryUI::WeaponsStrengthening_Characteristic2:
			case PlayerInventoryUI::WeaponsStrengthening_Characteristic3:
			case PlayerInventoryUI::WeaponsStrengthening_Icon:



				switch (SceneManager::Instance().GetPlayerWeaponInfo())
				{
				case PlayerWeaponInfo::PlayerWeaponInfo_Dagger:
					filePath += "/Dagger";
					break;
				case PlayerWeaponInfo::PlayerWeaponInfo_Sword:
					filePath += "/Sword";
					break;

				default:
					break;
				}


				break;
			default:
				break;
			}

			filePath += "/WeponStrengtheningIcon";
			filePath += std::to_string(i) + ".png";
			m_weaponsStrengtheningInfo[i].iconTex->Load(filePath);
		}
	}
}

void PlayerInventoryUI::PreUpdate()
{

}

void PlayerInventoryUI::Update()
{
	PlayerInventoryOpen();

	if (m_useEffectTimer > 0.0f)
	{
		m_useEffectTimer -= DeltaTime::Instance().GetRealDeltaTime();
		if (m_useEffectTimer < 0.0f) { m_useEffectTimer = 0.0f; }
	}


	if (m_playerInventoryUIFlg)
	{
		InventoryTypeChangeUpdate();

		switch (m_nowInventoryType)
		{
		case PlayerInventoryUI::PotionInventory:
			PotionUpdate();
			break;
		case PlayerInventoryUI::WeponInventory:
			WeaponsStrengtheningUpdate();
			break;
		case PlayerInventoryUI::PlayerStatus:
			PlayerStatusUpdate();
			break;
		default:
			break;
		}


	}


	//debaggu
	static bool flg = true;
	if (GetAsyncKeyState('1') & 0x8000)
	{
		//if (!flg)
		{
			std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
			if (spPlayer)
			{
				std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
				if (!spPlayerInventory) { return; }

				for (Inventory potion : spPlayerInventory->GetPotionsInventory())
				{
					spPlayer->GetPlayerInventory()->AddPotionsInventory(potion.m_ID);
				}
				for (Inventory weapon : spPlayerInventory->GetWeaponsStrengtheningInventory())
				{
					spPlayer->GetPlayerInventory()->AddWeaponsStrengtheningInventory(weapon.m_ID);
				}
			}
		}

		flg = true;
	}
	else
	{
		flg = false;
	}
}

void PlayerInventoryUI::PreDraw()
{
	switch (m_nowInventoryType)
	{
	case PlayerInventoryUI::PotionInventory:
		break;
	case PlayerInventoryUI::WeponInventory:
		break;
	case PlayerInventoryUI::PlayerStatus:
		PlayerStatusPreDraw();
		break;
	default:
		break;
	}

}

void PlayerInventoryUI::DrawSprite()
{
	if (!m_playerInventoryUIFlg) { return; }


	if (m_back1Tex)
	{
		Math::Color Color = { 1,1,1,1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_back1Tex, m_back1Tex2DPos.x, m_back1Tex2DPos.y, nullptr, &Color);
	}

	if (m_inventoryTypeTex)
	{
		Math::Vector2 pivo = { 0,0.5 };
		Math::Rectangle rec = { 0,(long)(m_inventoryTypeSiz.y * m_nowInventoryType),(long)(m_inventoryTypeSiz.x),(long)(m_inventoryTypeSiz.y) };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_inventoryTypeTex, m_inventoryType2DPos.x, m_inventoryType2DPos.y, m_inventoryTypeSiz.x, m_inventoryTypeSiz.y, &rec, nullptr, pivo);
	}

	if (m_back2Tex && m_nowInventoryType != PlayerInventoryUI::PlayerStatus)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_back2Tex, m_back2Tex2DPos.x, m_back2Tex2DPos.y, m_back2Tex2DSiz.x, m_back2Tex2DSiz.y);

		Math::Vector2 pos = m_back2Tex2DPos;
		pos.y += 150;
		pos.x += 85;

		// 数字が変わるときはちょっと跳ねる(色も変わる)
		Math::Color numColor = kWhiteColor;
		float numScale = 4.0f;
		if (m_useEffectTimer > 0.0f && (m_nowInventoryType == PotionInventory || m_nowInventoryType == WeponInventory))
		{
			float t = sinf(DirectX::XM_PI * (1.0f - m_useEffectTimer / m_useEffectTime));
			pos.y += 10.0f * t;
			numScale += 0.6f * t;

			switch (m_nowInventoryType)
			{
			case PlayerInventoryUI::PotionInventory:
				numColor = { 1.0f, 1.0f - 0.6f * t, 1.0f - 0.6f * t, 1.0f };   // 白→赤→白
				break;
			case PlayerInventoryUI::WeponInventory:
				numColor = { 1.0f, 1.0f - 0.15f * t, 1.0f - 0.7f * t, 1.0f };   // 白→金→白
				break;
			default:
				break;
			}

		}
		NumDraw::GetInstance().Drow(m_num, RAligned, pos, numColor, numScale);

		if (m_selectExplanationTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_selectExplanationTex, m_selectExplanationTexPos.x, m_selectExplanationTexPos.y);
		}


		if (m_selectNameTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_selectNameTex, m_selectNameTexPos.x, m_selectNameTexPos.y);
		}
	}



	//////////////////////////////////////////////////


	for (int i = 0; i < InventoryTypeSiz; i++)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_inventoryTypeChange[i].m_IconTex, m_inventoryTypeChange[i].m_2DPos.x, m_inventoryTypeChange[i].m_2DPos.y);


		if (i == m_nowInventoryType)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_potionSelectTex, m_inventoryTypeChange[i].m_2DPos.x, m_inventoryTypeChange[i].m_2DPos.y);

		}
	}

	//////////////////////////////////////////////////

	switch (m_nowInventoryType)
	{
	case PlayerInventoryUI::PotionInventory:
		PotionDraw();

		if (m_UseTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_UseTex, m_UseTex2DPos.x, m_UseTex2DPos.y);
		}
		break;
	case PlayerInventoryUI::WeponInventory:
		WeaponsStrengtheningDraw();
		break;
	case PlayerInventoryUI::PlayerStatus:
		PlayerStatusDraw();
		break;
	default:
		break;
	}



}

void PlayerInventoryUI::PlayerInventoryOpen()
{

	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		if (spPlayer->GetIsDead())
		{
			m_playerInventoryUIFlg = false;
			return;
		}
	}


	if (KeyInfo::Instance().GetValidKeyPush(VK_TAB, true))
	{
		m_playerInventoryUIFlg = !m_playerInventoryUIFlg;

		std::shared_ptr spGameScene = m_wpGameScene.lock();
		if (spGameScene)
		{
			//開いてたら止まるため
			spGameScene->SetPoseFlg(m_playerInventoryUIFlg);
			m_nowInventoryType = PotionInventory;
		}

		MouseInfo::Instance().SetMouseFreeFlg(m_playerInventoryUIFlg);


		if (m_playerInventoryUIFlg)
		{
			if (spPlayer)
			{
				spPlayer->StopSound();
			}

			AddPotionTexInfo();
			if (m_itemIconInfo.size() > 0)
			{
				m_selectExplanationTex = m_itemIconInfo[0].m_ExplanationTex;
				m_selectNameTex = m_itemIconInfo[0].m_NameTex;
				m_selectPotionID = m_itemIconInfo[0].m_ItemID;
			}
			else
			{
				m_Changeback2Tex = m_notSelsect;
				m_selectNameTex = m_NotselectNameTex;
			}
		}
	}

}

void PlayerInventoryUI::InventoryTypeChangeUpdate()
{

	POINT mousePos = MouseInfo::Instance().m_windowPos;

	for (auto& Icon : m_inventoryTypeChange)
	{
		float Left = Icon.m_2DPos.x - Icon.m_IconTexSiz.x / 2;
		float Right = Icon.m_2DPos.x + Icon.m_IconTexSiz.x / 2;
		float Top = Icon.m_2DPos.y + Icon.m_IconTexSiz.y / 2;
		float Bot = Icon.m_2DPos.y - Icon.m_IconTexSiz.y / 2;


		if (mousePos.x >= Left && mousePos.x <= Right &&
			mousePos.y >= Bot && mousePos.y <= Top)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			Icon.m_hit = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_nowInventoryType = Icon.m_ID;

				switch (m_nowInventoryType)
				{
				case PlayerInventoryUI::PotionInventory:
					PotionInfoInit();
					break;
				case PlayerInventoryUI::WeponInventory:
					WeaponsStrengtheningInfoInit();
					break;
				default:
					break;
				}
			}
		}
		else
		{
			Icon.m_hit = false;
		}
	}




}

void PlayerInventoryUI::PotionUpdate()
{
	AddPotionTexInfo();

	IconHit();

	PotionIUse();
}

void PlayerInventoryUI::WeaponsStrengtheningUpdate()
{
	//持ってる数を武器に渡す
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	std::shared_ptr<WeaponBase>spWeapon = spPlayer->GetWeapon().lock();
	if (spPlayer && spWeapon)
	{
		for (int i = 0; i < WeaponsStrengtheningSiz; i++)
		{
			int WeaponsStrengtheningItemID = m_weaponsStrengtheningInfo[i].strengtheningItem_ID;
			int num = spPlayer->GetPlayerInventory()->GetWeaponsStrengtheningInventoryNum(WeaponsStrengtheningItemID);
			spWeapon->SetWeaponsStrengtheningNum(i, num);
		}

	}

	WeaponsStrengtheningIconHit();

	WeaponsStrengtheningTexInfo();

	WeaponsStrengtheningButtonHit();
}

void PlayerInventoryUI::PlayerStatusUpdate()
{
	m_statusCamAngleY += m_statusCamRotateSpeed * DeltaTime::Instance().GetRealDeltaTime();
	if (m_statusCamAngleY >= 360.0f)
	{
		m_statusCamAngleY -= 360.0f;
	}
}

void PlayerInventoryUI::PotionDraw()
{
	// 0→1→0 と山なりに変化する値（演出の強さ）
	float t = 0.0f;
	if (m_useEffectTimer > 0.0f)
	{
		t = sinf(DirectX::XM_PI * (1.0f - m_useEffectTimer / m_useEffectTime));
	}

	for (auto& tex : m_itemIconInfo)
	{
		float w = m_iconDimensions.w;
		float h = m_iconDimensions.h;

		bool isUseEffect = (t > 0.0f && tex.m_ItemID == m_useEffectItemID);
		if (isUseEffect)
		{
			w *= 1.0f + 0.3f * t;
			h *= 1.0f + 0.3f * t;
		}

		KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_IconTex, tex.m_2DPos.x, tex.m_2DPos.y, w, h);

		// 白く光らせる（同じアイコンを加算で重ねる）
		if (isUseEffect)
		{
			Math::Color flash = { 1,1,1,t * 0.8f };
			KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
			KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_IconTex, tex.m_2DPos.x, tex.m_2DPos.y, w, h, nullptr, &flash);
			KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
		}

		if (tex.m_ItemID == m_selectPotionID)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_potionSelectTex, tex.m_2DPos.x, tex.m_2DPos.y, w, h);
		}
	}
}

void PlayerInventoryUI::WeaponsStrengtheningDraw()
{

	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		int x1 = 0;
		int x2 = 0;
		int y1 = 0;
		int y2 = 0;
		int PrIndex = m_weaponsStrengtheningInfo[i].PreconditionNodeID_index;
		if (PrIndex < 0) { continue; } // 前提なし（根元）は線を引かない

		x1 = m_weaponsStrengtheningInfo[i].iconPos.x;
		y1 = m_weaponsStrengtheningInfo[i].iconPos.y;

		x2 = m_weaponsStrengtheningInfo[PrIndex].iconPos.x;
		y2 = m_weaponsStrengtheningInfo[PrIndex].iconPos.y;


		float dx = x2 - x1;
		float dy = y2 - y1;

		// ラジアン
		float rad = atan2f(dy, dx);

		// 度数法（-180 ～ +180）
		float deg = rad * 180.0f / 3.14159265f;


		Math::Vector2 pivo = { 0,0.5 };
		Math::Vector2 siz = { 0,20 };
		siz.x = sqrtf(dx * dx + dy * dy);
		Math::Color col = kWhiteColor;
		float defoScale = 0.8;
		float scale = 1;

		int PreconditionNodeID_index = m_weaponsStrengtheningInfo[i].PreconditionNodeID_index;

		if (PreconditionNodeID_index < 0 || PreconditionNodeID_index >= WeaponsStrengtheningSiz)
		{
			break;
		}

		if (m_weaponsStrengtheningInfo[PreconditionNodeID_index].Lv == 0)
		{
			col = { 1,1,1,0.5 };
			defoScale = 0.5;
		}

		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
		if (m_LineTex)
		{
			// 通常の線
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_LineTex, x1, y1, siz.x, siz.y * defoScale, nullptr, &col, pivo, deg);

			// Lv > 0 なら(強化されてるなら)光らせる
			if (m_weaponsStrengtheningInfo[i].Lv > 0)
			{
				defoScale = 1;
				const int   glowNum = 4;      // 重ねる枚数
				const float glowMax = 3.0f;   // 一番外側の太さ倍率

				for (int j = 1; j <= glowNum; j++)
				{
					float t = (float)j / glowNum;           // 0～1
					float scale = 1.0f + t * (glowMax - 1.0f);
					float alpha = 0.4f * (1.0f - t) + 0.05f;
					scale *= defoScale;
					Math::Color glowCol = { col.x, col.y, col.z, col.w * alpha };

					KdShaderManager::Instance().m_spriteShader.DrawTex(m_LineTex, x1, y1, siz.x, siz.y * scale, nullptr, &glowCol, pivo, deg);
				}
			}
		}
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);

	}

	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		if (!m_weaponsStrengtheningInfo[i].iconTex) { continue; }
		bool PreconditionNodeIconFlg = false;

		float choseSizUp = 1;
		if (m_weaponsStrengtheningInfo[i].ChooseFlg)
		{
			choseSizUp = 1.3;
		}


		Math::Color iconCol = { 1,1,1,1 };


		const int preIndex = m_weaponsStrengtheningInfo[i].PreconditionNodeID_index;
		if (preIndex >= 0 && m_weaponsStrengtheningInfo[preIndex].Lv == 0)
		{
			m_weaponsStrengtheningInfo[i].strengtheningFlg = false;
			iconCol = { 0.5,0.5,0.5,1 };
			PreconditionNodeIconFlg = true;
		}

		if (m_weaponsStrengtheningInfo[i].Lv == 0)
		{
			iconCol = { 0.5,0.5,0.5,1 };
		}



		
		// 強化が可能なら光らす
		if (m_weaponsStrengtheningInfo[i].strengtheningFlg && !m_weaponsStrengtheningInfo[i].LvMaxFlg)
		{
			KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
			// 時間経過による点滅（パルス）アニメーション
			m_weaponsStrengtheningInfo[i].timer += 0.01f;

			float pulse = (sinf(m_weaponsStrengtheningInfo[i].timer) + 1.0f) * 0.5f;

			// ★ 大きくなって小さくなるスケール
			float scale = 1.0f + pulse * 0.25f;

			// 青(0,0,1) → 白(1,1,1) → 青(0,0,1)
			float r = pulse;
			float g = pulse;
			float b = 1.0f;

			float alpha = 1;

			Math::Color auraColor = { r, g, b, alpha };

			// 発光オーラ描画
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_weaponsStrengtheningInfo[i].iconTex,
				m_weaponsStrengtheningInfo[i].iconPos.x,
				m_weaponsStrengtheningInfo[i].iconPos.y,
				m_weaponsStrengtheningInfo[i].iconSiz.x * scale * choseSizUp,
				m_weaponsStrengtheningInfo[i].iconSiz.y * scale * choseSizUp,
				nullptr,
				&auraColor
			);

			if (m_weaponsStrengtheningInfo[i].timer > 1)
			{
				m_weaponsStrengtheningInfo[i].timer -= 1;
			}
			KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
		}





		// 0→1→0 と山なりに変化する値
		float t = 0.0f;
		if (m_useEffectTimer > 0.0f)
		{
			t = sinf(DirectX::XM_PI * (1.0f - m_useEffectTimer / m_useEffectTime));
		}
		float w = m_weaponsStrengtheningInfo[i].iconSiz.x;
		float h = m_weaponsStrengtheningInfo[i].iconSiz.y;
		bool isUseEffect = (t > 0.0f && m_weaponsStrengtheningInfo[i].MyNodeID == m_useEffectItemID);
		if (isUseEffect)
		{
			w *= 1.0f + 0.3f * t;
			h *= 1.0f + 0.3f * t;
		}

		//通常アイコン
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_weaponsStrengtheningInfo[i].iconTex, m_weaponsStrengtheningInfo[i].iconPos.x, m_weaponsStrengtheningInfo[i].iconPos.y, w * choseSizUp, h * choseSizUp, nullptr, &iconCol);


		//使われてたら光らす
		if (isUseEffect)
		{
			Math::Color flash = { 1,1,1,t * 0.8f };
			KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_weaponsStrengtheningInfo[i].iconTex, m_weaponsStrengtheningInfo[i].iconPos.x, m_weaponsStrengtheningInfo[i].iconPos.y, w * choseSizUp, h * choseSizUp, nullptr, &flash);
			KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
		}

		//前提ノードが解放されてなければ描画
		if (m_PreconditionNodeIconTex && PreconditionNodeIconFlg)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_PreconditionNodeIconTex, m_weaponsStrengtheningInfo[i].iconPos.x, m_weaponsStrengtheningInfo[i].iconPos.y, m_weaponsStrengtheningInfo[i].iconSiz.x * choseSizUp, m_weaponsStrengtheningInfo[i].iconSiz.y * choseSizUp, nullptr, &iconCol);
		}


	}

	if (!m_weaponsStrengtheningInfo[m_selectIndex].LvMaxFlg)
	{
		if (m_weaponsStrengtheningIconInfo.m_IconTex)
		{
			{
				KdShaderManager::Instance().m_spriteShader.DrawTex(m_weaponsStrengtheningIconInfo.m_IconTex, m_weaponsStrengtheningIconInfo.m_2DPos.x, m_weaponsStrengtheningIconInfo.m_2DPos.y, 60, 60);
				{
					//現在の持ってるアイテムの数
					Math::Vector2 pos = m_weaponsStrengtheningIconInfo.m_2DPos;
					pos.y -= 45;
					pos.x -= 5;

					Math::Color color = kWhiteColor;
					if (m_weaponsStrengtheningIconInfo.m_num < m_selectWeaponsStrengtheningnextLVNum)
					{
						color = kRedColor;
					}
					NumDraw::GetInstance().Drow(m_weaponsStrengtheningIconInfo.m_num, Aligned::RAligned, pos, color, 1.5);
				}
				{
					//レヴェルアップに必要なアイテムの数
					Math::Vector2 pos = m_weaponsStrengtheningIconInfo.m_2DPos;
					pos.y -= 50;
					pos.x += 5;
					NumDraw::GetInstance().Drow(m_selectWeaponsStrengtheningnextLVNum, Aligned::LAligned, pos, kWhiteColor, 1.5);
				}
			}
		}



		//強化ボタン
		if (m_weaponsStrengtheningButtonTex && !m_PreconditionNodeFlg)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_weaponsStrengtheningButtonTex, m_weaponsStrengtheningButtonTex2DPos.x, m_weaponsStrengtheningButtonTex2DPos.y);
		}

		//前提ノード
		if (m_PreconditionNodeTex && m_PreconditionNodeFlg)
		{

			KdShaderManager::Instance().m_spriteShader.DrawTex(m_PreconditionNodeTex, m_weaponsStrengtheningButtonTex2DPos.x, m_weaponsStrengtheningButtonTex2DPos.y);

		}

	}
	else
	{
		//レベルマックス
		if (m_LvMaxButtonTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_LvMaxButtonTex, m_weaponsStrengtheningButtonTex2DPos.x, m_weaponsStrengtheningButtonTex2DPos.y);
		}

	}
	//LV
	if (m_LVTex)
	{
		Math::Vector2 pivo = { 0,0.5 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_LVTex, m_LVTex2DPos.x, m_LVTex2DPos.y, nullptr, nullptr, pivo);
	}

}

void PlayerInventoryUI::PlayerStatusDraw()
{
	if (!m_spRtTargetPack) { return; }

	float scal = 0.25;
	Math::Vector2 siz = { 1280 * scal ,720 * scal };


	Math::Vector2 pos = { m_back1Tex2DPos.x - m_back1Tex->GetWidth() / 2 + siz.x / 2,m_back1Tex2DPos.y };


	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spRtTargetPack->m_RTTexture, pos.x, pos.y, siz.x, siz.y);


	//ステータス描画
	Math::Vector2 pivo = { 0,0.5 };
	for (int i = 0; i < PlayerStatusInfoSiz; i++)
	{
		Math::Color color;
		Math::Vector2 backPos = m_playerStatusInfo[i].namePos;
		Math::Vector2 backSiz = { 380,55 };
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
		NumDraw::GetInstance().Drow(m_playerStatusInfo[i].num, RAligned, m_playerStatusInfo[i].numPos, kWhiteColor, 3, true);

		if (m_playerStatusInfo[i].iconTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_playerStatusInfo[i].iconTex, m_playerStatusInfo[i].iconPos.x, m_playerStatusInfo[i].iconPos.y, nullptr, nullptr, pivo);
		}
	}
}

void PlayerInventoryUI::PlayerStatusPreDraw()
{
	if (!m_spRtTargetPack) { return; }
	std::shared_ptr<PlayerBase> spPlayer = m_wpPlayerBase.lock();
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
		for (int i = 0; i < PlayerStatusInfoSiz; i++)
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

			m_playerStatusInfo[i].num = num;
		}

	}


}

void PlayerInventoryUI::PotionInfoInit()
{
	if (m_itemIconInfo.size() != 0)
	{
		m_selectExplanationTex = m_itemIconInfo[0].m_ExplanationTex;
		m_selectNameTex = m_itemIconInfo[0].m_NameTex;
		m_selectPotionID = m_itemIconInfo[0].m_ItemID;
	}
	else
	{
		m_selectPotionID = -999;
		m_selectExplanationTex = m_NotselectExplanationTex;
		m_selectNameTex = m_NotselectNameTex;
	}
}

void PlayerInventoryUI::AddPotionTexInfo()
{
	std::shared_ptr<PlayerBase >spPlayer = m_wpPlayerBase.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
	if (!spPlayerInventory) { return; }

	m_itemIconInfo.clear();

	int i = 0;
	int j = 0;
	Math::Vector2 m_iconBase2DPos = { -460, 80 }; // 初期位置
	float m_iconSpacing = 10;
	for (Inventory potion : spPlayerInventory->GetPotionsInventory())
	{
		if (potion.m_num > 0)
		{
			ItemIconInfo itemIconInfo;
			itemIconInfo.m_ItemID = potion.m_ID;


			if (i > 20)
			{
				j++;
				i = 0;
			}

			float x = m_iconBase2DPos.x + i * m_iconDimensions.w + i * m_iconSpacing;
			float y = m_iconBase2DPos.y - j * m_iconDimensions.h - j * m_iconSpacing;
			itemIconInfo.m_2DPos = { x,y };
			itemIconInfo.m_num = potion.m_num;
			itemIconInfo.m_name = potion.m_name;

			std::shared_ptr<PotionTexInfo>_spPotionTexInfo = m_wpPotionTexInfo.lock();
			if (_spPotionTexInfo)
			{
				itemIconInfo.m_IconTex = _spPotionTexInfo->GetPotionIcon(itemIconInfo.m_ItemID);
				itemIconInfo.m_ExplanationTex = _spPotionTexInfo->GetPotionExplanation(itemIconInfo.m_ItemID);
				itemIconInfo.m_NameTex = _spPotionTexInfo->GetPotionName(itemIconInfo.m_ItemID); // ← 追加
			}

			m_itemIconInfo.push_back(itemIconInfo);
			i++;
		}
		else
		{
			if (potion.m_ID == m_selectPotionID)
			{
				m_selectPotionID = -999;
			}
		}
	}




	bool flg = false;
	for (auto& potion : m_itemIconInfo)
	{
		if (m_selectPotionID == potion.m_ItemID)
		{
			flg = true;
		}
	}

	if (!flg && m_itemIconInfo.size() != 0)
	{
		m_selectExplanationTex = m_itemIconInfo[0].m_ExplanationTex;
		m_selectPotionID = m_itemIconInfo[0].m_ItemID;
		m_selectNameTex = m_itemIconInfo[0].m_NameTex;
	}
}

void PlayerInventoryUI::IconHit()
{
	if (m_itemIconInfo.size() <= 0)
	{
		m_selectExplanationTex = m_NotselectExplanationTex;
		m_selectNameTex = m_NotselectNameTex;
		return;
	}


	POINT mousePos = MouseInfo::Instance().m_windowPos;

	for (auto& potion : m_itemIconInfo)
	{
		float Left = potion.m_2DPos.x - m_iconDimensions.w / 2;
		float Right = potion.m_2DPos.x + m_iconDimensions.w / 2;
		float Top = potion.m_2DPos.y + m_iconDimensions.h / 2;
		float Bot = potion.m_2DPos.y - m_iconDimensions.h / 2;


		if (mousePos.x >= Left && mousePos.x <= Right &&
			mousePos.y >= Bot && mousePos.y <= Top)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			potion.m_hit = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_selectExplanationTex = potion.m_ExplanationTex;
				m_selectPotionID = potion.m_ItemID;
				m_selectNameTex = potion.m_NameTex;
			}
		}
		else
		{
			potion.m_hit = false;
		}
	}


}

void PlayerInventoryUI::PotionIUse()
{


	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (!spPlayer) { return; }

	POINT mousePos = MouseInfo::Instance().m_windowPos;


	float Left = m_UseTex2DPos.x - m_UseTex->GetWidth() / 2;
	float Right = m_UseTex2DPos.x + m_UseTex->GetWidth() / 2;
	float Top = m_UseTex2DPos.y + m_UseTex->GetHeight() / 2;
	float Bot = m_UseTex2DPos.y - m_UseTex->GetHeight() / 2;

	if (mousePos.x >= Left && mousePos.x <= Right &&
		mousePos.y >= Bot && mousePos.y <= Top)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{
			if (m_itemIconInfo.size() <= 0 || m_selectPotionID == -999) { return; }
			std::shared_ptr<PotionUseController >spPotionUseController = m_wpPotionUseController.lock();
			if (!spPotionUseController) { return; }

			spPotionUseController->SetPlayer(spPlayer);
			spPotionUseController->PotionUse(m_selectPotionID);
			spPlayer->GetPlayerInventory()->UsePotionsInventory(m_selectPotionID);

			m_useEffectTimer = m_useEffectTime;
			m_useEffectItemID = m_selectPotionID;
		}
	}


	m_num = spPlayer->GetPlayerInventory()->GetPotionsInventoryNum(m_selectPotionID);
}

void PlayerInventoryUI::WeaponsStrengtheningInfoInit()
{
	// UI と武器の強化 enum は同じ並び（末尾が UI では Icon、武器では Body）であることが前提
	static_assert(static_cast<int>(WeaponsStrengtheningSiz) == static_cast<int>(::WeaponsStrengtheningSiz),
		"PlayerInventoryUI と WeaponBase の強化 enum の数が違います");

	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (!spPlayer) { return; }
	std::shared_ptr<WeaponBase>spWeapon = spPlayer->GetWeapon().lock();
	if (!spWeapon) { return; }
	std::shared_ptr<PotionTexInfo>_spTexInfo = m_wpPotionTexInfo.lock();
	if (!_spTexInfo) { return; }

	// 配列の番号 = 種類。武器側も同じ並びなので、番号どおりにコピーする
	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		WeaponsStrengtheningInfo& ui = m_weaponsStrengtheningInfo[i];
		const ::WeaponsStrengtheningInfo& src = spWeapon->GetWeaponsStrengtheningInfo(i);

		// テクスチャは最初の1回だけ読む
		if (!ui.m_ExplanationTex)
		{
			ui.m_ExplanationTex = _spTexInfo->GetWeaponsStrengtheningExplanation(i);
			ui.m_NameTex = _spTexInfo->GetWeaponsStrengtheningName(i);
		}

		// データは開くたびに取り直す
		ui.Lv = src.Lv;
		ui.nextLvNum = src.nextLvNum;
		ui.strengtheningItem_ID = src.strengtheningItem_ID;
		ui.MyNodeID = src.MyNodeID;
		ui.PreconditionNodeID = src.PreconditionNodeID;
		ui.PreconditionNodeID_index = spWeapon->FindIndexByNodeID(src.PreconditionNodeID); // -1 = 前提なし
	}

	m_selectIndex = 0;
	m_selectExplanationTex = m_weaponsStrengtheningInfo[m_selectIndex].m_ExplanationTex;
	m_selectNameTex = m_weaponsStrengtheningInfo[m_selectIndex].m_NameTex;
	m_num = m_weaponsStrengtheningInfo[m_selectIndex].Lv;
	m_selectWeaponsStrengtheningnextLVNum = m_weaponsStrengtheningInfo[m_selectIndex].nextLvNum;
	m_weaponsStrengtheningInfo[m_selectIndex].m_hit = true;

	WeaponChooseFlg(m_selectIndex);


	WeaponsStrengtheningInfoInitPos();
}

void PlayerInventoryUI::WeaponsStrengtheningInfoInitPos()
{

}

void PlayerInventoryUI::WeaponsStrengtheningTexInfo()
{
	std::shared_ptr<PlayerBase >spPlayer = m_wpPlayerBase.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
	if (!spPlayerInventory) { return; }

	Math::Vector2 m_iconBase2DPos = m_UseTex2DPos; // 初期位置
	m_iconBase2DPos.y += 90;
	for (Inventory weapon : spPlayerInventory->GetWeaponsStrengtheningInventory())
	{
		if (weapon.m_ID == m_weaponsStrengtheningInfo[m_selectIndex].strengtheningItem_ID)
		{
			ItemIconInfo itemIconInfo;
			itemIconInfo.m_ItemID = weapon.m_ID;
			itemIconInfo.m_2DPos = m_iconBase2DPos;
			itemIconInfo.m_num = weapon.m_num;
			itemIconInfo.m_name = weapon.m_name;

			std::shared_ptr<PotionTexInfo>_spTexInfo = m_wpPotionTexInfo.lock();
			if (_spTexInfo)
			{
				int WeaponsStrengtheningItemID = m_weaponsStrengtheningInfo[m_selectIndex].strengtheningItem_ID;
				itemIconInfo.m_IconTex = _spTexInfo->GetWeaponsStrengtheningItemIcon(WeaponsStrengtheningItemID);
				itemIconInfo.m_ExplanationTex = _spTexInfo->GetWeaponsStrengtheningExplanation(itemIconInfo.m_ItemID);
			}

			m_weaponsStrengtheningIconInfo = itemIconInfo;
		}
	}



	std::shared_ptr<WeaponBase> spWeapon = spPlayer->GetWeapon().lock();
	if (spWeapon)
	{
		// 全ノード分、武器側の最新の状態を取り直す
		for (int i = 0; i < WeaponsStrengtheningSiz; i++)
		{
			WeaponsStrengtheningInfo& weapon = m_weaponsStrengtheningInfo[i];
			weapon.LvMaxFlg = spWeapon->GetWeaponsStrengtheningLVMaxFlg(i);
			weapon.Lv = spWeapon->GetWeaponsStrengtheningLv(i);
			weapon.nextLvNum = spWeapon->GetWeaponsStrengtheningNextLvNum(i);
			weapon.strengtheningFlg = spWeapon->IsWeaponsStrengtheningPossible(i);
		}
	}
}


void PlayerInventoryUI::WeaponsStrengtheningIconHit()
{
	POINT mousePos = MouseInfo::Instance().m_windowPos;

	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		WeaponsStrengtheningInfo& weapon = m_weaponsStrengtheningInfo[i];

		float Left = weapon.iconPos.x - m_selectWeaponsStrengtheningIconSiz.x / 2;
		float Right = weapon.iconPos.x + m_selectWeaponsStrengtheningIconSiz.x / 2;
		float Top = weapon.iconPos.y + m_selectWeaponsStrengtheningIconSiz.y / 2;
		float Bot = weapon.iconPos.y - m_selectWeaponsStrengtheningIconSiz.y / 2;


		if (mousePos.x >= Left && mousePos.x <= Right &&
			mousePos.y >= Bot && mousePos.y <= Top)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			weapon.m_hit = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_selectExplanationTex = weapon.m_ExplanationTex;
				m_selectIndex = i;
				m_num = weapon.Lv;
				m_selectWeaponsStrengtheningnextLVNum = weapon.nextLvNum;

				// 前提なし、または前提ノードが解放済みなら通常の名前・説明
				const int preIndex = weapon.PreconditionNodeID_index;
				if (preIndex < 0 || m_weaponsStrengtheningInfo[preIndex].Lv > 0)
				{
					m_selectNameTex = weapon.m_NameTex;
					m_selectExplanationTex = weapon.m_ExplanationTex;
				}
				else
				{
					m_selectNameTex = m_PreconditionNodeNameTex;
					m_selectExplanationTex = m_PreconditionNodeExplanationTex;
				}
				WeaponChooseFlg(i);
			}
		}
		else
		{
			weapon.m_hit = false;
		}
	}
}

void PlayerInventoryUI::WeaponsStrengtheningButtonHit()
{
	if (!m_weaponsStrengtheningButtonTex) { return; }

	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (!spPlayer) { return; }
	std::shared_ptr<WeaponBase>spWeapon = spPlayer->GetWeapon().lock();
	if (!spWeapon) { return; }

	POINT mousePos = MouseInfo::Instance().m_windowPos;

	float Left = m_weaponsStrengtheningButtonTex2DPos.x - m_weaponsStrengtheningButtonTex->GetWidth() / 2;
	float Right = m_weaponsStrengtheningButtonTex2DPos.x + m_weaponsStrengtheningButtonTex->GetWidth() / 2;
	float Top = m_weaponsStrengtheningButtonTex2DPos.y + m_weaponsStrengtheningButtonTex->GetHeight() / 2;
	float Bot = m_weaponsStrengtheningButtonTex2DPos.y - m_weaponsStrengtheningButtonTex->GetHeight() / 2;


	//前提ノード確認（判定は武器側に任せる）
	if (!spWeapon->IsPreconditionCleared(m_selectIndex))
	{
		//解放されていなければ強化できない
		m_PreconditionNodeFlg = true;
		return;
	}

	m_PreconditionNodeFlg = false;

	if (mousePos.x >= Left && mousePos.x <= Right &&
		mousePos.y >= Bot && mousePos.y <= Top)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{

			if (spWeapon->IsWeaponsStrengtheningPossible(m_selectIndex))
			{
				spWeapon->AddWeaponsStrengtheningInfo(m_selectIndex);
				m_num = spWeapon->GetWeaponsStrengtheningLv(m_selectIndex);
				m_weaponsStrengtheningInfo[m_selectIndex].Lv = m_num;


				int WeaponsStrengtheningItemID = m_weaponsStrengtheningInfo[m_selectIndex].strengtheningItem_ID;
				spPlayer->GetPlayerInventory()->UseWeaponsStrengtheningInventory(WeaponsStrengtheningItemID, m_weaponsStrengtheningInfo[m_selectIndex].nextLvNum);
				m_weaponsStrengtheningInfo[m_selectIndex].nextLvNum = spWeapon->GetWeaponsStrengtheningNextLvNum(m_selectIndex);
				m_selectWeaponsStrengtheningnextLVNum = m_weaponsStrengtheningInfo[m_selectIndex].nextLvNum;

				m_useEffectTimer = m_useEffectTime;
				m_useEffectItemID = m_weaponsStrengtheningInfo[m_selectIndex].MyNodeID;
			}
		}
	}

}

void PlayerInventoryUI::WeaponChooseFlg(int _index)
{
	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		m_weaponsStrengtheningInfo[i].ChooseFlg = (i == _index);
	}
}
