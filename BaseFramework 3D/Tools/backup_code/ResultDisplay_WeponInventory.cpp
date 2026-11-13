#include "ResultDisplay_WeponInventory.h"
#include"../../../../../Info/NumDraw/NumDraw.h"
#include"../../../../../Scene/SceneManager.h"

#include"../../../../Character/Player/PlayerInventory/PlayerInventory.h"
#include"../../../../Potions/PotionTexInfo/PotionTexInfo.h"

#include"../../../../Weapon/WeaponBase.h"


#include"../../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../../Info/KeyInfo/KeyInfo.h"
void ResultDisplay_WeponInventory::Init()
{
	if (!m_WeaponsStrengtheningBackTex)
	{
		m_WeaponsStrengtheningBackTex = std::make_shared<KdTexture>();
		m_WeaponsStrengtheningBackTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_PotionInventory/BagBackTex2.png");
		m_WeaponsStrengtheningTexPos = { 200,-70 };

	}
	if (!m_WeaponsStrengtheningBack2Tex)
	{
		m_WeaponsStrengtheningBack2Tex = std::make_shared<KdTexture>();
		m_WeaponsStrengtheningBack2Tex->Load("Asset/Textures/GameUI/Result/ResultDisplay_PotionInventory/back.png");
	}

	if (!m_WeaponsStrengtheninTitleTex)
	{
		m_WeaponsStrengtheninTitleTex = std::make_shared<KdTexture>();
		m_WeaponsStrengtheninTitleTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_WeponInventory/OverallResult.png");
		m_WeaponsStrengtheninTitleTexPos = { 0,250 };
	}

	////////////////////////////////////////////////////////////////////////////
	//武器強化用
	Math::Vector2 weaponsStrengtheningBasePos = { -150,-100 };
	float weaponsStrengtheningOffset = 65.0f;
	float vecL = 75.0f;

	m_weaponsStrengtheningButtonTex2DPos = { m_WeaponsStrengtheningTexPos.x, -210 };

	if (!m_PreconditionNodeTex)
	{
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

	if (!m_LVTex)
	{
		m_LVTex = std::make_shared<KdTexture>();
		m_LVTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/LV.png");
		Math::Vector2 pos = { 107,15 };
		m_LVTex2DPos = pos;

	}


	if (!m_LineTex)
	{
		m_LineTex = std::make_shared<KdTexture>();
		m_LineTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Line.png");
	}

	float rad = DirectX::XMConvertToRadians(90.0f); // 0°,90°,180°,270°

	m_selectWeaponsStrengtheningIconSiz = { 32,32 };

	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		Math::Vector2 pos = weaponsStrengtheningBasePos;

		m_weaponsStrengtheningInfo[i].Lv = 0;

		float addY = 50;

		switch (i)
		{
		case ResultDisplay_WeponInventory::WeaponsStrengthening_Attck:
			pos.x -= weaponsStrengtheningOffset * 2;
			pos.y += addY;
			break;
		case ResultDisplay_WeponInventory::WeaponsStrengthening_Stamina:
			pos.x -= weaponsStrengtheningOffset;
			pos.y += addY;
			break;
		case ResultDisplay_WeponInventory::WeaponsStrengthening_ChargeTime:
			pos.x += weaponsStrengtheningOffset * 2;
			pos.y += addY;
			break;
		case ResultDisplay_WeponInventory::WeaponsStrengthening_Characteristic:
			pos.x = weaponsStrengtheningBasePos.x;
			pos.y += addY;
			break;
		case ResultDisplay_WeponInventory::WeaponsStrengthening_DefenseIgnore:
			pos.x += weaponsStrengtheningOffset;
			pos.y += addY;
			break;
		case ResultDisplay_WeponInventory::WeaponsStrengthening_Characteristic1:
			rad = DirectX::XMConvertToRadians(45.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.y + cos(rad) * vecL;
			break;
		case ResultDisplay_WeponInventory::WeaponsStrengthening_Characteristic2:
			rad = DirectX::XMConvertToRadians(0.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.y + cos(rad) * vecL;
			break;
		case ResultDisplay_WeponInventory::WeaponsStrengthening_Characteristic3:
			rad = DirectX::XMConvertToRadians(-45.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.y + cos(rad) * vecL;
			break;
		case ResultDisplay_WeponInventory::WeaponsStrengthening_Icon:
			break;
		case ResultDisplay_WeponInventory::WeaponsStrengtheningSiz:
			break;
		default:
			break;
		}

		m_weaponsStrengtheningInfo[i].ID = i;
		m_weaponsStrengtheningInfo[i].iconPos = pos;
		m_weaponsStrengtheningInfo[i].iconSiz = m_selectWeaponsStrengtheningIconSiz;
		m_weaponsStrengtheningInfo[i].nextLvNum = 0;

		if (!m_weaponsStrengtheningInfo[i].iconTex)
		{
			m_weaponsStrengtheningInfo[i].iconTex = std::make_shared<KdTexture>();
			std::string filePath = "Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon";
			switch (i)
			{
			case ResultDisplay_WeponInventory::WeaponsStrengthening_Characteristic:
			case ResultDisplay_WeponInventory::WeaponsStrengthening_Characteristic1:
			case ResultDisplay_WeponInventory::WeaponsStrengthening_Characteristic2:
			case ResultDisplay_WeponInventory::WeaponsStrengthening_Characteristic3:
			case ResultDisplay_WeponInventory::WeaponsStrengthening_Icon:



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

void ResultDisplay_WeponInventory::Update()
{
	WeaponsStrengtheningIconHit();
}

void ResultDisplay_WeponInventory::PreDraw()
{

}

void ResultDisplay_WeponInventory::DrawSprite()
{
	Math::Vector2 pos = { -150,-70 };

	if (m_WeaponsStrengtheningBack2Tex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_WeaponsStrengtheningBack2Tex, pos.x, pos.y);
	}

	if (m_WeaponsStrengtheningBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_WeaponsStrengtheningBackTex, m_WeaponsStrengtheningTexPos.x, m_WeaponsStrengtheningTexPos.y);
	}

	if (m_WeaponsStrengtheninTitleTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_WeaponsStrengtheninTitleTex, m_WeaponsStrengtheninTitleTexPos.x, m_WeaponsStrengtheninTitleTexPos.y);
	}

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
		float scale = 1;

		int PreconditionNodeID_index = m_weaponsStrengtheningInfo[i].PreconditionNodeID_index;

		if (PreconditionNodeID_index < 0 || PreconditionNodeID_index >= WeaponsStrengtheningSiz)
		{
			break;
		}

		if (m_weaponsStrengtheningInfo[PreconditionNodeID_index].Lv == 0)
		{
			col = { 1,1,1,0.5 };
		}

		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
		if (m_LineTex)
		{
			// 通常の線
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_LineTex, x1, y1, siz.x, siz.y, nullptr, &col, pivo, deg);

			// Lv > 0 なら(強化されてるなら)光らせる
			if (m_weaponsStrengtheningInfo[i].Lv > 0)
			{
				const int   glowNum = 4;      // 重ねる枚数
				const float glowMax = 3.0f;   // 一番外側の太さ倍率

				for (int j = 1; j <= glowNum; j++)
				{
					float t = (float)j / glowNum;           // 0～1
					float scale = 1.0f + t * (glowMax - 1.0f);
					float alpha = 0.4f * (1.0f - t) + 0.05f;

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

		Math::Color iconCol = { 1,1,1,1 };
		float choseSizUp = 1;
		if (m_weaponsStrengtheningInfo[i].ChooseFlg)
		{
			choseSizUp = 1.3;
		}
		// 前提ノードが未解放ならグレー表示
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
				m_weaponsStrengtheningInfo[i].iconSiz.x * scale* choseSizUp,
				m_weaponsStrengtheningInfo[i].iconSiz.y * scale* choseSizUp,
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
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_weaponsStrengtheningInfo[i].iconTex, m_weaponsStrengtheningInfo[i].iconPos.x, m_weaponsStrengtheningInfo[i].iconPos.y, m_weaponsStrengtheningInfo[i].iconSiz.x* choseSizUp, m_weaponsStrengtheningInfo[i].iconSiz.y* choseSizUp, nullptr, &iconCol);

		if (m_PreconditionNodeIconTex && PreconditionNodeIconFlg)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_PreconditionNodeIconTex, m_weaponsStrengtheningInfo[i].iconPos.x, m_weaponsStrengtheningInfo[i].iconPos.y, m_weaponsStrengtheningInfo[i].iconSiz.x* choseSizUp, m_weaponsStrengtheningInfo[i].iconSiz.y* choseSizUp, nullptr, &iconCol);
		}

	}

	// 選択中の強化に使うアイテム（インベントリと同じく strengtheningItem_ID で探す）
	const int itemIndex = FindSelectItemIconIndex();

	if (m_selectWeaponsStrengtheningID != WeponStrengtheningID::WeaponsStrengthening_Icon)
	{
		if (itemIndex >= 0 && m_weaponsStrengtheningIconInfoList[itemIndex].m_IconTex)
		{
			{
				KdShaderManager::Instance().m_spriteShader.DrawTex(m_weaponsStrengtheningIconInfoList[itemIndex].m_IconTex, m_weaponsStrengtheningIconInfoList[itemIndex].m_2DPos.x, m_weaponsStrengtheningIconInfoList[itemIndex].m_2DPos.y, 60, 60);
				{
					//現在の持ってるアイテムの数
					Math::Vector2 pos = m_weaponsStrengtheningIconInfoList[itemIndex].m_2DPos;
					pos.y -= 45;
					pos.x -= 5;

					Math::Color color = kWhiteColor;
					if (m_weaponsStrengtheningIconInfoList[itemIndex].m_num < m_selectWeaponsStrengtheningnextLVNum)
					{
						color = kRedColor;
					}
					NumDraw::GetInstance().Drow(m_weaponsStrengtheningIconInfoList[itemIndex].m_num, Aligned::RAligned, pos, color, 1.5);
				}
				{
					//レヴェルアップに必要なアイテムの数
					Math::Vector2 pos = m_weaponsStrengtheningIconInfoList[itemIndex].m_2DPos;
					pos.y -= 50;
					pos.x += 5;
					NumDraw::GetInstance().Drow(m_selectWeaponsStrengtheningnextLVNum, Aligned::LAligned, pos, kWhiteColor, 1.5);
				}
			}
		}
	}
	

	Math::Vector2 itemInfoPos = { m_WeaponsStrengtheningTexPos.x, -170 };
	if (itemIndex >= 0)
	{
		itemInfoPos = m_weaponsStrengtheningIconInfoList[itemIndex].m_2DPos;
	}

	//説明
	if (m_selectExplanationTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_selectExplanationTex, itemInfoPos.x, itemInfoPos.y + 50);
	}

	//武器の名前
	if (m_selectNameTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_selectNameTex, itemInfoPos.x, itemInfoPos.y + 257);
	}

	//LV
	if (m_LVTex)
	{
		Math::Vector2 pivo = { 0,0.5 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_LVTex, m_LVTex2DPos.x, m_LVTex2DPos.y, nullptr, nullptr, pivo);


		int num = m_weaponsStrengtheningInfo[m_selectWeaponsStrengtheningID].Lv;
		Math::Vector2 pos = m_LVTex2DPos;
		pos.x += 200;
		NumDraw::GetInstance().Drow(num, Aligned::RAligned, pos, kWhiteColor, 4);
	}
}

void ResultDisplay_WeponInventory::WeaponsStrengtheningInfoInit(std::vector<Inventory> _weaponsStrengtheningInventory)
{

	if (!m_spWeapon) { return; }
	std::shared_ptr<PotionTexInfo>_spTexInfo = std::make_shared<PotionTexInfo>();
	m_selectWeaponsStrengtheningID = WeaponsStrengthening_Icon;
	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		if (!m_weaponsStrengtheningInfo[i].m_ExplanationTex && _spTexInfo)
		{
			m_weaponsStrengtheningInfo[i].m_ExplanationTex = _spTexInfo->GetWeaponsStrengtheningExplanation(i);
			m_weaponsStrengtheningInfo[i].m_NameTex = _spTexInfo->GetWeaponsStrengtheningName(i);
		}

		// 配列の番号 = 種類。武器側も同じ並びなので番号どおりにコピーする
		const ::WeaponsStrengtheningInfo& src = m_spWeapon->GetWeaponsStrengtheningInfo(i);
		m_weaponsStrengtheningInfo[i].nextLvNum = src.nextLvNum;
		m_weaponsStrengtheningInfo[i].Lv = src.Lv;
		m_weaponsStrengtheningInfo[i].PreconditionNodeID = src.PreconditionNodeID;
		m_weaponsStrengtheningInfo[i].PreconditionNodeID_index = m_spWeapon->FindIndexByNodeID(src.PreconditionNodeID); // -1 = 前提なし
	}

	m_selectWeaponsStrengtheningnextLVNum = m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].nextLvNum;
	m_selectWeaponsStrengtheningID = WeaponsStrengthening_Icon;
	m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].m_hit = true;

	m_selectExplanationTex = m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].m_ExplanationTex;
	m_selectNameTex = m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].m_NameTex;
	m_PreconditionNodeFlg = false;


	Math::Vector2 m_iconBase2DPos = { m_WeaponsStrengtheningTexPos.x,-170 }; // 初期位置
	for (Inventory weapon : _weaponsStrengtheningInventory)
	{
		ItemIconInfo itemIconInfo;
		itemIconInfo.m_ItemID = weapon.m_ID;
		itemIconInfo.m_2DPos = m_iconBase2DPos;
		itemIconInfo.m_num = weapon.m_num;
		itemIconInfo.m_name = weapon.m_name;

		if (_spTexInfo)
		{
			// インベントリと同じ強化アイテム用の画像を使う
			itemIconInfo.m_IconTex = _spTexInfo->GetWeaponsStrengtheningItemIcon(itemIconInfo.m_ItemID);
			itemIconInfo.m_ExplanationTex = _spTexInfo->GetWeaponsStrengtheningExplanation(itemIconInfo.m_ItemID);
		}

		m_weaponsStrengtheningIconInfoList.push_back(itemIconInfo);
	}

	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		// その強化に使うアイテムの所持数を探す（アイテム一覧は強化の種類と数が違うので番号では引かない）
		const int itemID = m_spWeapon->GetWeaponsStrengtheningInfo(i).strengtheningItem_ID;
		int haveNum = 0;
		for (const ItemIconInfo& item : m_weaponsStrengtheningIconInfoList)
		{
			if (item.m_ItemID == itemID) { haveNum = item.m_num; break; }
		}

		m_weaponsStrengtheningInfo[i].strengtheningFlg = (m_weaponsStrengtheningInfo[i].nextLvNum <= haveNum);
	}

	WeaponChooseFlg(WeaponsStrengthening_Icon);
}

void ResultDisplay_WeponInventory::WeaponsStrengtheningIconHit()
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
				m_selectWeaponsStrengtheningID = weapon.ID;
				m_selectWeaponsStrengtheningnextLVNum = m_weaponsStrengtheningInfo[m_selectWeaponsStrengtheningID].nextLvNum;

				WeaponChooseFlg(weapon.ID);

				// 前提なし、または前提ノードが解放済みなら通常の名前・説明
				const int preIndex = weapon.PreconditionNodeID_index;
				if (preIndex < 0 || m_weaponsStrengtheningInfo[preIndex].Lv > 0)
				{
					m_selectNameTex = weapon.m_NameTex;
					m_selectExplanationTex = weapon.m_ExplanationTex;
					m_PreconditionNodeFlg = false;
				}
				else
				{
					m_selectNameTex = m_PreconditionNodeNameTex;
					m_selectExplanationTex = m_PreconditionNodeExplanationTex;
					m_PreconditionNodeFlg = true;
				}
			}
		}
		else
		{
			weapon.m_hit = false;
		}
	}
}

void ResultDisplay_WeponInventory::WeaponChooseFlg(int _ID)
{
	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		m_weaponsStrengtheningInfo[i].ChooseFlg = false;

		if (i == m_selectWeaponsStrengtheningID)
		{
			m_weaponsStrengtheningInfo[i].ChooseFlg = true;
		}
	}
}

int ResultDisplay_WeponInventory::FindSelectItemIconIndex() const
{
	if (!m_spWeapon) { return -1; }
	if (m_selectWeaponsStrengtheningID < 0 || m_selectWeaponsStrengtheningID >= WeaponsStrengtheningSiz) { return -1; }

	const int itemID = m_spWeapon->GetWeaponsStrengtheningInfo(m_selectWeaponsStrengtheningID).strengtheningItem_ID;
	for (int i = 0; i < static_cast<int>(m_weaponsStrengtheningIconInfoList.size()); ++i)
	{
		if (m_weaponsStrengtheningIconInfoList[i].m_ItemID == itemID) { return i; }
	}
	return -1;
}
