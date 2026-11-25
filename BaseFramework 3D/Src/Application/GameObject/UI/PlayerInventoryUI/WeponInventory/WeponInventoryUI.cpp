//---- 自分のヘッダー ----
#include "WeponInventoryUI.h"												//インベントリ画面(武器強化タブ)

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
#include"../../../../Info/VOL/VOLInfo.h"									//音量設定
#include"../../../../Info/KeyInfo/KeyInfo.h"								//キー入力
#include"../../../../Info/MouseInfo/MouseInfo.h"							//マウス入力
#include"../../../../Info/NumDraw/NumDraw.h"								//数字の画像描画

//=============================================
// 基本の処理
//=============================================

void WeponInventoryUI::Init()
{
	////////////////////////////////////////////////////////////////////////////
	//武器強化用
	Math::Vector2 weaponsStrengtheningBasePos = { m_spContext->m_back1Tex2DPos.x,-200 };
	float weaponsStrengtheningOffset = 150.0f;
	float vecL = 150.0f;

	if (!m_spWeaponsStrengtheningButtonTex)
	{
		m_weaponsStrengtheningButtonTex2DPos = m_spContext->m_useTex2DPos;
		m_spWeaponsStrengtheningButtonTex = std::make_shared<KdTexture>();
		m_spWeaponsStrengtheningButtonTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/StrengtheningButton.png");
	}
	if (!m_spPreconditionNodeTex)
	{
		m_weaponsStrengtheningButtonTex2DPos = m_spContext->m_useTex2DPos;
		m_spPreconditionNodeTex = std::make_shared<KdTexture>();
		m_spPreconditionNodeTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Not.png");
	}

	if (!m_spPreconditionNodeIconTex)
	{
		m_spPreconditionNodeIconTex = std::make_shared<KdTexture>();
		m_spPreconditionNodeIconTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/NotIcon.png");
	}

	if (!m_spPreconditionNodeExplanationTex)
	{
		m_spPreconditionNodeExplanationTex = std::make_shared<KdTexture>();
		m_spPreconditionNodeExplanationTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/NotSelectExplanationBase.png");
	}
	if (!m_spPreconditionNodeNameTex)
	{
		m_spPreconditionNodeNameTex = std::make_shared<KdTexture>();
		m_spPreconditionNodeNameTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/NotSelectNameBase.png");
	}

	if (!m_spLvMaxButtonTex)
	{
		m_spLvMaxButtonTex = std::make_shared<KdTexture>();
		m_spLvMaxButtonTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Max.png");
	}

	if (!m_spLVTex)
	{
		m_spLVTex = std::make_shared<KdTexture>();
		m_spLVTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/LV.png");
		Math::Vector2 pos = m_spContext->m_back2Tex2DPos;
		pos.y += 147;	// 数字の枠の真ん中（数字と同じ高さ）
		pos.x -= 80;
		m_LVTex2DPos = pos;

	}

	if (!m_spLineTex)
	{
		m_spLineTex = std::make_shared<KdTexture>();
		m_spLineTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Line.png");
	}

	if (!m_spNumSlashTex)
	{
		m_spNumSlashTex = std::make_shared<KdTexture>();
		m_spNumSlashTex->Load("Asset/Textures/Num/NumSlash.png");
	}


	float rad = DirectX::XMConvertToRadians(90.0f); // 0°,90°,180°,270°

	m_selectWeaponsStrengtheningIconSize = { 64,64 };

	for (int i = 0; i < WeaponsStrengtheningSize; i++)
	{
		Math::Vector2 pos = weaponsStrengtheningBasePos;

		m_weaponsStrengtheningInfo[i].m_lv = 0;

		float addY = 100;

		switch (i)
		{
		case WeponInventoryUI::WeaponsStrengthening_Attack:
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
			pos.x = m_spContext->m_back1Tex2DPos.x;
			pos.y += addY;
			break;
		case WeponInventoryUI::WeaponsStrengthening_DefenseIgnore:
			pos.x += weaponsStrengtheningOffset;
			pos.y += addY;
			break;
		case WeponInventoryUI::WeaponsStrengthening_Characteristic1:
			rad = DirectX::XMConvertToRadians(45.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].m_iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].m_iconPos.y + cos(rad) * vecL;
			break;
		case WeponInventoryUI::WeaponsStrengthening_Characteristic2:
			rad = DirectX::XMConvertToRadians(0.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].m_iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].m_iconPos.y + cos(rad) * vecL;
			break;
		case WeponInventoryUI::WeaponsStrengthening_Characteristic3:
			rad = DirectX::XMConvertToRadians(-45.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].m_iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].m_iconPos.y + cos(rad) * vecL;
			break;
		case WeponInventoryUI::WeaponsStrengthening_Icon:
			break;
		case WeponInventoryUI::WeaponsStrengtheningSize:
			break;
		default:
			break;
		}

		// 前提ノードはマスタから読む（WeaponsStrengtheningInfoInit）
		m_weaponsStrengtheningInfo[i].m_iconPos = pos;
		m_weaponsStrengtheningInfo[i].m_iconSize = m_selectWeaponsStrengtheningIconSize;
		m_weaponsStrengtheningInfo[i].m_nextLvNum = 0;

		if (!m_weaponsStrengtheningInfo[i].m_spIconTex)
		{
			m_weaponsStrengtheningInfo[i].m_spIconTex = std::make_shared<KdTexture>();
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
			m_weaponsStrengtheningInfo[i].m_spIconTex->Load(filePath);
		}
	}

	if (!m_spLineGlowTex)
	{
		m_spLineGlowTex = std::make_shared<KdTexture>();
		m_spLineGlowTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/LineGlow.png");
	}
}

void WeponInventoryUI::Update()
{
	//持ってる数を武器に渡す
	std::shared_ptr<PlayerBase>spPlayer = m_spContext->m_wpPlayer.lock();
	std::shared_ptr<WeaponBase>spWeapon = nullptr;
	if (spPlayer) { spWeapon = spPlayer->GetWeapon().lock(); }
	if (spPlayer && spWeapon)
	{
		for (int i = 0; i < WeaponsStrengtheningSize; i++)
		{
			int weaponsStrengtheningItemID = m_weaponsStrengtheningInfo[i].m_strengtheningItemID;
			int num = spPlayer->GetPlayerInventory()->GetWeaponsStrengtheningInventoryNum(weaponsStrengtheningItemID);
			spWeapon->SetWeaponsStrengtheningNum(i, num);
		}

	}

	WeaponsStrengtheningIconHit();

	WeaponsStrengtheningTexInfo();

	WeaponsStrengtheningButtonHit();


	const float dt = DeltaTime::Instance().GetRealDeltaTime();
	const float k = std::min(1.0f, dt * m_lineGlowFadeSpeed);
	for (int i = 0; i < WeaponsStrengtheningSize; i++)
	{
		WeaponsStrengtheningInfo& info = m_weaponsStrengtheningInfo[i];
		float target = (info.m_lv > 0) ? 1.0f : 0.0f;
		info.m_lineGlow += (target - info.m_lineGlow) * k;
	}
}

void WeponInventoryUI::DrawSprite()
{

	for (int i = 0; i < WeaponsStrengtheningSize; i++)
	{
		//前提ノードから伸ばす

		int x1 = 0;
		int x2 = 0;
		int y1 = 0;
		int y2 = 0;
		int prIndex = m_weaponsStrengtheningInfo[i].m_preconditionNodeIndex;
		if (prIndex < 0) { continue; } // 前提なし（根元）は線を引かない


		//前提ノード
		x1 = m_weaponsStrengtheningInfo[prIndex].m_iconPos.x;
		y1 = m_weaponsStrengtheningInfo[prIndex].m_iconPos.y;

		//ノード位置
		x2 = m_weaponsStrengtheningInfo[i].m_iconPos.x;
		y2 = m_weaponsStrengtheningInfo[i].m_iconPos.y;

		float dx = x2 - x1;
		float dy = y2 - y1;

		// ラジアン
		float rad = atan2f(dy, dx);

		// 度数法（-180 ～ +180）
		float deg = rad * 180.0f / 3.14159265f;


		Math::Vector2 pivot = { 0,0.5 };
		Math::Vector2 size = { 0,20 };
		size.x = sqrtf(dx * dx + dy * dy);
		Math::Color col = kWhiteColor;
		float defaultScale = 0.8;
		float scale = 1;

		int preconditionNodeIDIndex = m_weaponsStrengtheningInfo[i].m_preconditionNodeIndex;

		if (preconditionNodeIDIndex < 0 || preconditionNodeIDIndex >= WeaponsStrengtheningSize)
		{
			break;
		}

		if (m_weaponsStrengtheningInfo[preconditionNodeIDIndex].m_lv == 0)
		{
			col = { 1,1,1,0.5 };
			defaultScale = 0.5;
		}

		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
		if (m_spLineTex)
		{
			// 通常の線
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spLineTex, x1, y1, size.x, size.y * defaultScale, nullptr, &col, pivot, deg);
		}


		// 強化されてるなら光らせる（フェード付き）
		float glow = m_weaponsStrengtheningInfo[i].m_lineGlow;
		if (glow > 0.001f && m_spLineGlowTex)
		{
			Math::Color glowCol = { 1.0f, 0.8f, 0.4f, 0.6f * glow };
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spLineGlowTex, x1, y1, size.x * glow, size.y * 3.0f * (0.7f + 0.3f * glow), nullptr, &glowCol, pivot, deg);
		}
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
	}

	for (int i = 0; i < WeaponsStrengtheningSize; i++)
	{
		if (!m_weaponsStrengtheningInfo[i].m_spIconTex) { continue; }
		bool preconditionNodeIconFlg = false;

		float choseSizUp = 1;
		if (m_weaponsStrengtheningInfo[i].m_chooseFlg)
		{
			choseSizUp = 1.3;
		}


		Math::Color iconCol = { 1,1,1,1 };


		const int preIndex = m_weaponsStrengtheningInfo[i].m_preconditionNodeIndex;
		if (preIndex >= 0 && m_weaponsStrengtheningInfo[preIndex].m_lv == 0)
		{
			m_weaponsStrengtheningInfo[i].m_strengtheningFlg = false;
			iconCol = { 0.5,0.5,0.5,1 };
			preconditionNodeIconFlg = true;
		}

		if (m_weaponsStrengtheningInfo[i].m_lv == 0)
		{
			iconCol = { 0.5,0.5,0.5,1 };
		}


		// 0→1→0 と山なりに変化する値
		float t = 0.0f;
		if (m_spContext->m_useEffectTimer > 0.0f)
		{
			t = sinf(DirectX::XM_PI * (1.0f - m_spContext->m_useEffectTimer / m_spContext->m_useEffectTime));
		}
		float w = m_weaponsStrengtheningInfo[i].m_iconSize.x;
		float h = m_weaponsStrengtheningInfo[i].m_iconSize.y;
		bool isUseEffect = (t > 0.0f && m_weaponsStrengtheningInfo[i].m_myNodeID == m_spContext->m_useEffectItemID);
		if (isUseEffect)
		{
			w *= 1.0f + 0.3f * t;
			h *= 1.0f + 0.3f * t;
		}

		// 強化が可能なら光らす
		if (m_weaponsStrengtheningInfo[i].m_strengtheningFlg && !m_weaponsStrengtheningInfo[i].m_lvMaxFlg)
		{
			// 強化可能かどうか（目標の濃さ）
			auto& info = m_weaponsStrengtheningInfo[i];
			const bool canStrengthen = info.m_strengtheningFlg && !info.m_lvMaxFlg;
			const float dt = DeltaTime::Instance().GetRealDeltaTime();

			// 目標に向かってじわっと変化（出るときは速め、消えるときはゆっくり）
			const float target = canStrengthen ? 1.0f : 0.0f;
			const float fadeSpeed = canStrengthen ? 8.0f : 2.5f;
			info.m_auraAlpha += (target - info.m_auraAlpha) * std::min(1.0f, dt * fadeSpeed);

			if (info.m_auraAlpha > 0.005f)
			{
				KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);

				// 点滅（1周 約2秒）。2πでループさせて飛ばないようにする
				info.m_timer += dt * DirectX::XM_PI;
				if (info.m_timer > DirectX::XM_2PI) { info.m_timer -= DirectX::XM_2PI; }
				float pulse = (sinf(info.m_timer) + 1.0f) * 0.5f;

				// 消えていくほどふわっと広がる
				float fadeSpread = (1.0f - info.m_auraAlpha) * 0.4f;
				float scale = 1.0f + pulse * 0.25f + fadeSpread;

				// 青 → 白 → 青。濃さはオーラの濃さに合わせて薄くする
				Math::Color auraColor = { pulse, pulse, 1.0f, info.m_auraAlpha };

				KdShaderManager::Instance().m_spriteShader.DrawTex(
					info.m_spIconTex,
					info.m_iconPos.x,
					info.m_iconPos.y,
					w * scale * choseSizUp,
					h * scale * choseSizUp,
					nullptr,
					&auraColor
				);

				KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
			}
		}



		//通常アイコン
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_weaponsStrengtheningInfo[i].m_spIconTex, m_weaponsStrengtheningInfo[i].m_iconPos.x, m_weaponsStrengtheningInfo[i].m_iconPos.y, w * choseSizUp, h * choseSizUp, nullptr, &iconCol);


		//使われてたら光らす
		if (isUseEffect)
		{
			Math::Color flash = { 1,1,1,t * 0.8f };
			KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_weaponsStrengtheningInfo[i].m_spIconTex, m_weaponsStrengtheningInfo[i].m_iconPos.x, m_weaponsStrengtheningInfo[i].m_iconPos.y, w * choseSizUp, h * choseSizUp, nullptr, &flash);
			KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
		}

		//前提ノードが解放されてなければ描画
		if (m_spPreconditionNodeIconTex && preconditionNodeIconFlg)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPreconditionNodeIconTex, m_weaponsStrengtheningInfo[i].m_iconPos.x, m_weaponsStrengtheningInfo[i].m_iconPos.y, m_weaponsStrengtheningInfo[i].m_iconSize.x * choseSizUp, m_weaponsStrengtheningInfo[i].m_iconSize.y * choseSizUp, nullptr, &iconCol);
		}


	}

	if (!m_weaponsStrengtheningInfo[m_selectIndex].m_lvMaxFlg)
	{
		if (m_weaponsStrengtheningIconInfo.m_spIconTex)
		{
			{
				KdShaderManager::Instance().m_spriteShader.DrawTex(m_weaponsStrengtheningIconInfo.m_spIconTex, m_weaponsStrengtheningIconInfo.m_2DPos.x, m_weaponsStrengtheningIconInfo.m_2DPos.y, 60, 60);
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
						if (m_weaponsStrengtheningIconInfo.m_num < m_selectWeaponsStrengtheningNextLVNum)
						{
							color = kRedColor;

							float shakeX = 0.0f;
							if (m_spContext->m_useEffectType == UseEffectType::Shake && m_spContext->m_useEffectTimer > 0.0f)
							{
								float p = 1.0f - m_spContext->m_useEffectTimer / m_spContext->m_useEffectTime;
								shakeX = sinf(p * DirectX::XM_PI * 6.0f) * 6.0f * (1.0f - p);
								pos.x += shakeX;
							}
						}
						// アイコンの半分の幅に収める(「/」の左側)
						NumDraw::GetInstance().Draw(m_weaponsStrengtheningIconInfo.m_num, Aligned::RAligned, pos, color, numScale, false, -1, { m_selectWeaponsStrengtheningIconSize.x / 2.0f, 0.0f });
					}
					{
						//「/」
						if (m_spNumSlashTex)
						{
							Math::Rectangle rec = { 0,0,9,14 };
							KdShaderManager::Instance().m_spriteShader.DrawTex(m_spNumSlashTex, (int)basePos.x, (int)basePos.y, (int)(9 * numScale), (int)(14 * numScale), &rec);
						}
					}
					{
						//レベルアップに必要なアイテムの数（「/」の右に左揃え）
						Math::Vector2 pos = basePos;
						pos.x += numGapX;
						pos.y -= numGapY;
						// アイコンの半分の幅に収める(「/」の右側)
						NumDraw::GetInstance().Draw(m_selectWeaponsStrengtheningNextLVNum, Aligned::LAligned, pos, kWhiteColor, numScale, false, -1, { m_selectWeaponsStrengtheningIconSize.x / 2.0f, 0.0f });
					}
				}
			}
		}



		//強化ボタン
		if (m_spWeaponsStrengtheningButtonTex && !m_preconditionNodeFlg)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spWeaponsStrengtheningButtonTex, m_weaponsStrengtheningButtonTex2DPos.x, m_weaponsStrengtheningButtonTex2DPos.y);
		}

		//前提ノード
		if (m_spPreconditionNodeTex && m_preconditionNodeFlg)
		{

			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPreconditionNodeTex, m_weaponsStrengtheningButtonTex2DPos.x, m_weaponsStrengtheningButtonTex2DPos.y);

		}

	}
	else
	{
		//レベルマックス
		if (m_spLvMaxButtonTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spLvMaxButtonTex, m_weaponsStrengtheningButtonTex2DPos.x, m_weaponsStrengtheningButtonTex2DPos.y);
		}

	}

	//LV（数字の枠に収まるよう、数字と同じ高さ35に縮める）
	if (m_spLVTex)
	{
		Math::Vector2 pivot = { 0,0.5 };
		const float lvH = 35.0f;
		const float lvW = m_spLVTex->GetWidth() * lvH / m_spLVTex->GetHeight();
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spLVTex, m_LVTex2DPos.x, m_LVTex2DPos.y, lvW, lvH, nullptr, nullptr, pivot);
	}

}

//=============================================
// その他の処理
//=============================================

void WeponInventoryUI::OnSelect()
{
	// UI と武器の強化 enum は同じ並び（末尾が UI では Icon、武器では Body）であることが前提
	static_assert(static_cast<int>(WeaponsStrengtheningSize) == static_cast<int>(::WeaponsStrengtheningSize),
		"PlayerInventoryUI と WeaponBase の強化 enum の数が違います");

	std::shared_ptr<PlayerBase>spPlayer = m_spContext->m_wpPlayer.lock();
	if (!spPlayer) { return; }
	std::shared_ptr<WeaponBase>spWeapon = spPlayer->GetWeapon().lock();
	if (!spWeapon) { return; }
	std::shared_ptr<PotionTexInfo>spTexInfo = m_spContext->m_wpPotionTexInfo.lock();
	if (!spTexInfo) { return; }

	// 配列の番号 = 種類。武器側も同じ並びなので、番号どおりにコピーする
	for (int i = 0; i < WeaponsStrengtheningSize; i++)
	{
		WeaponsStrengtheningInfo& ui = m_weaponsStrengtheningInfo[i];
		const ::WeaponsStrengtheningInfo& src = spWeapon->GetWeaponsStrengtheningInfo(i);

		// テクスチャは最初の1回だけ読む
		if (!ui.m_spExplanationTex)
		{
			ui.m_spExplanationTex = spTexInfo->GetWeaponsStrengtheningExplanation(i);
			ui.m_spNameTex = spTexInfo->GetWeaponsStrengtheningName(i);
		}

		// データは開くたびに取り直す
		ui.m_lv = src.m_lv;
		ui.m_nextLvNum = src.m_nextLvNum;
		ui.m_strengtheningItemID = src.m_strengtheningItemID;
		ui.m_myNodeID = src.m_myNodeID;
		ui.m_preconditionNodeID = src.m_preconditionNodeID;
		ui.m_preconditionNodeIndex = spWeapon->FindIndexByNodeID(src.m_preconditionNodeID); // -1 = 前提なし
	}

	m_selectIndex = 0;
	m_spContext->m_spSelectExplanationTex = m_weaponsStrengtheningInfo[m_selectIndex].m_spExplanationTex;
	m_spContext->m_spSelectNameTex = m_weaponsStrengtheningInfo[m_selectIndex].m_spNameTex;
	m_spContext->m_num = m_weaponsStrengtheningInfo[m_selectIndex].m_lv;
	m_selectWeaponsStrengtheningNextLVNum = m_weaponsStrengtheningInfo[m_selectIndex].m_nextLvNum;
	m_weaponsStrengtheningInfo[m_selectIndex].m_hit = true;

	WeaponChooseFlg(m_selectIndex);


	WeaponsStrengtheningInfoInitPos();
}

void WeponInventoryUI::WeaponsStrengtheningInfoInitPos()
{

}

void WeponInventoryUI::WeaponsStrengtheningTexInfo()
{
	std::shared_ptr<PlayerBase >spPlayer = m_spContext->m_wpPlayer.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
	if (!spPlayerInventory) { return; }

	Math::Vector2 iconBase2DPos = m_spContext->m_useTex2DPos; // 初期位置
	iconBase2DPos.y += 90;
	for (Inventory weapon : spPlayerInventory->GetWeaponsStrengtheningInventory())
	{
		if (weapon.m_ID == m_weaponsStrengtheningInfo[m_selectIndex].m_strengtheningItemID)
		{
			InventoryItemIconInfo itemIconInfo;
			itemIconInfo.m_itemID = weapon.m_ID;
			itemIconInfo.m_2DPos = iconBase2DPos;
			itemIconInfo.m_num = weapon.m_num;
			itemIconInfo.m_name = weapon.m_name;

			std::shared_ptr<PotionTexInfo>spTexInfo = m_spContext->m_wpPotionTexInfo.lock();
			if (spTexInfo)
			{
				int weaponsStrengtheningItemID = m_weaponsStrengtheningInfo[m_selectIndex].m_strengtheningItemID;
				itemIconInfo.m_spIconTex = spTexInfo->GetWeaponsStrengtheningItemIcon(weaponsStrengtheningItemID);
				itemIconInfo.m_spExplanationTex = spTexInfo->GetWeaponsStrengtheningExplanation(itemIconInfo.m_itemID);
			}

			m_weaponsStrengtheningIconInfo = itemIconInfo;
		}
	}



	std::shared_ptr<WeaponBase> spWeapon = spPlayer->GetWeapon().lock();
	if (spWeapon)
	{
		// 全ノード分、武器側の最新の状態を取り直す
		for (int i = 0; i < WeaponsStrengtheningSize; i++)
		{
			WeaponsStrengtheningInfo& weapon = m_weaponsStrengtheningInfo[i];
			weapon.m_lvMaxFlg = spWeapon->GetWeaponsStrengtheningLVMaxFlg(i);
			weapon.m_lv = spWeapon->GetWeaponsStrengtheningLv(i);
			weapon.m_nextLvNum = spWeapon->GetWeaponsStrengtheningNextLvNum(i);
			weapon.m_strengtheningFlg = spWeapon->IsWeaponsStrengtheningPossible(i);
		}
	}
}

void WeponInventoryUI::WeaponsStrengtheningIconHit()
{
	POINT mousePos = MouseInfo::Instance().m_windowPos;

	for (int i = 0; i < WeaponsStrengtheningSize; i++)
	{
		WeaponsStrengtheningInfo& weapon = m_weaponsStrengtheningInfo[i];

		float left = weapon.m_iconPos.x - m_selectWeaponsStrengtheningIconSize.x / 2;
		float right = weapon.m_iconPos.x + m_selectWeaponsStrengtheningIconSize.x / 2;
		float top = weapon.m_iconPos.y + m_selectWeaponsStrengtheningIconSize.y / 2;
		float bot = weapon.m_iconPos.y - m_selectWeaponsStrengtheningIconSize.y / 2;


		if (mousePos.x >= left && mousePos.x <= right &&
			mousePos.y >= bot && mousePos.y <= top)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			weapon.m_hit = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_spContext->m_spSelectExplanationTex = weapon.m_spExplanationTex;
				m_selectIndex = i;
				m_spContext->m_num = weapon.m_lv;
				m_selectWeaponsStrengtheningNextLVNum = weapon.m_nextLvNum;

				// 前提なし、または前提ノードが解放済みなら通常の名前・説明
				const int preIndex = weapon.m_preconditionNodeIndex;
				if (preIndex < 0 || m_weaponsStrengtheningInfo[preIndex].m_lv > 0)
				{
					m_spContext->m_spSelectNameTex = weapon.m_spNameTex;
					m_spContext->m_spSelectExplanationTex = weapon.m_spExplanationTex;
				}
				else
				{
					m_spContext->m_spSelectNameTex = m_spPreconditionNodeNameTex;
					m_spContext->m_spSelectExplanationTex = m_spPreconditionNodeExplanationTex;
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
	if (!m_spWeaponsStrengtheningButtonTex) { return; }

	std::shared_ptr<PlayerBase>spPlayer = m_spContext->m_wpPlayer.lock();
	if (!spPlayer) { return; }
	std::shared_ptr<WeaponBase>spWeapon = spPlayer->GetWeapon().lock();
	if (!spWeapon) { return; }

	POINT mousePos = MouseInfo::Instance().m_windowPos;

	float left = m_weaponsStrengtheningButtonTex2DPos.x - m_spWeaponsStrengtheningButtonTex->GetWidth() / 2;
	float right = m_weaponsStrengtheningButtonTex2DPos.x + m_spWeaponsStrengtheningButtonTex->GetWidth() / 2;
	float top = m_weaponsStrengtheningButtonTex2DPos.y + m_spWeaponsStrengtheningButtonTex->GetHeight() / 2;
	float bot = m_weaponsStrengtheningButtonTex2DPos.y - m_spWeaponsStrengtheningButtonTex->GetHeight() / 2;

	// 前提ノードが解放されているか（表示の切り替えに使うので毎フレーム更新）
	m_preconditionNodeFlg = !spWeapon->IsPreconditionCleared(m_selectIndex);

	if (mousePos.x >= left && mousePos.x <= right &&
		mousePos.y >= bot && mousePos.y <= top)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{

			// 前提ノードが解放されているか
			if (m_preconditionNodeFlg)
			{

				// 前提ノードがまだ → ぶるっと震わせて、前提のノードを光らせる
				m_spContext->m_useEffectType = UseEffectType::Shake;
				m_spContext->m_useEffectTimer = m_spContext->m_useEffectTime;
				m_spContext->m_useEffectItemID = m_weaponsStrengtheningInfo[m_selectIndex].m_preconditionNodeID;

				// 強化できない音
				{
					std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/Strengthen_Fail.wav", false);
					if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
				}
				return;
			}

			if (spWeapon->IsWeaponsStrengtheningPossible(m_selectIndex))
			{
				spWeapon->AddWeaponsStrengtheningInfo(m_selectIndex);
				m_spContext->m_num = spWeapon->GetWeaponsStrengtheningLv(m_selectIndex);
				m_weaponsStrengtheningInfo[m_selectIndex].m_lv = m_spContext->m_num;


				int weaponsStrengtheningItemID = m_weaponsStrengtheningInfo[m_selectIndex].m_strengtheningItemID;
				spPlayer->GetPlayerInventory()->UseWeaponsStrengtheningInventory(weaponsStrengtheningItemID, m_weaponsStrengtheningInfo[m_selectIndex].m_nextLvNum);
				m_weaponsStrengtheningInfo[m_selectIndex].m_nextLvNum = spWeapon->GetWeaponsStrengtheningNextLvNum(m_selectIndex);
				m_selectWeaponsStrengtheningNextLVNum = m_weaponsStrengtheningInfo[m_selectIndex].m_nextLvNum;

				m_spContext->m_useEffectType = UseEffectType::Bounce;
				m_spContext->m_useEffectTimer = m_spContext->m_useEffectTime;
				m_spContext->m_useEffectItemID = m_weaponsStrengtheningInfo[m_selectIndex].m_myNodeID;

				// 強化できた音（金床）
				{
					std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/Strengthen_Success.wav", false);
					if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
				}
			}
			else if (!m_weaponsStrengtheningInfo[m_selectIndex].m_lvMaxFlg)
			{

				m_spContext->m_useEffectType = UseEffectType::Shake;
				m_spContext->m_useEffectTimer = m_spContext->m_useEffectTime;
				m_spContext->m_useEffectItemID = -999;

				// 素材が足りない音
				{
					std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/Strengthen_Fail.wav", false);
					if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
				}
			}
		}
	}

}

void WeponInventoryUI::WeaponChooseFlg(int _index)
{
	for (int i = 0; i < WeaponsStrengtheningSize; i++)
	{
		m_weaponsStrengtheningInfo[i].m_chooseFlg = (i == _index);
	}
}
