//---- 自分のヘッダー ----
#include "PlayerInventoryUI.h"											//インベントリ画面

//---- シーン ----
#include"../../../Scene/SceneManager.h"									//シーンの管理
#include"../../../Scene/GameScene/GameScene.h"							//ゲームシーン

//---- プレイヤー ----
#include"../../Character/Player/PlayerBase.h"							//プレイヤー
#include"../../Character/Player/PlayerInventory/PlayerInventory.h"		//プレイヤーの所持品

//---- 武器 ----
#include"../../Weapon/WeaponBase.h"										//武器の基底クラス

//---- ポーション ----
#include"../../Potions/PotionsType.h"									//ポーションの種類
#include"../../Potions/PotionUseController.h"							//ポーションの使用処理
#include"../../Potions/PotionTexInfo/PotionTexInfo.h"					//アイテムの名前・画像情報

//---- UI ----
#include"PotionInventory/PotionInventoryUI.h"							//インベントリ画面(ポーションタブ)
#include"WeponInventory/WeponInventoryUI.h"								//インベントリ画面(武器強化タブ)
#include"PlayerStatus/PlayerStatusInventoryUI.h"						//インベントリ画面(ステータスタブ)

//---- 便利機能 ----
#include"../../../Info/KeyInfo/KeyInfo.h"								//キー入力
#include"../../../Info/VOL/VOLInfo.h"									//音量設定
#include"../../../Info/MouseInfo/MouseInfo.h"							//マウス入力
#include"../../../Info/NumDraw/NumDraw.h"								//数字の画像描画

namespace
{
	// 所持金の表示位置(画面中央が0,0・上がプラス。種類アイコンと同じ高さ)
	const Math::Vector2 kCoinPos     = { 138.0f, 225.0f };	// 金貨のアイコンの中心(左端122)
	const Math::Vector2 kMoneyNumPos = { 246.0f, 225.0f };	// 数字の右端
	const Math::Vector2 kGoldPos     = { 268.0f, 227.0f };	// 「G」の中心
	constexpr float kMoneyNumSize = 2.0f;					// 数字の大きさ(14 × 2 = 28)

	// 数字が超えてはいけない大きさ(金貨の右端158 〜 数字の右端246 の間に収める)
	const Math::Vector2 kMoneyNumMaxSize = { 86.0f, 28.0f };
}

//=============================================
// 基本の処理
//=============================================

void PlayerInventoryUI::Init()
{
	KeyInfo::Instance().SetKeyValid(VK_TAB);

	// 所持金の画像
	if (!m_spCoinTex)
	{
		m_spCoinTex = std::make_shared<KdTexture>();
		m_spCoinTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/Money/Coin.png");
	}
	if (!m_spGoldTex)
	{
		m_spGoldTex = std::make_shared<KdTexture>();
		m_spGoldTex->Load("Asset/Textures/GameUI/Prepare/Menu/Text/Gold.png");
	}

	m_playerInventoryUIFlg = false;

	if (!m_spBack1Tex)
	{
		m_spBack1Tex = std::make_shared<KdTexture>();
		m_spBack1Tex->Load("Asset/Textures/GameUI/Item/PlayerInventory/BagBackTex1.png");
		m_back1Tex2DPos = { -100,0 };

	}


	if (!m_spInventoryTypeTex)
	{
		m_spInventoryTypeTex = std::make_shared<KdTexture>();
		m_spInventoryTypeTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/inventoryType/inventoryType.png");
		m_inventoryTypeSize = { (float)m_spInventoryTypeTex->GetWidth(),(float)m_spInventoryTypeTex->GetHeight() / 3 };
		m_inventoryType2DPos.x = m_back1Tex2DPos.x - m_spBack1Tex->GetWidth() / 2;
		m_inventoryType2DPos.y = m_back1Tex2DPos.y + 150;

	}


	if (!m_spNotSelect)
	{
		m_spNotSelect = std::make_shared<KdTexture>();
		m_spNotSelect->Load("Asset/Textures/GameUI/Item/PlayerInventory/BagBackTex2.png");
		m_spBack2Tex = m_spNotSelect;
		m_back2Tex2DPos = m_back1Tex2DPos + Math::Vector2{ (float)m_spBack1Tex->GetWidth() / 2,0 } + Math::Vector2{ (float)m_spBack2Tex->GetWidth(),0 };
		m_back2Tex2DSize = { (float)m_spBack2Tex->GetWidth() , (float)m_spBack2Tex->GetHeight() };
	}


	if (!m_spContext->m_spNotSelectNameTex)
	{
		m_spContext->m_spNotSelectNameTex = std::make_shared<KdTexture>();
		m_spContext->m_spNotSelectNameTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/SelectNameBase.png");
		m_spContext->m_spSelectNameTex = m_spContext->m_spNotSelectNameTex;
		m_selectNameTexPos = m_back2Tex2DPos;
		m_selectNameTexPos.y = 223;
	}


	if (!m_spContext->m_spSelectExplanationTex)
	{
		m_spContext->m_spNotSelectExplanationTex = std::make_shared<KdTexture>();
		m_spContext->m_spNotSelectExplanationTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/SelectExplanationBase.png");
		m_spContext->m_spSelectExplanationTex = m_spContext->m_spNotSelectExplanationTex;
		m_selectExplanationTexPos = m_back2Tex2DPos;
		m_selectExplanationTexPos.y = 50;
	}

	if (!m_spContext->m_spUseTex)
	{
		m_spContext->m_spUseTex = std::make_shared<KdTexture>();
		m_spContext->m_spUseTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/UseTex.png");

		float x = m_back2Tex2DPos.x;
		float y = m_back2Tex2DPos.y - m_spBack2Tex->GetHeight() / 2 + m_spContext->m_spUseTex->GetHeight() / 2 + 20;
		m_spContext->m_useTex2DPos = { x,y };
	}

	if (!m_spContext->m_spPotionSelectTex)
	{
		m_spContext->m_spPotionSelectTex = std::make_shared<KdTexture>();
		m_spContext->m_spPotionSelectTex->Load("Asset/Textures/GameUI/Item/Potion/PotionSelect.png");
	}

	m_layerPriority = 1;

	// 各タブで使う位置
	m_spContext->m_back1Tex2DPos = m_back1Tex2DPos;
	m_spContext->m_back1TexWidth = (float)m_spBack1Tex->GetWidth();
	m_spContext->m_back2Tex2DPos = m_back2Tex2DPos;


	Math::Vector2 basePos = { -420, 225 };
	for (int i = 0; i < InventoryTypeSize; i++)
	{
		if (m_inventoryTypeChange[i].m_spIconTex)
		{
			continue;
		}

		m_inventoryTypeChange[i].m_spIconTex = std::make_shared<KdTexture>();


		switch (i)
		{
		case PlayerInventoryUI::PotionInventory:
			m_inventoryTypeChange[i].m_spIconTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/inventoryType/Potion.png");
			m_inventoryTypeChange[i].m_ID = PotionInventory;
			break;
		case PlayerInventoryUI::WeaponInventory:
			m_inventoryTypeChange[i].m_spIconTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/inventoryType/Wepon.png");
			m_inventoryTypeChange[i].m_ID = WeaponInventory;
			break;
		case PlayerInventoryUI::PlayerStatus:
			m_inventoryTypeChange[i].m_spIconTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/inventoryType/PlayerStatus.png");
			m_inventoryTypeChange[i].m_ID = PlayerStatus;
			break;
		default:
			break;
		}


		float w = m_inventoryTypeChange[i].m_spIconTex->GetWidth();
		float h = m_inventoryTypeChange[i].m_spIconTex->GetHeight();
		m_inventoryTypeChange[i].m_iconTexSize = { w,h };


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

	if (!m_spWeaponInventoryUI)
	{
		m_spWeaponInventoryUI = std::make_shared<WeponInventoryUI>();
		m_spWeaponInventoryUI->SetContext(m_spContext);
	}
	m_spWeaponInventoryUI->Init();
}

void PlayerInventoryUI::PreUpdate()
{

}

void PlayerInventoryUI::Update()
{
	PlayerInventoryOpen();

	if (m_spContext->m_useEffectTimer > 0.0f)
	{
		m_spContext->m_useEffectTimer -= DeltaTime::Instance().GetRealDeltaTime();
		if (m_spContext->m_useEffectTimer < 0.0f) { m_spContext->m_useEffectTimer = 0.0f; }
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
			std::shared_ptr<PlayerBase>spPlayer = m_spContext->m_wpPlayer.lock();
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


	if (m_spBack1Tex)
	{
		Math::Color color = { 1,1,1,1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spBack1Tex, m_back1Tex2DPos.x, m_back1Tex2DPos.y, nullptr, &color);
	}

	if (m_spInventoryTypeTex)
	{
		Math::Vector2 pivot = { 0,0.5 };
		Math::Rectangle rec = { 0,(long)(m_inventoryTypeSize.y * m_nowInventoryType),(long)(m_inventoryTypeSize.x),(long)(m_inventoryTypeSize.y) };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spInventoryTypeTex, m_inventoryType2DPos.x, m_inventoryType2DPos.y, m_inventoryTypeSize.x, m_inventoryTypeSize.y, &rec, nullptr, pivot);
	}

	if (m_spBack2Tex && m_nowInventoryType != PlayerInventoryUI::PlayerStatus)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spBack2Tex, m_back2Tex2DPos.x, m_back2Tex2DPos.y, m_back2Tex2DSize.x, m_back2Tex2DSize.y);

		Math::Vector2 pos = m_back2Tex2DPos;
		pos.y += 147;	// 右の枠の上の暗い帯（数字の枠）の真ん中
		pos.x += 85;

		// 数字が変わるときはちょっと跳ねる(色も変わる)
		Math::Color numColor = kWhiteColor;
		float numScale = 2.5f;	// 数字の枠（高さ50）に収まる大きさ（14 × 2.5 = 35）
		if (m_spContext->m_useEffectTimer > 0.0f && (m_nowInventoryType == PotionInventory || m_nowInventoryType == WeaponInventory))
		{
			
			float t = m_spContext->UseEffectT();
			switch (m_spContext->m_useEffectType)
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
				case PlayerInventoryUI::WeaponInventory:
					numColor = { 1.0f, 1.0f - 0.15f * t, 1.0f - 0.7f * t, 1.0f };   // 白→金→白
					break;
				default:
					break;
				}

				break;

				// 横に震える
			case UseEffectType::Shake:
				// 0 → 1 とまっすぐ進む値（演出の進み具合）
				float p = 1.0f - m_spContext->m_useEffectTimer / m_spContext->m_useEffectTime;

				// だんだん弱くなる揺れ幅（最初が一番大きい）
				float amp = 6.0f * (1.0f - p);

				// 左右に3往復
				pos.x += sinf(p * DirectX::XM_PI * 6.0f) * amp;
				numColor = { 1.0f, 1.0f - 0.6f * (1.0f - p), 1.0f - 0.6f * (1.0f - p), 1.0f };   // 赤→白に戻る
				break;
			}
		}
		// 右の枠の幅に収める(縦は跳ねる演出で大きくなるので制限しない)
		NumDraw::GetInstance().Draw(m_spContext->m_num, RAligned, pos, numColor, numScale, false, -1, { 170.0f, 0.0f });

		if (m_spContext->m_spSelectExplanationTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spContext->m_spSelectExplanationTex, m_selectExplanationTexPos.x, m_selectExplanationTexPos.y);
		}


		if (m_spContext->m_spSelectNameTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spContext->m_spSelectNameTex, m_selectNameTexPos.x, m_selectNameTexPos.y);
		}
	}



	//////////////////////////////////////////////////


	for (int i = 0; i < InventoryTypeSize; i++)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_inventoryTypeChange[i].m_spIconTex, m_inventoryTypeChange[i].m_2DPos.x, m_inventoryTypeChange[i].m_2DPos.y);


		if (i == m_nowInventoryType)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spContext->m_spPotionSelectTex, m_inventoryTypeChange[i].m_2DPos.x, m_inventoryTypeChange[i].m_2DPos.y);

		}
	}

	//////////////////////////////////////////////////

	// 所持金(上の木の帯の右端)
	DrawMoney();

	//////////////////////////////////////////////////

	InventoryTabBase* tab = GetTab(m_nowInventoryType);
	if (tab)
	{
		tab->DrawSprite();
	}
}

void PlayerInventoryUI::DrawMoney()
{
	// 金貨
	if (m_spCoinTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spCoinTex, (int)kCoinPos.x, (int)kCoinPos.y);
	}

	// 所持金はプレイヤーのインベントリから読む
	long money = 0;
	if (std::shared_ptr<PlayerBase> spPlayer = m_spContext->m_wpPlayer.lock())
	{
		if (std::shared_ptr<PlayerInventory> spInventory = spPlayer->GetPlayerInventory())
		{
			money = spInventory->GetMoney();
		}
	}

	// 数字(右揃え・3桁区切り)
	// 桁が多くて金貨にかぶるときは、横を縮めて収める
	NumDraw::GetInstance().Draw(money, RAligned, kMoneyNumPos, kWhiteColor, kMoneyNumSize, true, -1, kMoneyNumMaxSize);

	// 「G」
	if (m_spGoldTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spGoldTex, (int)kGoldPos.x, (int)kGoldPos.y);
	}
}

//=============================================
// 取得
//=============================================

InventoryTabBase* PlayerInventoryUI::GetTab(InventoryType _type)
{
	switch (_type)
	{
	case PlayerInventoryUI::PotionInventory:	return m_spPotionInventoryUI.get();
	case PlayerInventoryUI::WeaponInventory:		return m_spWeaponInventoryUI.get();
	case PlayerInventoryUI::PlayerStatus:		return m_spPlayerStatusInventoryUI.get();
	default:									return nullptr;
	}
}

//=============================================
// その他の処理
//=============================================

void PlayerInventoryUI::PlayerInventoryOpen()
{

	std::shared_ptr<PlayerBase>spPlayer = m_spContext->m_wpPlayer.lock();
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
				std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/BagOpen.wav", false);
				if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
			}
		}
		else
		{
			{
				std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/BagClose.wav", false);
				if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
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
				m_spChangeBack2Tex = m_spNotSelect;
			}
		}
	}

}

void PlayerInventoryUI::InventoryTypeChangeUpdate()
{

	POINT mousePos = MouseInfo::Instance().m_windowPos;

	for (auto& icon : m_inventoryTypeChange)
	{
		float left = icon.m_2DPos.x - icon.m_iconTexSize.x / 2;
		float right = icon.m_2DPos.x + icon.m_iconTexSize.x / 2;
		float top = icon.m_2DPos.y + icon.m_iconTexSize.y / 2;
		float bot = icon.m_2DPos.y - icon.m_iconTexSize.y / 2;


		if (mousePos.x >= left && mousePos.x <= right &&
			mousePos.y >= bot && mousePos.y <= top)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			icon.m_hit = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				// 違うタブに切り替えたときだけ、ページをめくる音
				if (m_nowInventoryType != icon.m_ID)
				{
					{
						std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/PageTurn.wav", false);
						if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
					}
				}

				m_nowInventoryType = icon.m_ID;

				// ステータスは選んだときの処理なし（以前と同じ）
				switch (m_nowInventoryType)
				{
				case PlayerInventoryUI::PotionInventory:
					m_spPotionInventoryUI->OnSelect();
					break;
				case PlayerInventoryUI::WeaponInventory:
					m_spWeaponInventoryUI->OnSelect();
					break;
				default:
					break;
				}
			}
		}
		else
		{
			icon.m_hit = false;
		}
	}
}
