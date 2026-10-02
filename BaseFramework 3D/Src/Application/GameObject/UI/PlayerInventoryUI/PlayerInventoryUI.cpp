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


	if (!m_notSelsect)
	{
		m_notSelsect = std::make_shared<KdTexture>();
		m_notSelsect->Load("Asset/Textures/GameUI/Item/PlayerInventory/BagBackTex2.png");
		m_back2Tex = m_notSelsect;
		m_back2Tex2DPos = m_back1Tex2DPos + Math::Vector2{ (float)m_back1Tex->GetWidth() / 2,0 } + Math::Vector2{ (float)m_back2Tex->GetWidth(),0 };
		m_back2Tex2DSiz = { (float)m_back2Tex->GetWidth() , (float)m_back2Tex->GetHeight() };
	}

	if (!m_UseTex)
	{
		m_UseTex = std::make_shared<KdTexture>();
		m_UseTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/UseTex.png");

		float x = m_back2Tex2DPos.x;
		float y = m_back2Tex2DPos.y - m_back2Tex->GetHeight() / 2 + m_UseTex->GetHeight();
		m_UseTex2DPos = { x,y };
	}

	m_iconDimensions = { 40,40 };
	m_layerPriority = 1;



	Math::Vector2 basePos = { -420, 225 };
	for (int i = 0;i < InventoryTypeSiz;i++)
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

	for (int i = 0;i < PlayerStatusInfoSiz;i++)
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

	////////////////////////////////////////////////////////////////////////////
	//武器強化用
	Math::Vector2 weponStrengtheningBasePos = {-350,-70};
	float weponStrengtheningOffset = 100.0f;
	float vecL = 200.0f;

	float rad = DirectX::XMConvertToRadians(90.0f); // 0°,90°,180°,270°

	m_selectWeponStrengtheningIconSiz = { 64,64 }; 

	for (int i = 0; i < WeponStrengtheningSiz; i++)
	{
		Math::Vector2 pos = weponStrengtheningBasePos;

		switch (i)
		{
		case PlayerInventoryUI::WeponStrengthening_Attck:
			pos.x -= weponStrengtheningOffset;
			break;
		case PlayerInventoryUI::WeponStrengthening_Stamina:
			pos.y += weponStrengtheningOffset;
			break;
		case PlayerInventoryUI::WeponStrengthening_ChargeTime:
			pos.y -= weponStrengtheningOffset;
			break;
		case PlayerInventoryUI::WeaponStrengthening_ShockWave:
			pos.x += weponStrengtheningOffset;
			break;
		case PlayerInventoryUI::WeaponStrengthening_ShockWave_maxDistanceM:
			rad = DirectX::XMConvertToRadians(45.0f);
			pos.x = m_weponStrengtheningInfo[WeaponStrengthening_ShockWave].iconPos.x + cos(rad) * vecL;
			pos.y = m_weponStrengtheningInfo[WeaponStrengthening_ShockWave].iconPos.y + sin(rad) * vecL;
			break;
		case PlayerInventoryUI::WeaponStrengthening_ShockWave_speed:
			rad = DirectX::XMConvertToRadians(0.0f);
			pos.x = m_weponStrengtheningInfo[WeaponStrengthening_ShockWave].iconPos.x + cos(rad) * vecL;
			pos.y = m_weponStrengtheningInfo[WeaponStrengthening_ShockWave].iconPos.y + sin(rad) * vecL;
			break;
		case PlayerInventoryUI::WeaponStrengthening_ShockWave_hitNum:
			rad = DirectX::XMConvertToRadians(-45.0f);
			pos.x = m_weponStrengtheningInfo[WeaponStrengthening_ShockWave].iconPos.x + cos(rad) * vecL;
			pos.y = m_weponStrengtheningInfo[WeaponStrengthening_ShockWave].iconPos.y + sin(rad) * vecL;
			break;
		case PlayerInventoryUI::WeponStrengtheningSiz:
			break;
		default:
			break;
		}

		m_weponStrengtheningInfo[i].Lv = 0;
		m_weponStrengtheningInfo[i].iconPos = pos;
		m_weponStrengtheningInfo[i].iconSiz = m_selectWeponStrengtheningIconSiz;
		if (!m_weponStrengtheningInfo[i].iconTex)
		{
			m_weponStrengtheningInfo[i].iconTex = std::make_shared<KdTexture>();
			std::string filePath = "Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/WeponStrengtheningIcon";
			filePath += std::to_string(i) + ".png";
			m_weponStrengtheningInfo[i].iconTex->Load(filePath);
		}
		if (!m_weponStrengtheningInfo[i].m_ExplanationTex)
		{
			m_weponStrengtheningInfo[i].m_ExplanationTex = std::make_shared<KdTexture>();
			std::string filePath = "Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/WeponStrengtheningIcon";
			filePath += std::to_string(i) + ".png";
			m_weponStrengtheningInfo[i].m_ExplanationTex->Load(filePath);
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
			WeponUpdate();
			break;
		case PlayerInventoryUI::PlayerStatus:
			PlayerStatusUpdate();
			break;
		default:
			break;
		}


	}


	//debaggu
	if (GetAsyncKeyState('1') & 0x8000)
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
		}
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



	if (m_back2Tex && m_nowInventoryType != PlayerInventoryUI::PlayerStatus)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_back2Tex, m_back2Tex2DPos.x, m_back2Tex2DPos.y, m_back2Tex2DSiz.x, m_back2Tex2DSiz.y);

		Math::Vector2 pos = m_back2Tex2DPos;
		pos.y += 150;
		pos.x += 80;
		NumDraw::GetInstance().Drow(m_num, RAligned, pos, kWhiteColor, 4);
	}



	//////////////////////////////////////////////////


	for (int i = 0;i < InventoryTypeSiz;i++)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_inventoryTypeChange[i].m_IconTex, m_inventoryTypeChange[i].m_2DPos.x, m_inventoryTypeChange[i].m_2DPos.y);
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
		WeponDraw();

		if (m_UseTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_UseTex, m_UseTex2DPos.x, m_UseTex2DPos.y);
		}
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
			AddPotionTexInfo();
			if (m_itemIconInfo.size() > 0)
			{
				m_back2Tex = m_itemIconInfo[0].m_ExplanationTex;
				m_selectPotionID = m_itemIconInfo[0].m_ItemID;
			}
			else
			{
				m_back2Tex = m_notSelsect;
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
			Icon.m_hit = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_nowInventoryType = Icon.m_ID;
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

void PlayerInventoryUI::WeponUpdate()
{
	WeponStrengtheningIconHit();
}

void PlayerInventoryUI::PlayerStatusUpdate()
{
}

void PlayerInventoryUI::PotionDraw()
{
	for (auto tex : m_itemIconInfo)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_IconTex, tex.m_2DPos.x, tex.m_2DPos.y, m_iconDimensions.w, m_iconDimensions.h);
	}
}

void PlayerInventoryUI::WeponDraw()
{
	for (int i = 0; i < WeponStrengtheningSiz; i++)
	{
		if (!m_weponStrengtheningInfo[i].iconTex) { continue; }
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_weponStrengtheningInfo[i].iconTex, m_weponStrengtheningInfo[i].iconPos.x, m_weponStrengtheningInfo[i].iconPos.y, m_weponStrengtheningInfo[i].iconSiz.x, m_weponStrengtheningInfo[i].iconSiz.y);
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
	for (int i = 0;i < PlayerStatusInfoSiz;i++)
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
	SceneManager::Instance().ChangeRendertarget(m_spRtTargetPack);
	std::shared_ptr<PlayerBase> spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		// 3Dシェーダーのパスを開始
		KdShaderManager::Instance().m_StandardShader.BeginLit();
		// ステータスに描画するプレイヤーの描画
		spPlayer->DrawLit();
		// 3Dシェーダーのパスを終了
		KdShaderManager::Instance().m_StandardShader.EndLit();
	}
	SceneManager::Instance().UndoRenderTarget();;


	if (spPlayer)
	{
		for (int i = 0;i < PlayerStatusInfoSiz;i++)
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

void PlayerInventoryUI::AddPotionTexInfo()
{
	std::shared_ptr<PlayerBase >spPlayer = m_wpPlayerBase.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
	if (!spPlayerInventory) { return; }

	m_itemIconInfo.clear();

	int i = 0;
	Math::Vector2 m_iconBase2DPos = { -420, 90 }; // 初期位置
	float m_iconSpacing = 5;
	for (Inventory potion : spPlayerInventory->GetPotionsInventory())
	{
		if (potion.m_num > 0)
		{
			ItemIconInfo itemIconInfo;
			itemIconInfo.m_ItemID = potion.m_ID;
			itemIconInfo.m_2DPos = { m_iconBase2DPos.x + i * m_iconDimensions.w + i * m_iconSpacing, m_iconBase2DPos.y };
			itemIconInfo.m_num = potion.m_num;
			itemIconInfo.m_name = potion.m_name;

			std::shared_ptr<PotionTexInfo>_spPotionTexInfo = m_wpPotionTexInfo.lock();
			if (_spPotionTexInfo)
			{
				itemIconInfo.m_IconTex = _spPotionTexInfo->GetIcon(itemIconInfo.m_ItemID);
				itemIconInfo.m_ExplanationTex = _spPotionTexInfo->GetExplanation(itemIconInfo.m_ItemID);
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
}

void PlayerInventoryUI::IconHit()
{
	if (m_itemIconInfo.size() <= 0)
	{
		m_back2Tex = m_notSelsect;
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
			potion.m_hit = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_back2Tex = potion.m_ExplanationTex;
				m_selectPotionID = potion.m_ItemID;
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
	if (m_itemIconInfo.size() <= 0 || m_selectPotionID == -999) { return; }
	std::shared_ptr<PotionUseController >spPotionUseController = m_wpPotionUseController.lock();
	if (!spPotionUseController) { return; }

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
		if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{

			spPotionUseController->SetPlayer(spPlayer);
			spPotionUseController->PotionUse(m_selectPotionID);
			spPlayer->GetPlayerInventory()->UsePotionsInventory(m_selectPotionID);


		}
	}


	m_num = spPlayer->GetPlayerInventory()->GetPotionsInventoryNum(m_selectPotionID);
}

void PlayerInventoryUI::WeponStrengtheningIconHit()
{
	POINT mousePos = MouseInfo::Instance().m_windowPos;

	for (auto& wepon : m_weponStrengtheningInfo)
	{
		float Left = wepon.iconPos.x- m_selectWeponStrengtheningIconSiz.x / 2;
		float Right = wepon.iconPos.x + m_selectWeponStrengtheningIconSiz.x / 2;
		float Top = wepon.iconPos.y + m_selectWeponStrengtheningIconSiz.y / 2;
		float Bot = wepon.iconPos.y - m_selectWeponStrengtheningIconSiz.y / 2;


		if (mousePos.x >= Left && mousePos.x <= Right &&
			mousePos.y >= Bot && mousePos.y <= Top)
		{
			wepon.m_hit = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_back2Tex = wepon.m_ExplanationTex;
				m_selectWeponStrengtheningID = wepon.ID;
			}
		}
		else
		{
			wepon.m_hit = false;
		}
	}
}
