#include "ItemGetUIController.h"
#include"../../../Info/NumDraw/NumDraw.h"

#include"../../Potions/PotionTexInfo/PotionTexInfo.h"
void ItemGetUIController::Init()
{
	if (!m_backTex)
	{
		m_backTex = std::make_shared<KdTexture>();
		m_backTex->Load("Asset/Textures/GameUI/Item/ItemGetUI/ItemGetUI.png");
		m_basePos = { -500,-50 };

		float recY = m_backTex->GetHeight();
		m_IconWH = { recY,recY };

		float recX = m_backTex->GetWidth()/2;
		m__backWH = { recX ,recY/2 };
	}

	if (!m_back2Tex)
	{
		m_back2Tex = std::make_shared<KdTexture>();
		m_back2Tex->Load("Asset/Textures/GameUI/Item/ItemGetUI/Get.png");
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
		if (it->isFinished)	// isFinished ・・・ 無効ならtrue
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
	float delt = DeltaTime::Instance().GetGameDeltaTime();


	int i = 1;
	for (auto& item : m_getItemList)
	{
		item.drawFlg = true;


		//座標決め
		item.m_pos = m_basePos;
		item.m_pos.y -= i * m_IconWH.y;
		item.m_iconPos = item.m_pos;
		item.m_iconPos.x += -m__backWH.x + m_IconWH.x;

		//表示時間管理
		item.showTime -= delt;

		if (item.showTime < 0)
		{
			item.alpha -= delt;
			if (item.alpha <= 0)
			{
				item.isFinished = true;
			}
		}

		m_alpha = item.alpha;
		i++;
	}

}

void ItemGetUIController::DrawSprite()
{

	if (!m_backTex) { return; }

	if (m_getItemList.size() != 0)
	{
		Math::Color color = { 1,1,1,m_alpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_back2Tex, m_basePos.x, m_basePos.y, nullptr, &color);
	}


	for (auto item : m_getItemList)
	{
		if (!item.drawFlg) { continue; }


		//背景描画
		Math::Color color = { 1,1,1,item.alpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_backTex, item.m_pos.x, item.m_pos.y, nullptr, &color);

		Math::Vector2 pos = item.m_pos;
		pos.x += 60;
		NumDraw::GetInstance().Drow(item.GetNum, LAligned, pos, color);

		if (!item.IconTex) { continue; }
		KdShaderManager::Instance().m_spriteShader.DrawTex(item.IconTex, item.m_iconPos.x, item.m_iconPos.y, m_IconWH.x, m_IconWH.y, nullptr, &color);
	}

}

void ItemGetUIController::AddGetItemList(GetItem Item)
{

	if (!m_spPotionTexInfo) { return; }

	Item.IconTex = m_spPotionTexInfo->GetIcon(Item.ID);
	m_getItemList.push_back(Item);
}
