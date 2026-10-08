#include "MapManager.h"
#include "MapBase.h"
#include "MapGenerate/MapGenerate.h"
#include "../../Camera/CameraBase.h"
#include "../../Character/Player/PlayerBase.h"
#include "../../Character/Enemy/EnemyBase.h"
#include "../../Character/Enemy/EnemyManager.h"
#include "../../UI/UIManager.h"
#include "../../UI/UIMap/UIMapManager.h"
#include "../../UI/UIMap/UIMap_Map/UIMap_Map.h"
#include"../MapObj/TreasureChest/TreasureChest.h"
#include"../MapObj/MapObjManager.h"

void MapManager::Init()
{
	m_spMapGenerate = std::make_shared<MapGenerate>();

	CHUNK_SIZE = m_spMapGenerate->GetCHUNK_SIZE();
}

void MapManager::Update()
{
	for (const auto& mapObj : m_updateChankes)
	{
		mapObj->Update();
	}
}

void MapManager::PostUpdate()
{
	for (const auto& mapObj : m_updateChankes)
	{
		mapObj->PostUpdate();
	}
}

void MapManager::DrawLit()
{
	for (const auto& mapObj : m_updateChankes)
	{
		mapObj->DrawLit();
	}
}

void MapManager::PreDraw()
{
	for (const auto& mapObj : m_updateChankes)
	{
		mapObj->PreDraw();
	}
}

void MapManager::GenerateDepthMapFromLight()
{
	for (const auto& mapObj : m_updateChankes)
	{
		mapObj->GenerateDepthMapFromLight();
	}
}

void MapManager::MapHit(const std::shared_ptr<KdGameObject>& obj)
{
	if (!obj) return;

	obj->ClearHitObjectList();

	Math::Vector3 pos = obj->GetPos();

	// ワールド座標 → タイル座標へ変換
	int tileX = static_cast<int>(pos.x / m_mapTileSiz);
	int tileY = static_cast<int>(-pos.z / m_mapTileSiz);

	// タイル座標 → チャンク番号へ変換
	int cx = tileX / CHUNK_SIZE;
	int cy = tileY / CHUNK_SIZE;

	constexpr float hitCheckDistSq = 15.0f * 15.0f;

	// チャンク範囲チェック
	auto inRange = [&](int x, int y)
		{
			return y >= 0 && y < m_chunks.size() &&
				x >= 0 && x < m_chunks[y].size();
		};

	// 中心＋周囲チャンクだけ判定
	for (int dy = -m_updateChunkRadius.y; dy <= m_updateChunkRadius.y; dy++)
	{
		for (int dx = -m_updateChunkRadius.x; dx <= m_updateChunkRadius.x; dx++)
		{
			int ncx = cx + dx;
			int ncy = cy + dy;

			if (!inRange(ncx, ncy)) continue;

			// このチャンクに属する MapBase をすべてチェック
			for (auto& wpMapObj : m_chunks[ncy][ncx])
			{
				std::shared_ptr<MapBase>spMapObj = wpMapObj.lock();

				if (!spMapObj) continue;

				float distSq = Math::Vector3::DistanceSquared(spMapObj->GetPos(), pos);

				if (distSq <= hitCheckDistSq)
				{
					obj->RegistHitObject(spMapObj);
				}
			}
		}
	}
}

void MapManager::MapHitEnemy(const std::shared_ptr<EnemyBase>& obj)
{
	MapHit(obj);
}

void MapManager::SetCamera(const std::shared_ptr<CameraBase>& spCamera)
{
	m_wpCamera = spCamera;

	for (const auto& mapObj : m_mapObj)
	{
		mapObj->SetCamera(spCamera);
		spCamera->ResolveCameraOcclusionObject(mapObj);
	}

	m_spMapGenerate->SetCamera(spCamera);
}

void MapManager::GenerateMap(Math::Vector2 _mapSiz, int roomNum, MapType _MapType)
{
	m_chunks.clear();
	m_mapObj.clear();
	m_chankeNum = { -999,-999 };

	if (!m_spMapGenerate) { return; }


	m_spMapGenerate->SetMapObjManager(m_wpMapObjManager.lock());

	//敵が歩ける一覧
	std::vector<std::vector<bool>> mapData;
	Math::Vector3 basePos;
	mapData = m_spMapGenerate->Generate(_mapSiz, roomNum, m_mapTileSiz, _MapType, &m_mapObj, &m_playerSpawnPos, &basePos);
	auto mapRoomList = m_spMapGenerate->GetRoomInfoList();


	m_chunks = m_spMapGenerate->GetChunks();

	//プレイヤーを設定
	std::shared_ptr<PlayerBase> spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		for (const auto& mapObj : m_mapObj)
		{
			mapObj->SetPlayer(spPlayerBase);
		}
	}


	////////////////////////////////////////////////////
	// ミニマップ生成
	std::shared_ptr<UIManager> spUIManager = m_wpUIManager.lock();
	if (spUIManager)
	{
		std::shared_ptr<UIMapManager> spUIMapManager = spUIManager->GetUIMapManager();
		if (spUIMapManager)
		{
			spUIMapManager->SetBase3DPos(basePos);
			spUIMapManager->SetTileSiz(m_mapTileSiz);
			spUIMapManager->GetUIMap_Map()->PosListReset();
			spUIMapManager->ResetTreasureChest();

			for (const auto& mapObj : m_mapObj)
			{
				if (mapObj->GetMapObjType() == MapObjType::Ground || mapObj->GetMapObjType() == MapObjType::TypeSlope)
				{

					if (mapObj->GetRoomType() == RoomType::RoomType_NORoom)
					{
						spUIMapManager->GetUIMap_Map()->AddPosList(mapObj->GetPos(), m_mapTileSiz);
					}
					else
					{
						spUIMapManager->GetUIMap_Map()->AddPosList(mapObj->GetPos(), m_mapTileSiz, mapObj->GetRoomID());
					}


				}

				if (mapObj->GetMapObjType() == MapObjType::Stairs)
				{
					spUIMapManager->GetUIMap_Map()->AddStairsPos(mapObj->GetPos(), m_mapTileSiz, mapObj->GetRoomID());
					spUIMapManager->GetUIMap_Map()->SetIsStairsMine(true);
				}
			}
		}
	}

	////////////////////////////////////////////////////
	// 敵の生成
	std::shared_ptr<EnemyManager> spEnemyManager = m_wpEnemyManager.lock();
	if (spEnemyManager)
	{

		std::random_device rd;
		std::mt19937 mt(rd());

		for (size_t i = 0; i < mapRoomList.size(); i++)
		{
			if (mapRoomList[i].empty()) { continue; }

			int roomEnemyNum = mapRoomList[i][0].m_roomEnemyNum;

			struct EnemySpawnList {
				Math::Vector3 m_pos;
				int roomID = 0;
				int floorNum = 0;
			};

			std::vector<EnemySpawnList> enemySpawnList;
			enemySpawnList.reserve(mapRoomList[i].size());

			for (size_t j = 0; j < mapRoomList[i].size(); j++)
			{
				//何か設置されてたらスキップ
				if (mapRoomList[i][j].m_Installation) { continue; }

				EnemySpawnList enemySpawn;
				enemySpawn.m_pos = mapRoomList[i][j].m_pos;
				enemySpawn.roomID = mapRoomList[i][j].m_roomID;
				enemySpawn.floorNum = j;
				enemySpawnList.push_back(enemySpawn);
			}

			if (enemySpawnList.empty()) { continue; }

			// 敵数を制限
			roomEnemyNum = std::min(roomEnemyNum, static_cast<int>(enemySpawnList.size()));

			// ランダムシャッフル
			std::shuffle(enemySpawnList.begin(), enemySpawnList.end(), mt);

			// 先頭から必要数だけスポーン
			for (int n = 0; n < roomEnemyNum; n++)
			{
				spEnemyManager->SpawnEnemy(RoomEnemy, enemySpawnList[n].m_pos);
				mapRoomList[enemySpawnList[n].roomID][enemySpawnList[n].floorNum].m_Installation = true;
			}
		}
	}


	////////////////////////////////////////////////////
	//宝箱生成
	std::shared_ptr<MapObjManager> spMapObjManager = m_wpMapObjManager.lock();
	if (spMapObjManager)
	{
		std::list<Math::Vector3> TreasureChestPosList;

		for (size_t i = 0; i < mapRoomList.size(); i++)
		{
			if (mapRoomList[i].empty()) { continue; }
			int chestNum = mapRoomList[i][0].m_roomTreasuerChestNum;
			int roomType = mapRoomList[i][0].m_roomType;

			// 宝箱を置ける床（Installation == false）が存在するかチェック
			int freeTileCount = 0;
			for (const auto& r : mapRoomList[i])
			{
				if (!r.m_Installation) { freeTileCount++; }
			}
			if (freeTileCount == 0) { continue; }

			int tryCount = 0;
			int maxTry = static_cast<int>(mapRoomList[i].size()) * 3;

			while (chestNum > 0 && tryCount < maxTry)
			{
				tryCount++;
				int LoomNum = KdRandom::GetInt(0, static_cast<int>(mapRoomList[i].size()) - 1);
				if (mapRoomList[i][LoomNum].m_Installation)
				{
					continue;
				}

				float spawnRate = (roomType == RoomType::RoomType_TreasureChestRoom || roomType == RoomType::RoomType_SafeRoom) ? 1.0f : 0.4f;

				if (KdRandom::GetFloat(0.0f, 1.0f) <= spawnRate)
				{
					Math::Vector3 pos = mapRoomList[i][LoomNum].m_pos;
					mapRoomList[i][LoomNum].m_Installation = true;

					int x = static_cast<int>(mapRoomList[i][LoomNum].m_xy.x);
					int y = static_cast<int>(mapRoomList[i][LoomNum].m_xy.y);
					mapData[y][x] = false;
					TreasureChestPosList.push_back(pos);
				}

				chestNum--;
			}
		}

		for (const auto& pos : TreasureChestPosList)
		{
			std::shared_ptr<TreasureChest> spTreasureChest = std::make_shared<TreasureChest>();
			spTreasureChest->Init();
			spTreasureChest->SetPos(pos);

			// チャンク番号の計算とセット
			int tileX = static_cast<int>(pos.x / m_mapTileSiz);
			int tileY = static_cast<int>(-pos.z / m_mapTileSiz);
			int cx = tileX / CHUNK_SIZE;
			int cy = tileY / CHUNK_SIZE;
			spTreasureChest->SetChunkNum(Math::Vector2(static_cast<float>(cx), static_cast<float>(cy)));

			spTreasureChest->SetUIManager(m_wpUIManager.lock());
			spTreasureChest->SetPlayer(m_wpPlayerBase.lock());
			spTreasureChest->SetCamera(m_wpCamera.lock());

			spMapObjManager->AddMapObj(spTreasureChest);
		}
	}




	//A*の初期化
	CreateNodeGrid(static_cast<int>(_mapSiz.x), static_cast<int>(_mapSiz.y), m_mapTileSiz);
	//A*の設定
	ApplyWalkableFromMap(mapData);



	//カメラセット
	if (!m_wpCamera.expired())
	{
		SetCamera(m_wpCamera.lock());
	}

}

void MapManager::GenerateBossMap(Math::Vector2 _mapSiz, MapType _type)
{
	m_chankeNum = { -999,-999 };
	m_mapObj.clear();
	m_chunks.clear();

	if (!m_spMapGenerate) { return; }

	//敵が歩ける一覧
	std::vector<std::vector<bool>> mapData;

	Math::Vector3 basePos;
	mapData = m_spMapGenerate->GenerateBoss(_mapSiz, m_mapTileSiz, (int)_type, &m_mapObj, &m_playerSpawnPos, &basePos);

	m_chunks = m_spMapGenerate->GetChunks();

	//A*の初期化
	CreateNodeGrid(static_cast<int>(_mapSiz.x), static_cast<int>(_mapSiz.y), m_mapTileSiz);
	//A*の設定
	ApplyWalkableFromMap(mapData);

	//プレイヤーを設定
	std::shared_ptr<PlayerBase> spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		for (const auto& mapObj : m_mapObj)
		{
			mapObj->SetPlayer(spPlayerBase);
		}
	}
	////////////////////////////////////////////////////
	//ボス生成
	std::shared_ptr<EnemyManager> spEnemyManager = m_wpEnemyManager.lock();
	if (spEnemyManager)
	{
		spEnemyManager->SpawnBoss(m_spMapGenerate->GetBossSpawnPos());
	}


	////////////////////////////////////////////////////
	// ミニマップ生成
	std::shared_ptr<UIManager> spUIManager = m_wpUIManager.lock();
	if (spUIManager)
	{
		std::shared_ptr<UIMapManager> spUIMapManager = spUIManager->GetUIMapManager();
		if (spUIMapManager)
		{
			spUIMapManager->SetBase3DPos(basePos);
			spUIMapManager->SetTileSiz(m_mapTileSiz);
			spUIMapManager->GetUIMap_Map()->PosListReset();
			spUIMapManager->ResetTreasureChest();

			for (const auto& mapObj : m_mapObj)
			{
				if (mapObj->GetMapObjType() == MapObjType::Ground)
				{
					spUIMapManager->GetUIMap_Map()->AddPosList(mapObj->GetPos(), m_mapTileSiz);
				}
			}
		}
	}

	////////////////////////////////////////////////////
	//カメラセット
	if (!m_wpCamera.expired())
	{
		SetCamera(m_wpCamera.lock());
	}
}

void MapManager::SetPlayerChanke(Math::Vector3 _pos)
{
	// ワールド座標 → タイル座標へ変換
	int tileX = static_cast<int>(_pos.x / m_mapTileSiz);
	int tileY = static_cast<int>(-_pos.z / m_mapTileSiz);

	// タイル座標 → チャンク番号へ変換
	int cx = tileX / CHUNK_SIZE;
	int cy = tileY / CHUNK_SIZE;

	//今までと同じなら変えなくてOK
	if (m_chankeNum.x == cx && m_chankeNum.y == cy)
	{
		m_isChunkChanged = false;
		return;
	}

	m_isChunkChanged = true;
	m_updateChankes.clear();
	m_chankeNum = { static_cast<float>(cx),static_cast<float>(cy) };

	// チャンク範囲チェック
	auto inRange = [&](int x, int y)
		{
			return y >= 0 && y < m_chunks.size() &&
				x >= 0 && x < m_chunks[y].size();
		};

	if (m_updateChunkRadius.x <= 0)
	{
		m_updateChunkRadius.x = 1;
	}

	if (m_updateChunkRadius.y <= 0)
	{
		m_updateChunkRadius.y = 1;
	}

	std::unordered_set<std::shared_ptr<MapBase>> uniqueUpdateSet;

	for (int dy = -static_cast<int>(m_updateChunkRadius.y); dy <= static_cast<int>(m_updateChunkRadius.y); dy++)
	{
		for (int dx = -static_cast<int>(m_updateChunkRadius.x); dx <= static_cast<int>(m_updateChunkRadius.x); dx++)
		{
			int ncx = cx + dx;
			int ncy = cy + dy;

			if (!inRange(ncx, ncy)) continue;

			for (auto& wpMapObj : m_chunks[ncy][ncx])
			{
				if (auto spMapObj = wpMapObj.lock())
				{
					uniqueUpdateSet.insert(spMapObj);
				}
			}
		}
	}

	// 最後に push_back
	m_updateChankes.assign(uniqueUpdateSet.begin(), uniqueUpdateSet.end());
}

bool MapManager::GetChunksUpdate(Math::Vector3 _pos)
{
	// ワールド座標 → タイル座標へ変換（切り下げ）
	int tileX = static_cast<int>(std::floor(_pos.x / m_mapTileSiz));
	int tileY = static_cast<int>(std::floor(-_pos.z / m_mapTileSiz));
	
	// タイル座標 → チャンク番号へ変換
	int cx = static_cast<int>(std::floor(static_cast<float>(tileX) / CHUNK_SIZE));
	int cy = static_cast<int>(std::floor(static_cast<float>(tileY) / CHUNK_SIZE));

	// チャンク配列が存在する場合、配列の範囲内に安全に収める（クランプ）
	if (!m_chunks.empty())
	{
		int maxCy = static_cast<int>(m_chunks.size()) - 1;
		cy = std::clamp(cy, 0, std::max(0, maxCy));
		if (maxCy >= 0 && !m_chunks[cy].empty())
		{
			int maxCx = static_cast<int>(m_chunks[cy].size()) - 1;
			cx = std::clamp(cx, 0, std::max(0, maxCx));
		}
	}
	return GetChunksUpdate({ static_cast<float>(cx), static_cast<float>(cy) });
}

bool MapManager::GetChunksUpdate(Math::Vector2 _chunkNum)
{
	// チャンク範囲チェック
	auto inRange = [&](int x, int y)
		{
			return y >= 0 && y < static_cast<int>(m_chunks.size()) &&
				x >= 0 && x < static_cast<int>(m_chunks[y].size());
		};
	// float から int へ正確に変換（誤差防止）
	int targetCx = static_cast<int>(std::round(_chunkNum.x));
	int targetCy = static_cast<int>(std::round(_chunkNum.y));
	int playerCx = static_cast<int>(std::round(m_chankeNum.x));
	int playerCy = static_cast<int>(std::round(m_chankeNum.y));

	// 修正：dy に .y、dx に .x を使用
	for (int dy = -static_cast<int>(m_updateChunkRadius.y); dy <= static_cast<int>(m_updateChunkRadius.y); dy++)
	{
		for (int dx = -static_cast<int>(m_updateChunkRadius.x); dx <= static_cast<int>(m_updateChunkRadius.x); dx++)
		{
			int ncx = playerCx + dx;
			int ncy = playerCy + dy;
			if (!inRange(ncx, ncy)) { continue; }
			// int 型同士で安全・確実な比較
			if (targetCx == ncx && targetCy == ncy)
			{
				return true;
			}
		}
	}
	return false;
}


void MapManager::CreateNodeGrid(int width, int height, float tileSize)
{
	// 1タイルあたり 3x3（中央＋周囲8マス = 全9ノード）に分割
	int gridWidth = width * 3;
	int gridHeight = height * 3;

	m_nodes.resize(gridHeight);
	for (int y = 0; y < gridHeight; y++)
	{
		m_nodes[y].resize(gridWidth);

		for (int x = 0; x < gridWidth; x++)
		{
			Node& node = m_nodes[y][x];

			// グリッド座標
			node.pos = Math::Vector2(static_cast<float>(x), static_cast<float>(y));

			// 初期値(通れないで初期化)
			node.walkable = false;

			// A* 用の初期化
			node.gCost = FLT_MAX;
			node.hCost = 0;
			node.parent = nullptr;
		}
	}
}

void MapManager::ApplyWalkableFromMap(const std::vector<std::vector<bool>>& mapData)
{
	int height = static_cast<int>(mapData.size());
	if (height == 0) return;
	int width = static_cast<int>(mapData[0].size());
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			// 床ではない（壁/空きマス）場合はスキップ
			if (!mapData[y][x]) continue;
			// 隣接マス（上下左右）が「マップ外」または「床ではない（＝壁がある）」かを判定
			bool wallNorth = (y == 0) || !mapData[y - 1][x];          // 上側（Y-1）に壁
			bool wallSouth = (y == height - 1) || !mapData[y + 1][x];  // 下側（Y+1）に壁
			bool wallWest = (x == 0) || !mapData[y][x - 1];          // 左側（X-1）に壁
			bool wallEast = (x == width - 1) || !mapData[y][x + 1];  // 右側（X+1）に壁
			// タイル内の 3x3 サブノードに対してフラグを設定
			for (int dy = 0; dy < 3; dy++)
			{
				for (int dx = 0; dx < 3; dx++)
				{
					int subX = x * 3 + dx;
					int subY = y * 3 + dy;
					if (subY >= static_cast<int>(m_nodes.size()) || subX >= static_cast<int>(m_nodes[subY].size()))
					{
						continue;
					}
					// 基本は通行可能 (true) に初期化
					bool isWalkable = true;
					// 壁が立っている側のサブノード列は通行不可 (false) に設定
					if (wallNorth && dy == 0) isWalkable = false; // 上端（dy = 0）
					if (wallSouth && dy == 2) isWalkable = false; // 下端（dy = 2）
					if (wallWest && dx == 0) isWalkable = false; // 左端（dx = 0）
					if (wallEast && dx == 2) isWalkable = false; // 右端（dx = 2）
					m_nodes[subY][subX].walkable = isWalkable;
				}
			}
		}
	}
}

Math::Vector3 MapManager::NodeToWorld(const Node* node) const
{
	if (!node) return Math::Vector3::Zero;

	float subTileSize = m_mapTileSiz / 3.0f;
	float worldX = subTileSize * node->pos.x + subTileSize * 0.5f;
	float worldZ = -(subTileSize * node->pos.y + subTileSize * 0.5f);

	return Math::Vector3(worldX, 0.0f, worldZ);
}

Node* MapManager::WorldToNode(const Math::Vector3& worldPos)
{
	float subTileSize = m_mapTileSiz / 3.0f;

	// X は右へプラス
	int x = static_cast<int>(floor(worldPos.x / subTileSize));

	// Z は下へマイナス → -Z がノード番号
	int y = static_cast<int>(floor((-worldPos.z) / subTileSize));

	if (m_nodes.empty() || m_nodes[0].empty()) { return nullptr; }

	int width = static_cast<int>(m_nodes[0].size());
	int height = static_cast<int>(m_nodes.size());

	if (x < 0 || y < 0 || x >= width || y >= height)
	{
		return nullptr;
	}

	return &m_nodes[y][x];
}

std::vector<Node*> MapManager::FindPath(Node* start, Node* goal)
{
	if (!start || !goal) return {};

	for (auto& row : m_nodes)
	{
		for (auto& node : row)
		{
			node.gCost = FLT_MAX;
			node.hCost = 0.0f;
			node.parent = nullptr;
		}
	}

	// openList = 探索候補
	std::vector<Node*> openList;

	// closedList = 探索済み
	std::vector<Node*> closedList;

	// 初期化
	start->gCost = 0.0f;
	start->hCost = Heuristic(start, goal);
	start->parent = nullptr;

	openList.push_back(start);

	while (!openList.empty())
	{
		// openList の中で fCost が最小のノードを探す
		Node* current = openList[0];
		for (auto* node : openList)
		{
			if (node->fCost() < current->fCost() ||
				(node->fCost() == current->fCost() && node->hCost < current->hCost))
			{
				current = node;
			}
		}

		// openList から current を削除
		openList.erase(std::remove(openList.begin(), openList.end(), current), openList.end());
		closedList.push_back(current);

		// ゴールに到達したら経路復元
		if (current == goal)
		{
			return BuildPath(goal);
		}

		// 隣接ノードを取得 (周囲8方向)
		auto neighbors = GetNeighbors(current);

		for (auto* neighbor : neighbors)
		{
			// 通れない or 探索済みならスキップ
			if (!neighbor->walkable ||
				std::find(closedList.begin(), closedList.end(), neighbor) != closedList.end())
			{
				continue;
			}

			// 直交か斜めかで移動コストを設定 (直交=1.0, 斜め=1.414)
			float stepCost = (current->pos.x != neighbor->pos.x && current->pos.y != neighbor->pos.y) ? 1.41421356f : 1.0f;
			float newCost = current->gCost + stepCost;

			// 新しいルートの方が安いなら更新
			if (newCost < neighbor->gCost ||
				std::find(openList.begin(), openList.end(), neighbor) == openList.end())
			{
				neighbor->gCost = newCost;
				neighbor->hCost = Heuristic(neighbor, goal);
				neighbor->parent = current;

				// openList に未登録なら追加
				if (std::find(openList.begin(), openList.end(), neighbor) == openList.end())
				{
					openList.push_back(neighbor);
				}
			}
		}
	}

	// 経路なし
	return {};
}

std::vector<Node*> MapManager::BuildPath(Node* goal) const
{
	std::vector<Node*> path;
	Node* current = goal;

	while (current != nullptr)
	{
		path.push_back(current);
		current = current->parent;
	}

	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<Node*> MapManager::GetNeighbors(Node* node)
{
	std::vector<Node*> neighbors;
	if (!node) return neighbors;

	int x = static_cast<int>(node->pos.x);
	int y = static_cast<int>(node->pos.y);
	int width = static_cast<int>(m_nodes[0].size());
	int height = static_cast<int>(m_nodes.size());

	// 周囲8方向のオフセット (上下左右 ＋ 斜め4方向)
	static const int dirX[8] = {  0,  0, -1,  1, -1,  1, -1,  1 };
	static const int dirY[8] = { -1,  1,  0,  0, -1, -1,  1,  1 };

	for (int i = 0; i < 8; i++)
	{
		int nx = x + dirX[i];
		int ny = y + dirY[i];

		if (nx >= 0 && nx < width && ny >= 0 && ny < height)
		{
			// 斜め移動時の角抜け防止チェック（直交する壁がある場合は斜め移動を許可しない）
			if (dirX[i] != 0 && dirY[i] != 0)
			{
				if (!m_nodes[y][nx].walkable || !m_nodes[ny][x].walkable)
				{
					continue;
				}
			}

			neighbors.push_back(&m_nodes[ny][nx]);
		}
	}

	return neighbors;
}
