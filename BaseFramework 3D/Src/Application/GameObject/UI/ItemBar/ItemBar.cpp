//---- 自分のヘッダー ----
#include "ItemBar.h"													//アイテムバー

//---- プレイヤー ----
#include"../../Character/Player/PlayerBase.h"							//プレイヤー
#include"../../Character/Player/PlayerInventory/PlayerInventory.h"		//プレイヤーの所持品

//---- ポーション ----
#include"../../Potions/PotionUseController.h"							//ポーションの使用処理
#include"../../Potions/PotionTexInfo/PotionTexInfo.h"					//アイテムの名前・画像情報

//---- UI ----
#include"../PlayerInventoryUI/PlayerInventoryUI.h"						//インベントリ画面

//---- 便利機能 ----
#include"../../../Info/KeyInfo/KeyInfo.h"								//キー入力
#include"../../../Info/MouseInfo/MouseInfo.h"							//マウス入力
#include"../../../Info/DeltaTime/DeltaTime.h"							//デルタタイム
#include"../../../Info/NumDraw/NumDraw.h"								//数字の画像描画

//=============================================
// 基本の処理
//=============================================

void ItemBar::Init()
{
	// 使うキーを有効にする
	KeyInfo::Instance().SetKeyValid(kUseKey);

	// バーの中心位置
	m_2DPos = { 500, -300 };

	// 描画優先度(小さいほど手前)
	// 他のUI(INT_MAX)より手前、インベントリ(1)より奥
	m_layerPriority = 100;

	// 画像の読み込み(無くても動くように、無ければnullptrのまま)
	if (!m_spBackTex)	{ m_spBackTex = LoadTex("Asset/Textures/GameUI/ItemBar/ItemBarBack.png"); }
	if (!m_spSlotTex)	{ m_spSlotTex = LoadTex("Asset/Textures/GameUI/ItemBar/ItemBarSlot.png"); }
	if (!m_spSelectTex) { m_spSelectTex = LoadTex("Asset/Textures/GameUI/ItemBar/ItemBarSelect.png"); }
}

void ItemBar::Update()
{
	MakeItemList();

	if (!IsInputBlocked())
	{
		SelectUpdate();
		UseUpdate();
	}

	EffectUpdate();
}

void ItemBar::DrawSprite()
{
	// インベントリを開いている・倒れているときは出さない
	if (IsInputBlocked()) { return; }

	// はねる演出の倍率(0 → 1.15 → 1)
	float pop = 1.0f;
	if (m_popTimer > 0.0f)
	{
		pop = 1.0f + 0.15f * std::sin((m_popTimer / kPopTime) * DirectX::XM_PI);
	}

	// 帯
	if (m_spBackTex)
	{
		// 大きさはアイコンの間隔と大きさから決める(数値を変えても帯が合うように)
		const float backW = m_slotSpacing * (kSideNum * 2 + 1) + m_slotSize;
		const float backH = m_selectSize * 0.9f;
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spBackTex, (int)m_2DPos.x, (int)m_2DPos.y,
			(int)backW, (int)backH);
	}

	const int num = (int)m_itemIDList.size();
	const int selectIndex = GetSelectIndex();

	// アイコン(左右 → 中央の順に描くと、中央が上に重なる)
	if (num > 0 && selectIndex >= 0)
	{
		// アイテムが少ないときに同じものが二重に出ないよう、左右に出す数を絞る
		const int left = std::min(kSideNum, (num - 1) / 2);
		const int right = std::min(kSideNum, num - 1 - left);

		// 端の外にも1つ描いておくと、スライドで入ってくる分が途切れない
		const bool canExtra = (num > left + right + 1);
		const int kMin = canExtra ? -(left + 1) : -left;
		const int kMax = canExtra ? (right + 1) : right;

		// 描く順番(中央から遠い順。中央が一番上に重なる)
		std::vector<int> drawOrder;
		for (int k = kMin; k <= kMax; k++) { drawOrder.push_back(k); }
		std::sort(drawOrder.begin(), drawOrder.end(),
			[](int _a, int _b) { return std::abs(_a) > std::abs(_b); });

		for (int k : drawOrder)
		{
			const int index = ((selectIndex + k) % num + num) % num;

			// 中央からの距離(スライド中は小数になる)
			const float t = std::abs(k + m_slideOffset);
			const float x = m_2DPos.x + (k + m_slideOffset) * m_slotSpacing;

			// 大きさ:中央は大きく、離れると小さく
			float size = m_selectSize + (m_slotSize - m_selectSize) * std::min(t, 1.0f);
			if (k == 0) { size *= pop; }

			// 不透明度:離れるほど薄く、左右に出す数を超えたら消す
			float alpha = 1.0f - 0.2f * std::min(t, (float)kSideNum);
			if (t > kSideNum) { alpha *= std::clamp(kSideNum + 1.0f - t, 0.0f, 1.0f); }
			if (alpha <= 0.0f) { continue; }

			DrawSlot(m_itemIDList[index], x, size, alpha, k == 0);
		}
	}

	// 中央の選択枠(動かさずに固定)
	if (m_spSelectTex)
	{
		const float frameSize = m_selectSize * 1.4f * pop;
		const Math::Color frameColor = { 1, 1, 1, (num > 0) ? 1.0f : 0.6f };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spSelectTex, (int)m_2DPos.x, (int)m_2DPos.y,
			(int)frameSize, (int)frameSize, nullptr, &frameColor);
	}

	if (m_selectID < 0) { return; }

	//使うキー(選択枠のすぐ下に出す)
	{
		// キーの大きさはアイコンに合わせる
		const float keySize = m_selectSize * 0.6f;

		// 選択枠の下端に、キーの上端を合わせる(pivotを上にする)
		const Math::Vector2 keyPos = { m_2DPos.x, m_2DPos.y - m_selectSize * 0.7f };
		KeyInfo::Instance().Draw2Dkey(kUseKey, keyPos, { keySize, keySize }, 1.0f, { 0.5f, 1.0f });
	}


	// 個数(中央の右下)
	std::shared_ptr<PlayerBase> spPlayer = m_wpPlayer.lock();
	if (spPlayer && spPlayer->GetPlayerInventory())
	{
		const int count = spPlayer->GetPlayerInventory()->GetPotionsInventoryNum(m_selectID);
		// 数字の大きさはアイコンに合わせる(元の数字は1文字9×14。アイコン32なら1倍)
		const float numSize = std::max(1.0f, std::round(m_selectSize / 32.0f));
		NumDraw::GetInstance().Draw(count, RAligned,
			{ m_2DPos.x + m_selectSize * 0.65f, m_2DPos.y - m_selectSize * 0.45f },
			kWhiteColor, numSize);
	}

	// 名前(バーの上)
	std::shared_ptr<PotionTexInfo> spTexInfo = m_wpPotionTexInfo.lock();
	if (spTexInfo)
	{
		std::shared_ptr<KdTexture> spNameTex = spTexInfo->GetPotionName(m_selectID);
		if (spNameTex)
		{
			// 名前画像(200×53)はバーより大きいので、アイコンの大きさに合わせて縮める
			const float nameScale = m_selectSize / 64.0f;
			const float nameW = spNameTex->GetWidth() * nameScale;
			const float nameH = spNameTex->GetHeight() * nameScale;

			// 選択枠のすぐ上に置く
			const float nameY = m_2DPos.y + m_selectSize * 0.7f + nameH * 0.5f;
			KdShaderManager::Instance().m_spriteShader.DrawTex(spNameTex, (int)m_2DPos.x, (int)nameY,
				(int)nameW, (int)nameH);
		}
	}
}

//=============================================
// その他の処理
//=============================================

void ItemBar::MakeItemList()
{
	// 作り直す前に、今何番目を選んでいるか覚えておく
	int oldIndex = GetSelectIndex();

	m_itemIDList.clear();

	std::shared_ptr<PlayerBase> spPlayer = m_wpPlayer.lock();
	if (!spPlayer) { m_selectID = -1; return; }

	std::shared_ptr<PlayerInventory> spInventory = spPlayer->GetPlayerInventory();
	if (!spInventory) { m_selectID = -1; return; }

	for (const Inventory& potion : spInventory->GetPpotionsItemBarInventoryList())
	{
		if (potion.m_num > 0)
		{
			m_itemIDList.push_back(potion.m_ID);
		}
	}

	// 1個もなければ選択なし
	if (m_itemIDList.empty())
	{
		m_selectID = -1;
		return;
	}

	// 今選んでいるものが一覧にあれば、そのまま
	if (std::find(m_itemIDList.begin(), m_itemIDList.end(), m_selectID) != m_itemIDList.end())
	{
		return;
	}

	// なければ前の位置に近いものを選ぶ(初回は先頭)
	int index = std::clamp(oldIndex, 0, (int)m_itemIDList.size() - 1);
	m_selectID = m_itemIDList[index];
}

void ItemBar::SelectUpdate()
{
	const int num = (int)m_itemIDList.size();
	if (num <= 1) { return; }

	int index = GetSelectIndex();
	if (index < 0) { return; }

	// 1ノッチで±120。毎フレーム0に戻るのでそのまま使える
	const int wheel = MouseInfo::Instance().GetMouseWheelValue();
	if (wheel == 0) { return; }

	if (wheel > 0)
	{
		// 上 → 左のアイテム。並びは右へ流れる
		index--;
		m_slideOffset = std::clamp(m_slideOffset - 1.0f, -2.0f, 2.0f);
	}
	else
	{
		// 下 → 右のアイテム。並びは左へ流れる
		index++;
		m_slideOffset = std::clamp(m_slideOffset + 1.0f, -2.0f, 2.0f);
	}

	// 端まで行ったら反対側に回る
	index = (index % num + num) % num;
	m_selectID = m_itemIDList[index];

	MouseInfo::Instance().SEMouseClickPlay();
}

void ItemBar::UseUpdate()
{
	// 押した瞬間だけ(長押しで連続して使わない)
	if (!KeyInfo::Instance().GetValidKeyPush(kUseKey, true)) { return; }

	if (m_selectID < 0) { return; }

	std::shared_ptr<PlayerBase> spPlayer = m_wpPlayer.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<PlayerInventory> spInventory = spPlayer->GetPlayerInventory();
	if (!spInventory) { return; }

	// UsePotionsInventoryは個数を見ずに減らすので、先に確認する
	if (spInventory->GetPotionsInventoryNum(m_selectID) <= 0) { return; }

	std::shared_ptr<PotionUseController> spController = m_wpPotionUseController.lock();
	if (!spController) { return; }

	// インベントリ画面(PotionInventoryUI::PotionUse)と同じ流れ
	spController->SetPlayer(spPlayer);
	spController->PotionUse(m_selectID);
	spInventory->UsePotionsInventory(m_selectID);

	m_popTimer = kPopTime;
}

void ItemBar::EffectUpdate()
{
	const float dt = DeltaTime::Instance().GetRealDeltaTime();

	// ずれを0に近づける
	m_slideOffset -= m_slideOffset * std::min(1.0f, dt * 15.0f);
	if (std::abs(m_slideOffset) < 0.001f) { m_slideOffset = 0.0f; }

	// はねる演出
	if (m_popTimer > 0.0f)
	{
		m_popTimer -= dt;
		if (m_popTimer < 0.0f) { m_popTimer = 0.0f; }
	}
}

bool ItemBar::IsInputBlocked()
{
	std::shared_ptr<PlayerBase> spPlayer = m_wpPlayer.lock();
	if (!spPlayer) { return true; }

	// 倒れている
	if (spPlayer->GetIsDead()) { return true; }

	// インベントリを開いている
	std::shared_ptr<PlayerInventoryUI> spInventoryUI = m_wpPlayerInventoryUI.lock();
	if (spInventoryUI && spInventoryUI->GetPlayerInventoryUIFlg()) { return true; }

	return false;
}

int ItemBar::GetSelectIndex()
{
	for (int i = 0; i < (int)m_itemIDList.size(); i++)
	{
		if (m_itemIDList[i] == m_selectID) { return i; }
	}
	return -1;
}

void ItemBar::DrawSlot(int _ID, float _x, float _size, float _alpha, bool _select)
{
	const Math::Color color = { 1, 1, 1, _alpha };

	// 左右の小さい枠(中央は選択枠を別に描くので描かない)
	if (!_select && m_spSlotTex)
	{
		const float frameSize = _size * 1.4f;
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spSlotTex, (int)_x, (int)m_2DPos.y,
			(int)frameSize, (int)frameSize, nullptr, &color);
	}

	// アイコン(左右は少し暗く)
	std::shared_ptr<PotionTexInfo> spTexInfo = m_wpPotionTexInfo.lock();
	if (!spTexInfo) { return; }

	std::shared_ptr<KdTexture> spIcon = spTexInfo->GetPotionIcon(_ID);
	if (!spIcon) { return; }

	const float bright = _select ? 1.0f : 0.75f;
	const Math::Color iconColor = { bright, bright, bright, _alpha };
	KdShaderManager::Instance().m_spriteShader.DrawTex(spIcon, (int)_x, (int)m_2DPos.y,
		(int)_size, (int)_size, nullptr, &iconColor);
}

std::shared_ptr<KdTexture> ItemBar::LoadTex(const std::string& _path)
{
	// ファイルがなければ読み込まない(画像を作る前でも動くように)
	std::ifstream ifs(_path);
	if (!ifs) { return nullptr; }
	ifs.close();

	std::shared_ptr<KdTexture> spTex = std::make_shared<KdTexture>();
	spTex->Load(_path);
	return spTex;
}
