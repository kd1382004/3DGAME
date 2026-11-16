#include "PlayerInventoryUI.h"
#include"../../../Scene/SceneManager.h"



#include"../../../Info/KeyInfo/KeyInfo.h"
#include"../../../Info/VOL/VOLInfo.h"
#include"../../../Scene/GameScene/GameScene.h"
#include"../../../Info/MouseInfo/MouseInfo.h"

#include"../../Potions/PotionsType.h"
#include"../../Potions/PotionUseController.h"


#include"../../Character/Player/PlayerBase.h"
#include"../../Character/Player/PlayerInventory/PlayerInventory.h"

#include"../../Potions/PotionTexInfo/PotionTexInfo.h"

#include"../../../Info/NumDraw/NumDraw.h"

#include"../../Weapon/WeaponBase.h"

#include"PotionInventory/PotionInventoryUI.h"
#include"WeponInventory/WeponInventoryUI.h"
#include"PlayerStatus/PlayerStatusInventoryUI.h"

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


	if (!m_spContext->notSelectNameTex)
	{
		m_spContext->notSelectNameTex = std::make_shared<KdTexture>();
		m_spContext->notSelectNameTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/SelectNameBase.png");
		m_spContext->selectNameTex = m_spContext->notSelectNameTex;
		m_selectNameTexPos = m_back2Tex2DPos;
		m_selectNameTexPos.y = 223;
	}


	if (!m_spContext->selectExplanationTex)
	{
		m_spContext->notSelectExplanationTex = std::make_shared<KdTexture>();
		m_spContext->notSelectExplanationTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/SelectExplanationBase.png");
		m_spContext->selectExplanationTex = m_spContext->notSelectExplanationTex;
		m_selectExplanationTexPos = m_back2Tex2DPos;
		m_selectExplanationTexPos.y = 50;
	}

	if (!m_spContext->useTex)
	{
		m_spContext->useTex = std::make_shared<KdTexture>();
		m_spContext->useTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/UseTex.png");

		float x = m_back2Tex2DPos.x;
		float y = m_back2Tex2DPos.y - m_back2Tex->GetHeight() / 2 + m_spContext->useTex->GetHeight() / 2 + 20;
		m_spContext->useTex2DPos = { x,y };
	}

	if (!m_spContext->potionSelectTex)
	{
		m_spContext->potionSelectTex = std::make_shared<KdTexture>();
		m_spContext->potionSelectTex->Load("Asset/Textures/GameUI/Item/Potion/PotionSelect.png");
	}

	m_layerPriority = 1;

	// 各タブで使う位置
	m_spContext->back1Tex2DPos = m_back1Tex2DPos;
	m_spContext->back1TexWidth = (float)m_back1Tex->GetWidth();
	m_spContext->back2Tex2DPos = m_back2Tex2DPos;


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
	//各タブ
	if (!m_spPotionInventoryUI)
	{
		m_spPotionInventoryUI = std::make_shared<PotionInventoryUI>();
		m_spPotionInventoryUI->SetContext(m_spContext);
	}
	m_spPotionInventoryUI->Init();

	if (!m_spPlayerStatusInventoryUI)
	{
		m_spPlayerStatusInventoryUI = std::make_shared<PlayerStatusInventoryUI>();
		m_spPlayerStatusInventoryUI->SetContext(m_spContext);
	}
	m_spPlayerStatusInventoryUI->Init();

	if (!m_spWeponInventoryUI)
	{
		m_spWeponInventoryUI = std::make_shared<WeponInventoryUI>();
		m_spWeponInventoryUI->SetContext(m_spContext);
	}
	m_spWeponInventoryUI->Init();
}

void PlayerInventoryUI::PreUpdate()
{

}

void PlayerInventoryUI::Update()
{
	PlayerInventoryOpen();

	if (m_spContext->useEffectTimer > 0.0f)
	{
		m_spContext->useEffectTimer -= DeltaTime::Instance().GetRealDeltaTime();
		if (m_spContext->useEffectTimer < 0.0f) { m_spContext->useEffectTimer = 0.0f; }
	}


	if (m_playerInventoryUIFlg)
	{
		InventoryTypeChangeUpdate();

		InventoryTabBase* tab = GetTab(m_nowInventoryType);
		if (tab)
		{
			tab->Update();
		}
	}


	//デバック
	static bool flg = true;
	if (GetAsyncKeyState('1') & 0x8000)
	{
		//if (!flg)
		{
			std::shared_ptr<PlayerBase>spPlayer = m_spContext->wpPlayer.lock();
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
	InventoryTabBase* tab = GetTab(m_nowInventoryType);
	if (tab)
	{
		tab->PreDraw();
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
		pos.y += 147;	// 右の枠の上の暗い帯（数字の枠）の真ん中
		pos.x += 85;

		// 数字が変わるときはちょっと跳ねる(色も変わる)
		Math::Color numColor = kWhiteColor;
		float numScale = 2.5f;	// 数字の枠（高さ50）に収まる大きさ（14 × 2.5 = 35）
		if (m_spContext->useEffectTimer > 0.0f && (m_nowInventoryType == PotionInventory || m_nowInventoryType == WeponInventory))
		{
			
			float t = m_spContext->UseEffectT();
			switch (m_spContext->useEffectType)
			{
			// 上に跳ねる
			case UseEffectType::Bounce:
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

				break;

				// 横に震える
			case UseEffectType::Shake:
				// 0 → 1 とまっすぐ進む値（演出の進み具合）
				float p = 1.0f - m_spContext->useEffectTimer / m_spContext->useEffectTime;

				// だんだん弱くなる揺れ幅（最初が一番大きい）
				float amp = 6.0f * (1.0f - p);

				// 左右に3往復
				pos.x += sinf(p * DirectX::XM_PI * 6.0f) * amp;
				numColor = { 1.0f, 1.0f - 0.6f * (1.0f - p), 1.0f - 0.6f * (1.0f - p), 1.0f };   // 赤→白に戻る
				break;
			}
		}
		NumDraw::GetInstance().Drow(m_spContext->num, RAligned, pos, numColor, numScale);

		if (m_spContext->selectExplanationTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spContext->selectExplanationTex, m_selectExplanationTexPos.x, m_selectExplanationTexPos.y);
		}


		if (m_spContext->selectNameTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spContext->selectNameTex, m_selectNameTexPos.x, m_selectNameTexPos.y);
		}
	}



	//////////////////////////////////////////////////


	for (int i = 0; i < InventoryTypeSiz; i++)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_inventoryTypeChange[i].m_IconTex, m_inventoryTypeChange[i].m_2DPos.x, m_inventoryTypeChange[i].m_2DPos.y);


		if (i == m_nowInventoryType)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spContext->potionSelectTex, m_inventoryTypeChange[i].m_2DPos.x, m_inventoryTypeChange[i].m_2DPos.y);

		}
	}

	//////////////////////////////////////////////////

	InventoryTabBase* tab = GetTab(m_nowInventoryType);
	if (tab)
	{
		tab->DrawSprite();
	}
}

void PlayerInventoryUI::PlayerInventoryOpen()
{

	std::shared_ptr<PlayerBase>spPlayer = m_spContext->wpPlayer.lock();
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

		// 開ける音・閉じる音（かばん）
		if (m_playerInventoryUIFlg)
		{
			{
				std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/BagOpen.wav", false);
				if (se) { se->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
			}
		}
		else
		{
			{
				std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/BagClose.wav", false);
				if (se) { se->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
			}
		}

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

			// アイテムがなければ「選択なし」の表示にする
			if (!m_spPotionInventoryUI->OnInventoryOpen())
			{
				m_Changeback2Tex = m_notSelsect;
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
				// 違うタブに切り替えたときだけ、ページをめくる音
				if (m_nowInventoryType != Icon.m_ID)
				{
					{
						std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/PageTurn.wav", false);
						if (se) { se->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
					}
				}

				m_nowInventoryType = Icon.m_ID;

				// ステータスは選んだときの処理なし（以前と同じ）
				switch (m_nowInventoryType)
				{
				case PlayerInventoryUI::PotionInventory:
					m_spPotionInventoryUI->OnSelect();
					break;
				case PlayerInventoryUI::WeponInventory:
					m_spWeponInventoryUI->OnSelect();
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

InventoryTabBase* PlayerInventoryUI::GetTab(InventoryType _type)
{
	switch (_type)
	{
	case PlayerInventoryUI::PotionInventory:	return m_spPotionInventoryUI.get();
	case PlayerInventoryUI::WeponInventory:		return m_spWeponInventoryUI.get();
	case PlayerInventoryUI::PlayerStatus:		return m_spPlayerStatusInventoryUI.get();
	default:									return nullptr;
	}
}
