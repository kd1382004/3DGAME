//---- 自分のヘッダー ----
#include "ItemGetUIController.h"							//アイテム入手の表示

//---- ポーション ----
#include"../../Potions/PotionTexInfo/PotionTexInfo.h"		//アイテムの名前・画像情報

//---- 便利機能 ----
#include"../../../Info/NumDraw/NumDraw.h"					//数字の画像描画

//=============================================
// 基本の処理
//=============================================

void ItemGetUIController::Init()
{
	if (!m_spBackTex)
	{
		m_spBackTex = std::make_shared<KdTexture>();
		m_spBackTex->Load("Asset/Textures/GameUI/Item/ItemGetUI/ItemGetUI.png");
		m_basePos = { -640,-50 };

		m_basePos.x += m_spBackTex->GetWidth() / 2;

		float recY = m_spBackTex->GetHeight();
		m_iconWH = { recY,recY };

		float recX = m_spBackTex->GetWidth() / 2;
		m_backWH = { recX ,recY / 2 };
	}

	if (!m_spBack2Tex)
	{
		m_spBack2Tex = std::make_shared<KdTexture>();
		m_spBack2Tex->Load("Asset/Textures/GameUI/Item/ItemGetUI/Get.png");
	}

	if (!m_spPotionTexInfo)
	{
		m_spPotionTexInfo = std::make_shared<PotionTexInfo>();
	}

}

void ItemGetUIController::PreUpdate()
{
	//リストの管理
	auto it = m_getItemList.begin();

	while (it != m_getItemList.end())
	{
		if (it->m_isFinished)	// isFinished ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_getItemList.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}
}

void ItemGetUIController::Update()
{
	float delta = DeltaTime::Instance().GetGameDeltaTime();


	int i = 1;
	for (auto& item : m_getItemList)
	{
		item.m_drawFlg = true;


		//座標決め
		item.m_pos = m_basePos;
		item.m_pos.y -= i * m_iconWH.y;
		item.m_iconPos = item.m_pos;
		item.m_iconPos.x += -m_backWH.x + m_iconWH.x;
		item.m_namePos = item.m_iconPos;
		item.m_namePos.x += m_iconWH.x;

		//表示時間管理
		item.m_showTime -= 0.5 * delta;

		if (item.m_showTime < 0)
		{
			item.m_alpha -= delta;
			if (item.m_alpha <= 0)
			{
				item.m_isFinished = true;
			}
		}

		m_alpha = item.m_alpha;
		i++;
	}

}

void ItemGetUIController::DrawSprite()
{

	if (!m_spBackTex) { return; }

	if (m_getItemList.size() != 0)
	{
		Math::Color color = { 1,1,1,m_alpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spBack2Tex, m_basePos.x, m_basePos.y, nullptr, &color);
	}


	for (const auto& item : m_getItemList)
	{
		if (!item.m_drawFlg) { continue; }


		//背景描画
		Math::Color color = { 1,1,1,item.m_alpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spBackTex, item.m_pos.x, item.m_pos.y, nullptr, &color);

		Math::Vector2 pos = item.m_pos;
		pos.x += 60;
		NumDraw::GetInstance().Draw(item.m_getNum, LAligned, pos, color);

		if (!item.m_spIconTex) { continue; }
		KdShaderManager::Instance().m_spriteShader.DrawTex(item.m_spIconTex, item.m_iconPos.x, item.m_iconPos.y, m_iconWH.x, m_iconWH.y, nullptr, &color);

		if (!item.m_spNameTex) { continue; }
		Math::Vector2 nameSize = {};
		Math::Vector2 nameNowSize = { (float)item.m_spNameTex->GetWidth() , (float)item.m_spNameTex->GetHeight() };
		Math::Vector2 pivot = { 0,0.5 };

		float a = m_iconWH.y / nameNowSize.y;
		nameSize = nameNowSize * a;
		KdShaderManager::Instance().m_spriteShader.DrawTex(item.m_spNameTex, item.m_namePos.x, item.m_namePos.y, nameSize.x, nameSize.y, nullptr, &color, pivot);
	}

}

//=============================================
// 設定
//=============================================

void ItemGetUIController::AddGetItemList(GetItem _item, ItemType _type)
{

	if (!m_spPotionTexInfo) { return; }


	switch (_type)
	{
	case Potion:

		_item.m_spIconTex = m_spPotionTexInfo->GetPotionIcon(_item.m_ID);
		_item.m_spNameTex = m_spPotionTexInfo->GetPotionName(_item.m_ID);
		break;
	case WeaponsStrengthening:
		_item.m_spIconTex = m_spPotionTexInfo->GetWeaponsStrengtheningItemIcon(_item.m_ID);
		_item.m_spNameTex = m_spPotionTexInfo->GetWeaponsStrengtheningItemName(_item.m_ID);
		break;
	default:
		break;
	}
	m_getItemList.push_back(_item);
}
