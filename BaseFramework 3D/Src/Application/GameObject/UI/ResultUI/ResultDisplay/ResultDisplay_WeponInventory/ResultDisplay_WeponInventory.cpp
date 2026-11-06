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
	m_spWeapon = std::make_shared<WeaponBase>();
	m_spWeapon->Init();

	if (!m_WeaponsStrengtheningBackTex)
	{
		m_WeaponsStrengtheningBackTex = std::make_shared<KdTexture>();
		m_WeaponsStrengtheningBackTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_PotionInventory/BagBackTex2.png");
		m_WeaponsStrengtheningTexPos = { 200,-70 };

	}

	////////////////////////////////////////////////////////////////////////////
	//武器強化用
	Math::Vector2 weaponsStrengtheningBasePos = {-150,-150 };
	float weaponsStrengtheningOffset = 75.0f;
	float vecL = 75.0f;

	if (!m_selectTex)
	{
		m_selectTex2DPos = {};
		m_selectTex = std::make_shared<KdTexture>();
		m_selectTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/select.png");
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
		Math::Vector2 pos = {};
		pos.y += 150;
		pos.x -= 80;
		m_LVTex2DPos = pos;

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
			m_weaponsStrengtheningInfo[i].Lv = 1;
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

void ResultDisplay_WeponInventory::PreDraw()
{
	
}

void ResultDisplay_WeponInventory::DrawSprite()
{
	Math::Vector2 pos = { -150,-70 };
	Math::Vector2 siz = { 180,368 / 2 };
	Math::Color col = { 0.8,0.8,0.8,1 };
	KdShaderManager::Instance().m_spriteShader.DrawBox(pos.x, pos.y, siz.x, siz.y, &col);

	if (m_WeaponsStrengtheningBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_WeaponsStrengtheningBackTex, m_WeaponsStrengtheningTexPos.x, m_WeaponsStrengtheningTexPos.y);
	}


	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		int x1 = 0;
		int x2 = 0;
		int y1 = 0;
		int y2 = 0;
		switch (i)
		{
		case WeponStrengtheningID::WeaponsStrengthening_Attck:
		case WeponStrengtheningID::WeaponsStrengthening_Stamina:
		case WeponStrengtheningID::WeaponsStrengthening_ChargeTime:
		case WeponStrengtheningID::WeaponsStrengthening_Characteristic:
		case WeponStrengtheningID::WeaponsStrengthening_DefenseIgnore:

			x1 = m_weaponsStrengtheningInfo[i].iconPos.x;
			y1 = m_weaponsStrengtheningInfo[i].iconPos.y;

			x2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].iconPos.x;
			y2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].iconPos.y;

			break;
		case WeponStrengtheningID::WeaponsStrengthening_Characteristic1:
			x1 = m_weaponsStrengtheningInfo[i].iconPos.x;
			y1 = m_weaponsStrengtheningInfo[i].iconPos.y;

			x2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.x;
			y2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.y;
			break;
		case WeponStrengtheningID::WeaponsStrengthening_Characteristic2:
			x1 = m_weaponsStrengtheningInfo[i].iconPos.x;
			y1 = m_weaponsStrengtheningInfo[i].iconPos.y;

			x2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.x;
			y2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.y;
			break;
		case WeponStrengtheningID::WeaponsStrengthening_Characteristic3:
			x1 = m_weaponsStrengtheningInfo[i].iconPos.x;
			y1 = m_weaponsStrengtheningInfo[i].iconPos.y;

			x2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.x;
			y2 = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].iconPos.y;
			break;
		case WeponStrengtheningID::WeaponsStrengtheningSiz:
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
		case WeponStrengtheningID::WeaponsStrengthening_Characteristic1:
		case WeponStrengtheningID::WeaponsStrengthening_Characteristic2:
		case WeponStrengtheningID::WeaponsStrengthening_Characteristic3:

			if (m_weaponsStrengtheningInfo[WeponStrengtheningID::WeaponsStrengthening_Characteristic].Lv == 0)
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

	if (m_selectWeaponsStrengtheningID != WeponStrengtheningID::WeaponsStrengthening_Icon)
	{
		if (m_weaponsStrengtheningIconInfoList[m_selectWeaponsStrengtheningID].m_IconTex)
		{
			{
				KdShaderManager::Instance().m_spriteShader.DrawTex(m_weaponsStrengtheningIconInfoList[m_selectWeaponsStrengtheningID].m_IconTex, m_weaponsStrengtheningIconInfoList[m_selectWeaponsStrengtheningID].m_2DPos.x, m_weaponsStrengtheningIconInfoList[m_selectWeaponsStrengtheningID].m_2DPos.y, 60, 60);
				{
					//現在の持ってるアイテムの数
					Math::Vector2 pos = m_weaponsStrengtheningIconInfoList[m_selectWeaponsStrengtheningID].m_2DPos;
					pos.y -= 45;
					pos.x -= 5;

					Math::Color color = kWhiteColor;
					if (m_weaponsStrengtheningIconInfoList[m_selectWeaponsStrengtheningID].m_num < m_selectWeaponsStrengtheningnextLVNum)
					{
						color = kRedColor;
					}
					NumDraw::GetInstance().Drow(m_weaponsStrengtheningIconInfoList[m_selectWeaponsStrengtheningID].m_num, Aligned::RAligned, pos, color, 1.5);
				}
				{
					//レヴェルアップに必要なアイテムの数
					Math::Vector2 pos = m_weaponsStrengtheningIconInfoList[m_selectWeaponsStrengtheningID].m_2DPos;
					pos.y -= 50;
					pos.x += 5;
					NumDraw::GetInstance().Drow(m_selectWeaponsStrengtheningnextLVNum, Aligned::LAligned, pos, kWhiteColor, 1.5);
				}
			}
		}
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

void ResultDisplay_WeponInventory::WeaponsStrengtheningInfoInit(std::vector<Inventory> _weaponsStrengtheningInventory)
{

	if (!m_spWeapon) { return; }
	std::shared_ptr<PotionTexInfo>_spTexInfo = std::make_shared<PotionTexInfo>();
	m_selectWeaponsStrengtheningID = WeaponsStrengthening_Icon;
	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		if (!m_weaponsStrengtheningInfo[i].m_ExplanationTex)
		{
		
			if (!_spTexInfo) { continue; }
			m_weaponsStrengtheningInfo[i].m_ExplanationTex = _spTexInfo->GetWeaponsStrengtheningExplanation(i);
			m_weaponsStrengtheningInfo[i].m_NameTex = _spTexInfo->GetWeaponsStrengtheningName(i);

			if (!m_spWeapon) { return; }
			m_weaponsStrengtheningInfo[i].nextLvNum = m_spWeapon->GetWeaponsStrengtheningNextLvNum(i);
			m_weaponsStrengtheningInfo[i].strengtheningFlg = m_spWeapon->AddWeaponsStrengtheningInfo(i);
		}
	}


	//m_selectExplanationTex = m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].m_ExplanationTex;
	//m_selectNameTex = m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].m_NameTex;
	//m_num = m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].Lv;
	m_selectWeaponsStrengtheningnextLVNum = m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].nextLvNum;
	m_selectWeaponsStrengtheningID = WeaponsStrengthening_Icon;
	m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].m_hit = true;


	Math::Vector2 m_iconBase2DPos = {}; // 初期位置
	m_iconBase2DPos.y += 90;
	for (Inventory weapon : _weaponsStrengtheningInventory)
	{
		ItemIconInfo itemIconInfo;
		itemIconInfo.m_ItemID = weapon.m_ID;
		itemIconInfo.m_2DPos = m_iconBase2DPos;
		itemIconInfo.m_num = weapon.m_num;
		itemIconInfo.m_name = weapon.m_name;
		
		if (_spTexInfo)
		{
			itemIconInfo.m_IconTex = _spTexInfo->GetWeaponsStrengtheningIcon(itemIconInfo.m_ItemID);
			itemIconInfo.m_ExplanationTex = _spTexInfo->GetWeaponsStrengtheningExplanation(itemIconInfo.m_ItemID);
		}

		m_weaponsStrengtheningIconInfoList.push_back(itemIconInfo);
	}



	/*std::shared_ptr<WeaponBase> spWeapon = spPlayer->GetWeapon().lock();
	if (spWeapon)
	{
		for (auto& weapon : m_weaponsStrengtheningInfo)
		{

			if (weapon.ID == PlayerInventoryUI::WeaponsStrengthening_Icon)
			{
				continue;
			}

			weapon.Lv = spWeapon->GetWeaponsStrengtheningLv(weapon.ID);
			weapon.nextLvNum = spWeapon->GetWeaponsStrengtheningNextLvNum(weapon.ID);
			weapon.strengtheningFlg = spWeapon->IsWeaponsStrengtheningPossible(weapon.ID);
		}
	}*/
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
				//m_selectExplanationTex = weapon.m_ExplanationTex;
				m_selectWeaponsStrengtheningID = weapon.ID;
				//m_num = weapon.Lv;
				//m_selectWeaponsStrengtheningnextLVNum = weapon.nextLvNum;
				//m_selectNameTex = weapon.m_NameTex;
			}
		}
		else
		{
			weapon.m_hit = false;
		}
	}
}



