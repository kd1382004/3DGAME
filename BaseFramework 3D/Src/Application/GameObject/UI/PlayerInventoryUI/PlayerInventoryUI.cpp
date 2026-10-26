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
		m_selectExplanationTexPos.y = 20;
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

	if (!m_spStatusCamera)
	{
		m_spStatusCamera = std::make_shared<KdCamera>();
		m_spStatusCamera->SetProjectionMatrix(45.0f, 2000.0f, 0.01f, 1280.0f / 720.0f);
	}

	////////////////////////////////////////////////////////////////////////////
	//武器強化用
	Math::Vector2 weaponsStrengtheningBasePos = { m_back1Tex2DPos.x,-150 };
	float weaponsStrengtheningOffset = 150.0f;
	float vecL = 200.0f;



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
	if (!m_selectTex)
	{
		m_selectTex2DPos = m_UseTex2DPos;
		m_selectTex = std::make_shared<KdTexture>();
		m_selectTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/select.png");
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

	float rad = DirectX::XMConvertToRadians(90.0f); // 0°,90°,180°,270°

	m_selectWeaponsStrengtheningIconSiz = { 64,64 };

	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		Math::Vector2 pos = weaponsStrengtheningBasePos;

		switch (i)
		{
		case PlayerInventoryUI::WeaponsStrengthening_Attck:
			pos.x -= weaponsStrengtheningOffset * 2;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_Stamina:
			pos.x -= weaponsStrengtheningOffset;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_ChargeTime:
			pos.x += weaponsStrengtheningOffset * 2;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_ShockWave:
			pos.x = m_back1Tex2DPos.x;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_DefenseIgnore:
			pos.x += weaponsStrengtheningOffset;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_ShockWave_maxDistanceM:
			rad = DirectX::XMConvertToRadians(45.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].iconPos.y + cos(rad) * vecL;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_ShockWave_speed:
			rad = DirectX::XMConvertToRadians(0.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].iconPos.y + cos(rad) * vecL;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_ShockWave_hitNum:
			rad = DirectX::XMConvertToRadians(-45.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].iconPos.y + cos(rad) * vecL;
			break;
		case PlayerInventoryUI::WeaponsStrengtheningSiz:
			break;
		default:
			break;
		}

		m_weaponsStrengtheningInfo[i].Lv = 0;
		m_weaponsStrengtheningInfo[i].ID = i;
		m_weaponsStrengtheningInfo[i].iconPos = pos;
		m_weaponsStrengtheningInfo[i].iconSiz = m_selectWeaponsStrengtheningIconSiz;
		m_weaponsStrengtheningInfo[i].nextLvNum = 0;
		if (!m_weaponsStrengtheningInfo[i].iconTex)
		{
			m_weaponsStrengtheningInfo[i].iconTex = std::make_shared<KdTexture>();
			std::string filePath = "Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/WeponStrengtheningIcon";
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
		if (!flg)
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
		NumDraw::GetInstance().Drow(m_num, RAligned, pos, kWhiteColor, 4);

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
			int num = spPlayer->GetPlayerInventory()->GetWeaponsStrengtheningInventoryNum(i);
			spWeapon->SetWeaponsStrengtheningNum(i, num);
		}

	}

	WeaponsStrengtheningIconHit();

	WeaponsStrengtheningTexInfo();

	WeaponsStrengtheningButtonHit();




	//serrekuto
	m_selectTex2DPos = m_weaponsStrengtheningInfo[m_selectWeaponsStrengtheningID].iconPos;
	m_selectTex2DPos.y += m_weaponsStrengtheningInfo[m_selectWeaponsStrengtheningID].iconSiz.y / 2;

	m_selectTex2DPos.y += m_selectAdd;
	m_selectAddTotal += abs(m_selectAdd);

	if (m_selectAdd > 0)
	{
		m_selectAdd += m_selectAddNum;
	}
	else
	{
		m_selectAdd -= m_selectAddNum;
	}

	if (m_selectAddTotal >= m_selectAddTotalMax)
	{
		m_selectAddTotal = 0;
		if (m_selectAdd > 0)
		{
			m_selectAdd = -m_selectDefoAdd;
		}
		else
		{
			m_selectAdd = m_selectDefoAdd;
		}
	}


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
	for (auto tex : m_itemIconInfo)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_IconTex, tex.m_2DPos.x, tex.m_2DPos.y, m_iconDimensions.w, m_iconDimensions.h);

		if (tex.m_ItemID == m_selectPotionID)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_potionSelectTex, tex.m_2DPos.x, tex.m_2DPos.y, m_iconDimensions.w, m_iconDimensions.h);
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
		switch (i)
		{
		case PlayerInventoryUI::WeaponsStrengthening_Attck:
			break;
		case PlayerInventoryUI::WeaponsStrengthening_Stamina:
			break;
		case PlayerInventoryUI::WeaponsStrengthening_ChargeTime:
			break;
		case PlayerInventoryUI::WeaponsStrengthening_ShockWave:
			break;
		case PlayerInventoryUI::WeaponsStrengthening_ShockWave_maxDistanceM:
			x1 = m_weaponsStrengtheningInfo[i].iconPos.x;
			y1 = m_weaponsStrengtheningInfo[i].iconPos.y;

			x2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].iconPos.x;
			y2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].iconPos.y;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_ShockWave_speed:
			x1 = m_weaponsStrengtheningInfo[i].iconPos.x;
			y1 = m_weaponsStrengtheningInfo[i].iconPos.y;

			x2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].iconPos.x;
			y2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].iconPos.y;
			break;
		case PlayerInventoryUI::WeaponsStrengthening_ShockWave_hitNum:
			x1 = m_weaponsStrengtheningInfo[i].iconPos.x;
			y1 = m_weaponsStrengtheningInfo[i].iconPos.y;

			x2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].iconPos.x;
			y2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].iconPos.y;
			break;
		case PlayerInventoryUI::WeaponsStrengtheningSiz:
			break;
		default:
			break;
		}

		KdShaderManager::Instance().m_spriteShader.DrawLine(x1, y1, x2, y2, &kWhiteColor);
	}

	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		if (!m_weaponsStrengtheningInfo[i].iconTex) { continue; }

		Math::Color iconCol = { 1,1,1,1 };

		switch (i)
		{
		case PlayerInventoryUI::WeaponsStrengthening_ShockWave_maxDistanceM:
		case PlayerInventoryUI::WeaponsStrengthening_ShockWave_speed:
		case PlayerInventoryUI::WeaponsStrengthening_ShockWave_hitNum:

			if (m_weaponsStrengtheningInfo[WeponStrengtheningID::WeaponsStrengthening_ShockWave].Lv == 0)
			{
				m_weaponsStrengtheningInfo[i].strengtheningFlg = false;
				iconCol = { 0.5,0.5,0.5,1 };
			}
			break;
		}

		if (m_weaponsStrengtheningInfo[i].Lv == 0)
		{
			iconCol = { 0.5,0.5,0.5,1 };
		}



		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
		// 強化が可能なら光らす


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
		if (m_weaponsStrengtheningInfo[i].strengtheningFlg)
		{
			// 発光オーラ描画
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_weaponsStrengtheningInfo[i].iconTex,
				m_weaponsStrengtheningInfo[i].iconPos.x,
				m_weaponsStrengtheningInfo[i].iconPos.y,
				m_weaponsStrengtheningInfo[i].iconSiz.x * scale,
				m_weaponsStrengtheningInfo[i].iconSiz.y * scale,
				nullptr,
				&auraColor
			);
		}


		if (m_weaponsStrengtheningInfo[i].timer > 1)
		{
			m_weaponsStrengtheningInfo[i].timer -= 1;
		}
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);

		// 通常のアイコン描画
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_weaponsStrengtheningInfo[i].iconTex, m_weaponsStrengtheningInfo[i].iconPos.x, m_weaponsStrengtheningInfo[i].iconPos.y, m_weaponsStrengtheningInfo[i].iconSiz.x, m_weaponsStrengtheningInfo[i].iconSiz.y, nullptr, &iconCol);

	}


	if (m_weaponsStrengtheningIconInfo.m_IconTex)
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

	//LV
	if (m_LVTex)
	{
		Math::Vector2 pivo = { 0,0.5 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_LVTex, m_LVTex2DPos.x, m_LVTex2DPos.y, nullptr, nullptr, pivo);
	}
	if (m_selectTex)
	{
		Math::Vector2 pivo = { 0.5,0 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_selectTex, m_selectTex2DPos.x, m_selectTex2DPos.y, nullptr, nullptr, pivo);
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

		KdShaderManager::Instance().WorkAmbientController().SetFogEnable(false, false);
		KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 0,0,0,0 });

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
		for (int i = 0; i < PlayerStatusInfoSiz; i++)
		{
			int num = 0;

			switch (i)
			{

			case PlayerStatusInfo_HP:
				num = spPlayer->GetMaxHP();
				break;
			case PlayerStatusInfo_MP:
				num = spPlayer->GetMaxMP();
				break;
			case PlayerStatusInfo_AttackPower:
				num = spPlayer->NowAttack();
				break;
			case PlayerStatusInfo_DefensePower:
				num = spPlayer->NowDefense();
				break;
			case PlayerStatusInfo_Speed:
				num = spPlayer->GetWalkSpeed();
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
	Math::Vector2 m_iconBase2DPos = { -475, 90 }; // 初期位置
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


		}
	}


	m_num = spPlayer->GetPlayerInventory()->GetPotionsInventoryNum(m_selectPotionID);
}

void PlayerInventoryUI::WeaponsStrengtheningInfoInit()
{
	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		if (!m_weaponsStrengtheningInfo[i].m_ExplanationTex)
		{
			std::shared_ptr<PotionTexInfo>_spTexInfo = m_wpPotionTexInfo.lock();
			if (!_spTexInfo) { continue; }
			m_weaponsStrengtheningInfo[i].m_ExplanationTex = _spTexInfo->GetWeaponsStrengtheningExplanation(i);
			m_weaponsStrengtheningInfo[i].m_NameTex = _spTexInfo->GetWeaponsStrengtheningName(i);

			std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
			if (!spPlayer) { return; }
			std::shared_ptr<WeaponBase>spWeapon = spPlayer->GetWeapon().lock();
			if (!spWeapon) { return; }
			m_weaponsStrengtheningInfo[i].nextLvNum = spWeapon->GetWeaponsStrengtheningNextLvNum(i);
		}
	}
	m_selectExplanationTex = m_weaponsStrengtheningInfo[0].m_ExplanationTex;
	m_selectNameTex = m_weaponsStrengtheningInfo[0].m_NameTex;
	m_num = m_weaponsStrengtheningInfo[0].Lv;
	m_selectWeaponsStrengtheningnextLVNum = m_weaponsStrengtheningInfo[0].nextLvNum;
	m_selectWeaponsStrengtheningID = 0;
	m_weaponsStrengtheningInfo[0].m_hit = true;
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
		if (weapon.m_ID == m_selectWeaponsStrengtheningID)
		{
			ItemIconInfo itemIconInfo;
			itemIconInfo.m_ItemID = weapon.m_ID;
			itemIconInfo.m_2DPos = m_iconBase2DPos;
			itemIconInfo.m_num = weapon.m_num;
			itemIconInfo.m_name = weapon.m_name;

			std::shared_ptr<PotionTexInfo>_spTexInfo = m_wpPotionTexInfo.lock();
			if (_spTexInfo)
			{
				itemIconInfo.m_IconTex = _spTexInfo->GetWeaponsStrengtheningIcon(itemIconInfo.m_ItemID);
				itemIconInfo.m_ExplanationTex = _spTexInfo->GetWeaponsStrengtheningExplanation(itemIconInfo.m_ItemID);
			}

			m_weaponsStrengtheningIconInfo = itemIconInfo;

		}
	}



	std::shared_ptr<WeaponBase> spWeapon = spPlayer->GetWeapon().lock();
	if (spWeapon)
	{
		for (auto& weapon : m_weaponsStrengtheningInfo)
		{
			weapon.Lv = spWeapon->GetWeaponsStrengtheningLv(weapon.ID);
			weapon.nextLvNum = spWeapon->GetWeaponsStrengtheningNextLvNum(weapon.ID);
			weapon.strengtheningFlg = spWeapon->IsWeaponsStrengtheningPossible(weapon.ID);
		}
	}
}


void PlayerInventoryUI::WeaponsStrengtheningIconHit()
{
	POINT mousePos = MouseInfo::Instance().m_windowPos;

	for (auto& weapon : m_weaponsStrengtheningInfo)
	{
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
				m_selectWeaponsStrengtheningID = weapon.ID;
				m_num = weapon.Lv;
				m_selectWeaponsStrengtheningnextLVNum = weapon.nextLvNum;
				m_selectNameTex = weapon.m_NameTex;
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


	//前提ノード確認
	switch (m_selectWeaponsStrengtheningID)
	{
	case PlayerInventoryUI::WeaponsStrengthening_ShockWave_maxDistanceM:
	case PlayerInventoryUI::WeaponsStrengthening_ShockWave_speed:
	case PlayerInventoryUI::WeaponsStrengthening_ShockWave_hitNum:

		if (m_weaponsStrengtheningInfo[WeponStrengtheningID::WeaponsStrengthening_ShockWave].Lv == 0)
		{
			//解放されていなければ強化できない
			m_PreconditionNodeFlg = true;
			return;
		}
		break;
	}
	m_PreconditionNodeFlg = false;

	if (mousePos.x >= Left && mousePos.x <= Right &&
		mousePos.y >= Bot && mousePos.y <= Top)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{

			if (spWeapon->IsWeaponsStrengtheningPossible(m_selectWeaponsStrengtheningID))
			{
				spWeapon->AddWeaponsStrengtheningInfo(m_selectWeaponsStrengtheningID);
				m_num = spWeapon->GetWeaponsStrengtheningLv(m_selectWeaponsStrengtheningID);
				m_weaponsStrengtheningInfo[m_selectWeaponsStrengtheningID].Lv = m_num;
				spPlayer->GetPlayerInventory()->UseWeaponsStrengtheningInventory(m_selectWeaponsStrengtheningID, m_weaponsStrengtheningInfo[m_selectWeaponsStrengtheningID].nextLvNum);
				m_weaponsStrengtheningInfo[m_selectWeaponsStrengtheningID].nextLvNum = spWeapon->GetWeaponsStrengtheningNextLvNum(m_selectWeaponsStrengtheningID);
				m_selectWeaponsStrengtheningnextLVNum = m_weaponsStrengtheningInfo[m_selectWeaponsStrengtheningID].nextLvNum;
			}
		}
	}

}

