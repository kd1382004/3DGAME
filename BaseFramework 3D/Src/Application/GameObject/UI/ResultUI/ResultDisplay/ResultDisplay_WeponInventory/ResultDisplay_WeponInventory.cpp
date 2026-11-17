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
	if (!m_spWeaponsStrengtheningBackTex)
	{
		m_spWeaponsStrengtheningBackTex = std::make_shared<KdTexture>();
		m_spWeaponsStrengtheningBackTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_PotionInventory/BagBackTex2.png");
		m_weaponsStrengtheningTexPos = { 200,-70 };

	}
	if (!m_spWeaponsStrengtheningBack2Tex)
	{
		m_spWeaponsStrengtheningBack2Tex = std::make_shared<KdTexture>();
		m_spWeaponsStrengtheningBack2Tex->Load("Asset/Textures/GameUI/Result/ResultDisplay_PotionInventory/back.png");
	}

	if (!m_spWeaponsStrengtheningTitleTex)
	{
		m_spWeaponsStrengtheningTitleTex = std::make_shared<KdTexture>();
		m_spWeaponsStrengtheningTitleTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_WeponInventory/OverallResult.png");
		m_weaponsStrengtheningTitleTexPos = { 0,250 };
	}

	////////////////////////////////////////////////////////////////////////////
	//武器強化用
	Math::Vector2 weaponsStrengtheningBasePos = { -150,-100 };

	//インベントリ(PlayerInventoryUI)と同じ並べ方。リザルトは画面が狭いので全体を縮小する（1にするとインベントリと同じ大きさ）
	const float layoutScale = 0.5f;
	float weaponsStrengtheningOffset = 150.0f * layoutScale;
	float vecL = 150.0f * layoutScale;

	m_weaponsStrengtheningButtonTex2DPos = { m_weaponsStrengtheningTexPos.x, -210 };

	if (!m_spPreconditionNodeTex)
	{
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

	if (!m_spLVTex)
	{
		m_spLVTex = std::make_shared<KdTexture>();
		m_spLVTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/LV.png");
		Math::Vector2 pos = { 112,11 };	// 右の枠の上の暗い帯（数字の枠）の左端・真ん中
		m_LVTex2DPos = pos;

	}


	if (!m_spLineTex)
	{
		m_spLineTex = std::make_shared<KdTexture>();
		m_spLineTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Line.png");
	}

	float rad = DirectX::XMConvertToRadians(90.0f); // 0°,90°,180°,270°

	m_selectWeaponsStrengtheningIconSiz = { 64 * layoutScale,64 * layoutScale };

	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		Math::Vector2 pos = weaponsStrengtheningBasePos;

		m_weaponsStrengtheningInfo[i].m_lv = 0;

		float addY = 100 * layoutScale;

		switch (i)
		{
		case ResultDisplay_WeponInventory::WeaponsStrengthening_Attack:
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
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].m_iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].m_iconPos.y + cos(rad) * vecL;
			break;
		case ResultDisplay_WeponInventory::WeaponsStrengthening_Characteristic2:
			rad = DirectX::XMConvertToRadians(0.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].m_iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].m_iconPos.y + cos(rad) * vecL;
			break;
		case ResultDisplay_WeponInventory::WeaponsStrengthening_Characteristic3:
			rad = DirectX::XMConvertToRadians(-45.0f);
			pos.x = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].m_iconPos.x + sin(rad) * vecL;
			pos.y = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].m_iconPos.y + cos(rad) * vecL;
			break;
		case ResultDisplay_WeponInventory::WeaponsStrengthening_Icon:
			break;
		case ResultDisplay_WeponInventory::WeaponsStrengtheningSiz:
			break;
		default:
			break;
		}

		m_weaponsStrengtheningInfo[i].m_ID = i;
		m_weaponsStrengtheningInfo[i].m_iconPos = pos;
		m_weaponsStrengtheningInfo[i].m_iconSiz = m_selectWeaponsStrengtheningIconSiz;
		m_weaponsStrengtheningInfo[i].m_nextLvNum = 0;

		if (!m_weaponsStrengtheningInfo[i].m_spIconTex)
		{
			m_weaponsStrengtheningInfo[i].m_spIconTex = std::make_shared<KdTexture>();
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
			m_weaponsStrengtheningInfo[i].m_spIconTex->Load(filePath);
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

	if (m_spWeaponsStrengtheningBack2Tex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spWeaponsStrengtheningBack2Tex, pos.x, pos.y);
	}

	if (m_spWeaponsStrengtheningBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spWeaponsStrengtheningBackTex, m_weaponsStrengtheningTexPos.x, m_weaponsStrengtheningTexPos.y);
	}

	if (m_spWeaponsStrengtheningTitleTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spWeaponsStrengtheningTitleTex, m_weaponsStrengtheningTitleTexPos.x, m_weaponsStrengtheningTitleTexPos.y);
	}

	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		int x1 = 0;
		int x2 = 0;
		int y1 = 0;
		int y2 = 0;
		int prIndex = m_weaponsStrengtheningInfo[i].m_preconditionNodeIndex;
		if (prIndex < 0) { continue; } // 前提なし（根元）は線を引かない

		x1 = m_weaponsStrengtheningInfo[i].m_iconPos.x;
		y1 = m_weaponsStrengtheningInfo[i].m_iconPos.y;

		x2 = m_weaponsStrengtheningInfo[prIndex].m_iconPos.x;
		y2 = m_weaponsStrengtheningInfo[prIndex].m_iconPos.y;


		float dx = x2 - x1;
		float dy = y2 - y1;

		// ラジアン
		float rad = atan2f(dy, dx);

		// 度数法（-180 ～ +180）
		float deg = rad * 180.0f / 3.14159265f;


		Math::Vector2 pivot = { 0,0.5 };
		Math::Vector2 siz = { 0,20 };
		siz.x = sqrtf(dx * dx + dy * dy);
		Math::Color col = kWhiteColor;
		float defaultScale = 0.8;
		float scale = 1;

		int preconditionNodeIDIndex = m_weaponsStrengtheningInfo[i].m_preconditionNodeIndex;

		if (preconditionNodeIDIndex < 0 || preconditionNodeIDIndex >= WeaponsStrengtheningSiz)
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
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spLineTex, x1, y1, siz.x, siz.y * defaultScale, nullptr, &col, pivot, deg);

			// Lv > 0 なら(強化されてるなら)光らせる
			if (m_weaponsStrengtheningInfo[i].m_lv > 0)
			{
				defaultScale = 1;
				const int   glowNum = 4;      // 重ねる枚数
				const float glowMax = 3.0f;   // 一番外側の太さ倍率

				for (int j = 1; j <= glowNum; j++)
				{
					float t = (float)j / glowNum;           // 0～1
					float scale = 1.0f + t * (glowMax - 1.0f);
					float alpha = 0.4f * (1.0f - t) + 0.05f;
					scale *= defaultScale;
					Math::Color glowCol = { col.x, col.y, col.z, col.w * alpha };

					KdShaderManager::Instance().m_spriteShader.DrawTex(m_spLineTex, x1, y1, siz.x, siz.y * scale, nullptr, &glowCol, pivot, deg);
				}
			}
		}
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
	}

	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		if (!m_weaponsStrengtheningInfo[i].m_spIconTex) { continue; }
		bool preconditionNodeIconFlg = false;

		Math::Color iconCol = { 1,1,1,1 };
		float choseSizUp = 1;
		if (m_weaponsStrengtheningInfo[i].m_chooseFlg)
		{
			choseSizUp = 1.3;
		}
		// 前提ノードが未解放ならグレー表示
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



		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
		// 強化が可能なら光らす


		// 時間経過による点滅（パルス）アニメーション
		m_weaponsStrengtheningInfo[i].m_timer += 0.01f;

		float pulse = (sinf(m_weaponsStrengtheningInfo[i].m_timer) + 1.0f) * 0.5f;

		// ★ 大きくなって小さくなるスケール
		float scale = 1.0f + pulse * 0.25f;

		// 青(0,0,1) → 白(1,1,1) → 青(0,0,1)
		float r = pulse;
		float g = pulse;
		float b = 1.0f;

		float alpha = 1;

		Math::Color auraColor = { r, g, b, alpha };
		if (m_weaponsStrengtheningInfo[i].m_strengtheningFlg && !m_weaponsStrengtheningInfo[i].m_lvMaxFlg)
		{
			// 発光オーラ描画
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_weaponsStrengtheningInfo[i].m_spIconTex,
				m_weaponsStrengtheningInfo[i].m_iconPos.x,
				m_weaponsStrengtheningInfo[i].m_iconPos.y,
				m_weaponsStrengtheningInfo[i].m_iconSiz.x * scale * choseSizUp,
				m_weaponsStrengtheningInfo[i].m_iconSiz.y * scale * choseSizUp,
				nullptr,
				&auraColor
			);
		}


		if (m_weaponsStrengtheningInfo[i].m_timer > 1)
		{
			m_weaponsStrengtheningInfo[i].m_timer -= 1;
		}
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);

		// 通常のアイコン描画
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_weaponsStrengtheningInfo[i].m_spIconTex, m_weaponsStrengtheningInfo[i].m_iconPos.x, m_weaponsStrengtheningInfo[i].m_iconPos.y, m_weaponsStrengtheningInfo[i].m_iconSiz.x * choseSizUp, m_weaponsStrengtheningInfo[i].m_iconSiz.y * choseSizUp, nullptr, &iconCol);

		if (m_spPreconditionNodeIconTex && preconditionNodeIconFlg)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPreconditionNodeIconTex, m_weaponsStrengtheningInfo[i].m_iconPos.x, m_weaponsStrengtheningInfo[i].m_iconPos.y, m_weaponsStrengtheningInfo[i].m_iconSiz.x * choseSizUp, m_weaponsStrengtheningInfo[i].m_iconSiz.y * choseSizUp, nullptr, &iconCol);
		}

	}

	// 選択中の強化に使うアイテム（インベントリと同じく strengtheningItem_ID で探す）
	const int itemIndex = FindSelectItemIconIndex();

	if (m_selectWeaponsStrengtheningID != WeaponStrengtheningID::WeaponsStrengthening_Icon)
	{
		if (itemIndex >= 0 && m_weaponsStrengtheningIconInfoList[itemIndex].m_spIconTex)
		{
			{
				KdShaderManager::Instance().m_spriteShader.DrawTex(m_weaponsStrengtheningIconInfoList[itemIndex].m_spIconTex, m_weaponsStrengtheningIconInfoList[itemIndex].m_2DPos.x, m_weaponsStrengtheningIconInfoList[itemIndex].m_2DPos.y, 60, 60);
				{
					//現在の持ってるアイテムの数
					Math::Vector2 pos = m_weaponsStrengtheningIconInfoList[itemIndex].m_2DPos;
					pos.y -= 45;
					pos.x -= 5;

					Math::Color color = kWhiteColor;
					if (m_weaponsStrengtheningIconInfoList[itemIndex].m_num < m_selectWeaponsStrengtheningNextLVNum)
					{
						color = kRedColor;
					}
					// アイコン(60px)の半分の幅に収める
					NumDraw::GetInstance().Draw(m_weaponsStrengtheningIconInfoList[itemIndex].m_num, Aligned::RAligned, pos, color, 1.5, false, -1, { 30.0f, 0.0f });
				}
				{
					//レヴェルアップに必要なアイテムの数
					Math::Vector2 pos = m_weaponsStrengtheningIconInfoList[itemIndex].m_2DPos;
					pos.y -= 50;
					pos.x += 5;
					// アイコン(60px)の半分の幅に収める
					NumDraw::GetInstance().Draw(m_selectWeaponsStrengtheningNextLVNum, Aligned::LAligned, pos, kWhiteColor, 1.5, false, -1, { 30.0f, 0.0f });
				}
			}
		}
	}


	Math::Vector2 itemInfoPos = { m_weaponsStrengtheningTexPos.x, -170 };
	if (itemIndex >= 0)
	{
		itemInfoPos = m_weaponsStrengtheningIconInfoList[itemIndex].m_2DPos;
	}

	//説明
	if (m_spSelectExplanationTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spSelectExplanationTex, itemInfoPos.x, itemInfoPos.y + 50 + 25);
	}

	//武器の名前
	if (m_spSelectNameTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spSelectNameTex, itemInfoPos.x, itemInfoPos.y + 257);
	}

	//LV
	if (m_spLVTex)
	{
		Math::Vector2 pivot = { 0,0.5 };
		// 数字の枠に収まるよう、数字と同じ高さ35に縮める
		const float lvH = 35.0f;
		const float lvW = m_spLVTex->GetWidth() * lvH / m_spLVTex->GetHeight();
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spLVTex, m_LVTex2DPos.x, m_LVTex2DPos.y, lvW, lvH, nullptr, nullptr, pivot);


		int num = m_weaponsStrengtheningInfo[m_selectWeaponsStrengtheningID].m_lv;
		Math::Vector2 pos = m_LVTex2DPos;
		pos.x = 285;	// 枠の右端より少し内側に右揃え
		NumDraw::GetInstance().Draw(num, Aligned::RAligned, pos, kWhiteColor, 2.5f);
	}
}

void ResultDisplay_WeponInventory::WeaponsStrengtheningInfoInit(std::vector<Inventory> _weaponsStrengtheningInventory)
{

	if (!m_spWeapon) { return; }
	std::shared_ptr<PotionTexInfo>spTexInfo = std::make_shared<PotionTexInfo>();
	m_selectWeaponsStrengtheningID = WeaponsStrengthening_Icon;
	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		if (!m_weaponsStrengtheningInfo[i].m_spExplanationTex && spTexInfo)
		{
			m_weaponsStrengtheningInfo[i].m_spExplanationTex = spTexInfo->GetWeaponsStrengtheningExplanation(i);
			m_weaponsStrengtheningInfo[i].m_spNameTex = spTexInfo->GetWeaponsStrengtheningName(i);
		}

		// 配列の番号 = 種類。武器側も同じ並びなので番号どおりにコピーする
		const ::WeaponsStrengtheningInfo& src = m_spWeapon->GetWeaponsStrengtheningInfo(i);
		m_weaponsStrengtheningInfo[i].m_nextLvNum = src.m_nextLvNum;
		m_weaponsStrengtheningInfo[i].m_lv = src.m_lv;
		m_weaponsStrengtheningInfo[i].m_preconditionNodeID = src.m_preconditionNodeID;
		m_weaponsStrengtheningInfo[i].m_preconditionNodeIndex = m_spWeapon->FindIndexByNodeID(src.m_preconditionNodeID); // -1 = 前提なし
		m_weaponsStrengtheningInfo[i].m_lvMaxFlg = m_spWeapon->GetWeaponsStrengtheningLVMaxFlg(i);
	}

	m_selectWeaponsStrengtheningNextLVNum = m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].m_nextLvNum;
	m_selectWeaponsStrengtheningID = WeaponsStrengthening_Icon;
	m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].m_hit = true;

	m_spSelectExplanationTex = m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].m_spExplanationTex;
	m_spSelectNameTex = m_weaponsStrengtheningInfo[WeaponsStrengthening_Icon].m_spNameTex;
	m_preconditionNodeFlg = false;


	Math::Vector2 iconBase2DPos = { m_weaponsStrengtheningTexPos.x,-170 }; // 初期位置
	for (Inventory weapon : _weaponsStrengtheningInventory)
	{
		ItemIconInfo itemIconInfo;
		itemIconInfo.m_itemID = weapon.m_ID;
		itemIconInfo.m_2DPos = iconBase2DPos;
		itemIconInfo.m_num = weapon.m_num;
		itemIconInfo.m_name = weapon.m_name;

		if (spTexInfo)
		{
			// インベントリと同じ強化アイテム用の画像を使う
			itemIconInfo.m_spIconTex = spTexInfo->GetWeaponsStrengtheningItemIcon(itemIconInfo.m_itemID);
			itemIconInfo.m_spExplanationTex = spTexInfo->GetWeaponsStrengtheningExplanation(itemIconInfo.m_itemID);
		}

		m_weaponsStrengtheningIconInfoList.push_back(itemIconInfo);
	}

	for (int i = 0; i < WeaponsStrengtheningSiz; i++)
	{
		// その強化に使うアイテムの所持数を探す（アイテム一覧は強化の種類と数が違うので番号では引かない）
		const int itemID = m_spWeapon->GetWeaponsStrengtheningInfo(i).m_strengtheningItemID;
		int haveNum = 0;
		for (const ItemIconInfo& item : m_weaponsStrengtheningIconInfoList)
		{
			if (item.m_itemID == itemID) { haveNum = item.m_num; break; }
		}

		m_weaponsStrengtheningInfo[i].m_strengtheningFlg = (m_weaponsStrengtheningInfo[i].m_nextLvNum <= haveNum);
	}

	WeaponChooseFlg(WeaponsStrengthening_Icon);
}

void ResultDisplay_WeponInventory::WeaponsStrengtheningIconHit()
{
	POINT mousePos = MouseInfo::Instance().m_windowPos;

	for (auto& weapon : m_weaponsStrengtheningInfo)
	{
		float left = weapon.m_iconPos.x - m_selectWeaponsStrengtheningIconSiz.x / 2;
		float right = weapon.m_iconPos.x + m_selectWeaponsStrengtheningIconSiz.x / 2;
		float top = weapon.m_iconPos.y + m_selectWeaponsStrengtheningIconSiz.y / 2;
		float bot = weapon.m_iconPos.y - m_selectWeaponsStrengtheningIconSiz.y / 2;


		if (mousePos.x >= left && mousePos.x <= right &&
			mousePos.y >= bot && mousePos.y <= top)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			weapon.m_hit = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_selectWeaponsStrengtheningID = weapon.m_ID;
				m_selectWeaponsStrengtheningNextLVNum = m_weaponsStrengtheningInfo[m_selectWeaponsStrengtheningID].m_nextLvNum;

				WeaponChooseFlg(weapon.m_ID);

				// 前提なし、または前提ノードが解放済みなら通常の名前・説明
				const int preIndex = weapon.m_preconditionNodeIndex;
				if (preIndex < 0 || m_weaponsStrengtheningInfo[preIndex].m_lv > 0)
				{
					m_spSelectNameTex = weapon.m_spNameTex;
					m_spSelectExplanationTex = weapon.m_spExplanationTex;
					m_preconditionNodeFlg = false;
				}
				else
				{
					m_spSelectNameTex = m_spPreconditionNodeNameTex;
					m_spSelectExplanationTex = m_spPreconditionNodeExplanationTex;
					m_preconditionNodeFlg = true;
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
		m_weaponsStrengtheningInfo[i].m_chooseFlg = false;

		if (i == m_selectWeaponsStrengtheningID)
		{
			m_weaponsStrengtheningInfo[i].m_chooseFlg = true;
		}
	}
}

int ResultDisplay_WeponInventory::FindSelectItemIconIndex() const
{
	if (!m_spWeapon) { return -1; }
	if (m_selectWeaponsStrengtheningID < 0 || m_selectWeaponsStrengtheningID >= WeaponsStrengtheningSiz) { return -1; }

	const int itemID = m_spWeapon->GetWeaponsStrengtheningInfo(m_selectWeaponsStrengtheningID).m_strengtheningItemID;
	for (int i = 0; i < static_cast<int>(m_weaponsStrengtheningIconInfoList.size()); ++i)
	{
		if (m_weaponsStrengtheningIconInfoList[i].m_itemID == itemID) { return i; }
	}
	return -1;
}
