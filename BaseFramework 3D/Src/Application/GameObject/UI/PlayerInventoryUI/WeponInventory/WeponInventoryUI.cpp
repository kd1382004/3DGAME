#include "WeponInventoryUI.h"
#include"../../../../Scene/SceneManager.h"

#include"../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Scene/GameScene/GameScene.h"
#include"../../../../Info/MouseInfo/MouseInfo.h"

#include"../../../Potions/PotionsType.h"
#include"../../../Potions/PotionUseController.h"

#include"../../../Character/Player/PlayerBase.h"
#include"../../../Character/Player/PlayerInventory/PlayerInventory.h"

#include"../../../Potions/PotionTexInfo/PotionTexInfo.h"

#include"../../../../Info/NumDraw/NumDraw.h"

#include"../../../Weapon/WeaponBase.h"

void WeponInventoryUI::Init()
{
	////////////////////////////////////////////////////////////////////////////
	//武器強化用
	Math::Vector2 weaponsStrengtheningBasePos = { m_spContext->back1Tex2DPos.x,-200 };
	float weaponsStrengtheningOffset = 150.0f;
	float vecL = 150.0f;

	if (!m_weaponsStrengtheningButtonTex)
	{
		m_weaponsStrengtheningButtonTex2DPos = m_spContext->useTex2DPos;
		m_weaponsStrengtheningButtonTex = std::make_shared<KdTexture>();
		m_weaponsStrengtheningButtonTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/StrengtheningButton.png");
	}
	if (!m_PreconditionNodeTex)
	{
		m_weaponsStrengtheningButtonTex2DPos = m_spContext->useTex2DPos;
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
		Math::Vector2 pos = m_spContext->back2Tex2DPos;
		pos.y += 150;
		pos.x -= 80;
		m_LVTex2DPos = pos;

	}

	if (!m_LineTex)
	{
		m_LineTex = std::make_shared<KdTexture>();
		m_LineTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Line.png");
	}

	if (!m_numSlashTex)
	{
		m_numSlashTex = std::make_shared<KdTexture>();
		m_numSlashTex->Load("Asset/Textures/Num/NumSlash.png");
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
		case WeponInventoryUI::WeaponsStrengthening_Attck:
			pos.x -= weaponsStrengtheningOffset * 2;
			pos.y += addY;
			break;
		case WeponInventoryUI::WeaponsStrengthening_Stamina:
			pos.x -= weaponsStrengtheningOffset;
			pos.y += addY;
			break;
		case WeponInventoryUI::WeaponsStrengthening_ChargeTime:
			pos.x += weaponsStrengtheningOffset * 2;
			pos.y += addY;
			break;
		case WeponInventoryUI::WeaponsStrengthening_Characteristic:
			pos.x = m_spContext->back1Tex2DPos.x;
			pos.y += addY;
			break;
		case WeponInventoryUI::WeaponsStrengthening_DefenseIgnore:
			pos.x += weaponsStrengtheningOffset;
			pos.y += addY;
			break;
		case WeponInventoryUI::WeaponsStrengthening_Characteristic1:
			rad = DirectX::XMConvertToRadians(45.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.y + cos(rad) * vecL;
			break;
		case WeponInventoryUI::WeaponsStrengthening_Characteristic2:
			rad = DirectX::XMConvertToRadians(0.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.y + cos(rad) * vecL;
			break;
		case WeponInventoryUI::WeaponsStrengthening_Characteristic3:
			rad = DirectX::XMConvertToRadians(-45.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.y + cos(rad) * vecL;
			break;
		case WeponInventoryUI::WeaponsStrengthening_Icon:
			break;
		case WeponInventoryUI::WeaponsStrengtheningSiz:
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
			case WeponInventoryUI::WeaponsStrengthening_Characteristic:
			case WeponInventoryUI::WeaponsStrengthening_Characteristic1:
			case WeponInventoryUI::WeaponsStrengthening_Characteristic2:
			case WeponInventoryUI::WeaponsStrengthening_Characteristic3:
			case WeponInventoryUI::WeaponsStrengthening_Icon:



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

	if (!m_LineGlowTex)
	{
		m_LineGlowTex = std::make_shared<KdTexture>();
		m_LineGlowTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/LineGlow.png");
	}
}

void WeponInventoryUI::OnSelect()
{
	// UI と武器の強化 enum は同じ並び（末尾が UI では Icon、武器では Body）であることが前提
	static_assert(static_cast<int>(WeaponsStrengtheningSiz) == static_cast<int>(::WeaponsStrengtheningSiz),
		"PlayerInventoryUI と WeaponBase の強化 enum の数が違います");

	std::shared_ptr<PlayerBase>spPlayer = m_spContext->wpPlayer.lock();
	if (!spPlayer) { return; }
	std::shared_ptr<WeaponBase>spWeapon = spPlayer->GetWeapon().lock();
	if (!spWeapon) { return; }
	std::shared_ptr<PotionTexInfo>_spTexInfo = m_spContext->wpPotionTexInfo.lock();
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
	m_spContext->selectExplanationTex = m_weaponsStrengtheningInfo[m_selectIndex].m_ExplanationTex;
	m_spContext->selectNameTex = m_weaponsStrengtheningInfo[m_selectIndex].m_NameTex;
	m_spContext->num = m_weaponsStrengtheningInfo[m_selectIndex].Lv;
	m_selectWeaponsStrengtheningnextLVNum = m_weaponsStrengtheningInfo[m_selectIndex].nextLvNum;
	m_weaponsStrengtheningInfo[m_selectIndex].m_hit = true;

	WeaponChooseFlg(m_selectIndex);


	WeaponsStrengtheningInfoInitPos();
}

void WeponInventoryUI::Update()
{
	//持ってる数を武器に渡す
	std::shared_ptr<PlayerBase>spPlayer = m_spContext->wpPlayer.lock();
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


	const float dt = DeltaTime::Instance().GetRealDeltaTime();
	const float k = std::min(1.0f, dt * m_lineGlowFadeSpeed);
	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		WeaponsStrengtheningInfo& info = m_weaponsStrengtheningInfo[i];
		float target = (info.Lv > 0) ? 1.0f : 0.0f;
		info.lineGlow += (target - info.lineGlow) * k;
	}
}

void WeponInventoryUI::DrawSprite()
{

	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		//前提ノードから伸ばす

		int x1 = 0;
		int x2 = 0;
		int y1 = 0;
		int y2 = 0;
		int PrIndex = m_weaponsStrengtheningInfo[i].PreconditionNodeID_index;
		if (PrIndex < 0) { continue; } // 前提なし（根元）は線を引かない


		//前提ノード
		x1 = m_weaponsStrengtheningInfo[PrIndex].iconPos.x;
		y1 = m_weaponsStrengtheningInfo[PrIndex].iconPos.y;

		//ノード位置
		x2 = m_weaponsStrengtheningInfo[i].iconPos.x;
		y2 = m_weaponsStrengtheningInfo[i].iconPos.y;

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
		}


		// 強化されてるなら光らせる（フェード付き）
		float glow = m_weaponsStrengtheningInfo[i].lineGlow;
		if (glow > 0.001f && m_LineGlowTex)
		{
			Math::Color glowCol = { 1.0f, 0.8f, 0.4f, 0.6f * glow };
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_LineGlowTex, x1, y1, siz.x * glow, siz.y * 3.0f * (0.7f + 0.3f * glow), nullptr, &glowCol, pivo, deg);
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


		// 0→1→0 と山なりに変化する値
		float t = 0.0f;
		if (m_spContext->useEffectTimer > 0.0f)
		{
			t = sinf(DirectX::XM_PI * (1.0f - m_spContext->useEffectTimer / m_spContext->useEffectTime));
		}
		float w = m_weaponsStrengtheningInfo[i].iconSiz.x;
		float h = m_weaponsStrengtheningInfo[i].iconSiz.y;
		bool isUseEffect = (t > 0.0f && m_weaponsStrengtheningInfo[i].MyNodeID == m_spContext->useEffectItemID);
		if (isUseEffect)
		{
			w *= 1.0f + 0.3f * t;
			h *= 1.0f + 0.3f * t;
		}

		// 強化が可能なら光らす
		if (m_weaponsStrengtheningInfo[i].strengtheningFlg && !m_weaponsStrengtheningInfo[i].LvMaxFlg)
		{
			// 強化可能かどうか（目標の濃さ）
			auto& info = m_weaponsStrengtheningInfo[i];
			const bool canStrengthen = info.strengtheningFlg && !info.LvMaxFlg;
			const float dt = DeltaTime::Instance().GetRealDeltaTime();

			// 目標に向かってじわっと変化（出るときは速め、消えるときはゆっくり）
			const float target = canStrengthen ? 1.0f : 0.0f;
			const float fadeSpeed = canStrengthen ? 8.0f : 2.5f;
			info.auraAlpha += (target - info.auraAlpha) * std::min(1.0f, dt * fadeSpeed);

			if (info.auraAlpha > 0.005f)
			{
				KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);

				// 点滅（1周 約2秒）。2πでループさせて飛ばないようにする
				info.timer += dt * DirectX::XM_PI;
				if (info.timer > DirectX::XM_2PI) { info.timer -= DirectX::XM_2PI; }
				float pulse = (sinf(info.timer) + 1.0f) * 0.5f;

				// 消えていくほどふわっと広がる
				float fadeSpread = (1.0f - info.auraAlpha) * 0.4f;
				float scale = 1.0f + pulse * 0.25f + fadeSpread;

				// 青 → 白 → 青。濃さはオーラの濃さに合わせて薄くする
				Math::Color auraColor = { pulse, pulse, 1.0f, info.auraAlpha };

				KdShaderManager::Instance().m_spriteShader.DrawTex(
					info.iconTex,
					info.iconPos.x,
					info.iconPos.y,
					w * scale * choseSizUp,
					h * scale * choseSizUp,
					nullptr,
					&auraColor
				);

				KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
			}
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
					const float numScale = 1.5f;
					const float numGapX = 9.0f * numScale * 0.5f + 2.0f;   // 「/」の半分 ＋ すき間
					const float numGapY = 3.0f * numScale;
					Math::Vector2 basePos = m_weaponsStrengtheningIconInfo.m_2DPos;
					basePos.y -= 47;

					{
						//現在の持ってるアイテムの数（「/」の左に右揃え）
						Math::Vector2 pos = basePos;
						pos.x -= numGapX;
						pos.y += numGapY;
						Math::Color color = kWhiteColor;
						if (m_weaponsStrengtheningIconInfo.m_num < m_selectWeaponsStrengtheningnextLVNum)
						{
							color = kRedColor;

							float shakeX = 0.0f;
							if (m_spContext->useEffectType == UseEffectType::Shake && m_spContext->useEffectTimer > 0.0f)
							{
								float p = 1.0f - m_spContext->useEffectTimer / m_spContext->useEffectTime;
								shakeX = sinf(p * DirectX::XM_PI * 6.0f) * 6.0f * (1.0f - p);
								pos.x += shakeX;
							}
						}
						NumDraw::GetInstance().Drow(m_weaponsStrengtheningIconInfo.m_num, Aligned::RAligned, pos, color, numScale);
					}
					{
						//「/」
						if (m_numSlashTex)
						{
							Math::Rectangle rec = { 0,0,9,14 };
							KdShaderManager::Instance().m_spriteShader.DrawTex(m_numSlashTex, (int)basePos.x, (int)basePos.y, (int)(9 * numScale), (int)(14 * numScale), &rec);
						}
					}
					{
						//レベルアップに必要なアイテムの数（「/」の右に左揃え）
						Math::Vector2 pos = basePos;
						pos.x += numGapX;
						pos.y -= numGapY;
						NumDraw::GetInstance().Drow(m_selectWeaponsStrengtheningnextLVNum, Aligned::LAligned, pos, kWhiteColor, numScale);
					}
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

void WeponInventoryUI::WeaponsStrengtheningInfoInitPos()
{

}

void WeponInventoryUI::WeaponsStrengtheningTexInfo()
{
	std::shared_ptr<PlayerBase >spPlayer = m_spContext->wpPlayer.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
	if (!spPlayerInventory) { return; }

	Math::Vector2 m_iconBase2DPos = m_spContext->useTex2DPos; // 初期位置
	m_iconBase2DPos.y += 90;
	for (Inventory weapon : spPlayerInventory->GetWeaponsStrengtheningInventory())
	{
		if (weapon.m_ID == m_weaponsStrengtheningInfo[m_selectIndex].strengtheningItem_ID)
		{
			InventoryItemIconInfo itemIconInfo;
			itemIconInfo.m_ItemID = weapon.m_ID;
			itemIconInfo.m_2DPos = m_iconBase2DPos;
			itemIconInfo.m_num = weapon.m_num;
			itemIconInfo.m_name = weapon.m_name;

			std::shared_ptr<PotionTexInfo>_spTexInfo = m_spContext->wpPotionTexInfo.lock();
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

void WeponInventoryUI::WeaponsStrengtheningIconHit()
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
				m_spContext->selectExplanationTex = weapon.m_ExplanationTex;
				m_selectIndex = i;
				m_spContext->num = weapon.Lv;
				m_selectWeaponsStrengtheningnextLVNum = weapon.nextLvNum;

				// 前提なし、または前提ノードが解放済みなら通常の名前・説明
				const int preIndex = weapon.PreconditionNodeID_index;
				if (preIndex < 0 || m_weaponsStrengtheningInfo[preIndex].Lv > 0)
				{
					m_spContext->selectNameTex = weapon.m_NameTex;
					m_spContext->selectExplanationTex = weapon.m_ExplanationTex;
				}
				else
				{
					m_spContext->selectNameTex = m_PreconditionNodeNameTex;
					m_spContext->selectExplanationTex = m_PreconditionNodeExplanationTex;
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

void WeponInventoryUI::WeaponsStrengtheningButtonHit()
{
	if (!m_weaponsStrengtheningButtonTex) { return; }

	std::shared_ptr<PlayerBase>spPlayer = m_spContext->wpPlayer.lock();
	if (!spPlayer) { return; }
	std::shared_ptr<WeaponBase>spWeapon = spPlayer->GetWeapon().lock();
	if (!spWeapon) { return; }

	POINT mousePos = MouseInfo::Instance().m_windowPos;

	float Left = m_weaponsStrengtheningButtonTex2DPos.x - m_weaponsStrengtheningButtonTex->GetWidth() / 2;
	float Right = m_weaponsStrengtheningButtonTex2DPos.x + m_weaponsStrengtheningButtonTex->GetWidth() / 2;
	float Top = m_weaponsStrengtheningButtonTex2DPos.y + m_weaponsStrengtheningButtonTex->GetHeight() / 2;
	float Bot = m_weaponsStrengtheningButtonTex2DPos.y - m_weaponsStrengtheningButtonTex->GetHeight() / 2;

	// 前提ノードが解放されているか（表示の切り替えに使うので毎フレーム更新）
	m_PreconditionNodeFlg = !spWeapon->IsPreconditionCleared(m_selectIndex);

	if (mousePos.x >= Left && mousePos.x <= Right &&
		mousePos.y >= Bot && mousePos.y <= Top)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{

			// 前提ノードが解放されているか
			if (m_PreconditionNodeFlg)
			{

				// 前提ノードがまだ → ぶるっと震わせて、前提のノードを光らせる
				m_spContext->useEffectType = UseEffectType::Shake;
				m_spContext->useEffectTimer = m_spContext->useEffectTime;
				m_spContext->useEffectItemID = m_weaponsStrengtheningInfo[m_selectIndex].PreconditionNodeID;
				return;
			}

			if (spWeapon->IsWeaponsStrengtheningPossible(m_selectIndex))
			{
				spWeapon->AddWeaponsStrengtheningInfo(m_selectIndex);
				m_spContext->num = spWeapon->GetWeaponsStrengtheningLv(m_selectIndex);
				m_weaponsStrengtheningInfo[m_selectIndex].Lv = m_spContext->num;


				int WeaponsStrengtheningItemID = m_weaponsStrengtheningInfo[m_selectIndex].strengtheningItem_ID;
				spPlayer->GetPlayerInventory()->UseWeaponsStrengtheningInventory(WeaponsStrengtheningItemID, m_weaponsStrengtheningInfo[m_selectIndex].nextLvNum);
				m_weaponsStrengtheningInfo[m_selectIndex].nextLvNum = spWeapon->GetWeaponsStrengtheningNextLvNum(m_selectIndex);
				m_selectWeaponsStrengtheningnextLVNum = m_weaponsStrengtheningInfo[m_selectIndex].nextLvNum;

				m_spContext->useEffectType = UseEffectType::Bounce;
				m_spContext->useEffectTimer = m_spContext->useEffectTime;
				m_spContext->useEffectItemID = m_weaponsStrengtheningInfo[m_selectIndex].MyNodeID;
			}
			else if (!m_weaponsStrengtheningInfo[m_selectIndex].LvMaxFlg)
			{

				m_spContext->useEffectType = UseEffectType::Shake;
				m_spContext->useEffectTimer = m_spContext->useEffectTime;
				m_spContext->useEffectItemID = -999;
			}
		}
	}

}

void WeponInventoryUI::WeaponChooseFlg(int _index)
{
	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		m_weaponsStrengtheningInfo[i].ChooseFlg = (i == _index);
	}
}
