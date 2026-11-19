#include <queue>
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

//A*の探索で、優先度付きキューに入れる1件分
struct PathQueueItem
{
	//合計のコスト
	float m_fCost;
	//ゴールまでの推定コスト
	float m_hCost;
	//ノード
	Node* m_node;

	//優先度付きキューで小さい順に並べるための比較(fCostが同じならhCostで比べる)
	// _other ... 比べる相手
	//戻り値 ... 自分の方が大きければtrue
	bool operator>(const PathQueueItem& _other) const
	{
		if (m_fCost == _other.m_fCost) return m_hCost > _other.m_hCost;
		return m_fCost > _other.m_fCost;
	}
};

void MapManager::Init()
{
	m_spMapGenerate = std::make_shared<MapGenerate>();

	m_chunkSize = m_spMapGenerate->GetChunkSize();


	if (!m_spBoxModel)
	{
		m_spBoxModel = std::make_shared<KdModelData>();
		m_spBoxModel->Load("Asset/Models/Terrains/Map/Box/Base.gltf");
	}
}

void MapManager::Update()
{
	for (const auto& mapObj : m_updateChunks)
	{
		mapObj->Update();
	}
}

void MapManager::PostUpdate()
{
	for (const auto& mapObj : m_updateChunks)
	{
		mapObj->PostUpdate();
	}
}

void MapManager::DrawLit()
{
	for (const auto& mapObj : m_drawChunks)
	{
		mapObj->DrawLit();
	}

	if (m_spBoxModel)
	{
		auto spCamera = m_wpCamera.lock();
		if (spCamera)
		{
			Math::Vector3 pos = spCamera->GetPos();
			m_boxPos.x = pos.x;
			m_boxPos.z = pos.z;
		}


		Math::Matrix tMat = Math::Matrix::CreateTranslation(m_boxPos);
		Math::Matrix sMat = Math::Matrix::CreateScale(m_boxSiz / 2);
		Math::Matrix mat = sMat * tMat;

		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spBoxModel, mat);
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullBack);
	}

}

void MapManager::PreDraw()
{
	for (const auto& mapObj : m_drawChunks)
	{
		mapObj->PreDraw();
	}
}

void MapManager::GenerateDepthMapFromLight()
{
	for (const auto& mapObj : m_drawChunks)
	{
		mapObj->GenerateDepthMapFromLight();
	}
}

bool MapManager::GetChunksUpdate(Math::Vector3 _pos)
{
	// ワールド座標 → タイル座標へ変換（切り下げ）
	int tileX = static_cast<int>(std::floor(_pos.x / m_mapTileSiz));
	int tileY = static_cast<int>(std::floor(-_pos.z / m_mapTileSiz));

	// タイル座標 → チャンク番号へ変換
	int cx = static_cast<int>(std::floor(static_cast<float>(tileX) / m_chunkSize));
	int cy = static_cast<int>(std::floor(static_cast<float>(tileY) / m_chunkSize));

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
	auto inRange = [&](int _x, int _y)
		{
			return _y >= 0 && _y < static_cast<int>(m_chunks.size()) &&
				_x >= 0 && _x < static_cast<int>(m_chunks[_y].size());
		};
	// float から int へ正確に変換（誤差防止）
	int targetCx = static_cast<int>(std::round(_chunkNum.x));
	int targetCy = static_cast<int>(std::round(_chunkNum.y));
	int playerCx = static_cast<int>(std::round(m_chunkNum.x));
	int playerCy = static_cast<int>(std::round(m_chunkNum.y));

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

bool MapManager::GetChunksDraw(Math::Vector2 _chunkNum)
{
	// チャンク範囲チェック
	auto inRange = [&](int _x, int _y)
		{
			return _y >= 0 && _y < static_cast<int>(m_chunks.size()) &&
				_x >= 0 && _x < static_cast<int>(m_chunks[_y].size());
		};
	// float から int へ正確に変換（誤差防止）
	int targetCx = static_cast<int>(std::round(_chunkNum.x));
	int targetCy = static_cast<int>(std::round(_chunkNum.y));
	int playerCx = static_cast<int>(std::round(m_chunkNum.x));
	int playerCy = static_cast<int>(std::round(m_chunkNum.y));

	// 修正：dy に .y、dx に .x を使用
	for (int dy = -static_cast<int>(m_updateChunkRadius.y + m_drawChunkRadius.y); dy <= static_cast<int>(m_updateChunkRadius.y + m_drawChunkRadius.y); dy++)
	{
		for (int dx = -static_cast<int>(m_updateChunkRadius.x + m_drawChunkRadius.x); dx <= static_cast<int>(m_updateChunkRadius.x + m_drawChunkRadius.x); dx++)
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

Math::Vector3 MapManager::GetRandomPos()
{
	Math::Vector3 retPos = {};
	while (1)
	{
		auto it = m_mapObj.begin();
		std::advance(it, rand() % m_mapObj.size());
		auto& randomElem = *it;

		if (randomElem->GetMapObjType() == MapObjType::Ground)
		{
			retPos = randomElem->GetPos();
			break;
		}
	}

	return retPos;
}

Math::Vector3 MapManager::GetChunkCenterPos()
{
	// チャンク番号（float なので int にキャスト）
	int cx = static_cast<int>(m_chunkNum.x);
	int cy = static_cast<int>(m_chunkNum.y);

	// チャンクのワールド座標サイズ
	float chunkWorldSize = m_chunkSize * m_mapTileSiz;

	// チャンクの左上（minX, minZ）
	float minX = cx * chunkWorldSize;
	float minZ = -(cy * chunkWorldSize);

	// チャンク中心
	float centerX = minX + chunkWorldSize * 0.5f;
	float centerZ = minZ - chunkWorldSize * 0.5f;


	// Y は地面高さに合わせる（必要なら変更）
	return Math::Vector3(centerX, m_mapBasePos.y, centerZ);
}

void MapManager::SetCamera(const std::shared_ptr<CameraBase>& _spCamera)
{
	m_wpCamera = _spCamera;

	for (const auto& mapObj : m_mapObj)
	{
		mapObj->SetCamera(_spCamera);
		_spCamera->ResolveCameraOcclusionObject(mapObj);
	}

	m_spMapGenerate->SetCamera(_spCamera);
}

void MapManager::SetPlayerChunk(Math::Vector3 _pos)
{
	// ワールド座標 → タイル座標へ変換
	int tileX = static_cast<int>(_pos.x / m_mapTileSiz);
	int tileY = static_cast<int>(-_pos.z / m_mapTileSiz);

	// タイル座標 → チャンク番号へ変換
	int cx = tileX / m_chunkSize;
	int cy = tileY / m_chunkSize;

	//今までと同じなら変えなくてOK
	if (m_chunkNum.x == cx && m_chunkNum.y == cy)
	{
		m_isChunkChanged = false;
		return;
	}

	m_isChunkChanged = true;
	m_updateChunks.clear();
	m_drawChunks.clear();
	m_chunkNum = { static_cast<float>(cx),static_cast<float>(cy) };

	// チャンク範囲チェック
	auto inRange = [&](int _x, int _y)
		{
			return _y >= 0 && _y < m_chunks.size() &&
				_x >= 0 && _x < m_chunks[_y].size();
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
	std::unordered_set<std::shared_ptr<MapBase>> uniqueDrawSet;



	for (int dy = -(int)m_updateChunkRadius.y - m_drawChunkRadius.y; dy <= (int)m_updateChunkRadius.y + m_drawChunkRadius.y; dy++)
	{
		for (int dx = -(int)m_updateChunkRadius.x - m_drawChunkRadius.x; dx <= (int)m_updateChunkRadius.x + m_drawChunkRadius.x; dx++)
		{
			int ncx = cx + dx;
			int ncy = cy + dy;

			if (!inRange(ncx, ncy)) continue;

			for (auto& wpMapObj : m_chunks[ncy][ncx])
			{
				if (auto spMapObj = wpMapObj.lock())
				{
					if (dy >= -(int)m_updateChunkRadius.y && dy <= (int)m_updateChunkRadius.y &&
						dx >= -(int)m_updateChunkRadius.x && dx <= (int)m_updateChunkRadius.x)
					{
						uniqueUpdateSet.insert(spMapObj);
					}
					uniqueDrawSet.insert(spMapObj);
				}
			}
		}
	}

	int num = 1;

	// 最後に push_back
	m_updateChunks.assign(uniqueUpdateSet.begin(), uniqueUpdateSet.end());
	m_drawChunks.assign(uniqueDrawSet.begin(), uniqueDrawSet.end());

	int drawMinChunkX =
		cx - ((int)m_updateChunkRadius.x + m_drawChunkRadius.x - 1);

	int drawMaxChunkX =
		cx + ((int)m_updateChunkRadius.x + m_drawChunkRadius.x - 1);

	int drawMinChunkY =
		cy - ((int)m_updateChunkRadius.y + m_drawChunkRadius.y - 1);

	int drawMaxChunkY =
		cy + ((int)m_updateChunkRadius.y + m_drawChunkRadius.y - 1);

	// ワールド座標に変換
	float worldMinX = drawMinChunkX * m_chunkSize * m_mapTileSiz;

	float worldMaxX = (drawMaxChunkX + 1) * m_chunkSize * m_mapTileSiz;

	float worldMinZ = -(drawMinChunkY * m_chunkSize * m_mapTileSiz);

	float worldMaxZ = -((drawMaxChunkY + 1) * m_chunkSize * m_mapTileSiz);

	float sizeX = fabsf(worldMaxX - worldMinX);
	float sizeZ = fabsf(worldMaxZ - worldMinZ);
	float sizeY = m_mapTileSiz * m_spMapGenerate->GetHeightLevelMax();

	m_boxPos = GetChunkCenterPos();
	m_boxSiz = { sizeX, sizeY * 2.0f, sizeZ };
}

void MapManager::MapHit(const std::shared_ptr<KdGameObject>& _spObj)
{
	if (!_spObj) return;

	_spObj->ClearHitObjectList();

	Math::Vector3 pos = _spObj->GetPos();

	// ワールド座標 → タイル座標へ変換
	int tileX = static_cast<int>(pos.x / m_mapTileSiz);
	int tileY = static_cast<int>(-pos.z / m_mapTileSiz);

	// タイル座標 → チャンク番号へ変換
	int cx = tileX / m_chunkSize;
	int cy = tileY / m_chunkSize;

	constexpr float hitCheckDistSq = 15.0f * 15.0f;

	// チャンク範囲チェック
	auto inRange = [&](int _x, int _y)
		{
			return _y >= 0 && _y < m_chunks.size() &&
				_x >= 0 && _x < m_chunks[_y].size();
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

				//壁は縦に伸ばしていて、位置は一番下の段にあるので、高さを無視して横の距離だけで測る
				Math::Vector3 objPos = spMapObj->GetPos();
				float dx = objPos.x - pos.x;
				float dz = objPos.z - pos.z;
				float distSq = dx * dx + dz * dz;

				if (distSq <= hitCheckDistSq)
				{
					_spObj->RegistHitObject(spMapObj);
				}
			}
		}
	}
}

void MapManager::MapHitEnemy(const std::shared_ptr<EnemyBase>& _spObj)
{
	MapHit(_spObj);
}

void MapManager::GenerateMap(Math::Vector2 _mapSiz, int _roomNum, MapType _mapType)
{
	m_chunks.clear();
	m_mapObj.clear();
	m_chunkNum = { -999,-999 };

	if (!m_spMapGenerate) { return; }


	m_spMapGenerate->SetMapObjManager(m_wpMapObjManager.lock());

	//敵が歩ける一覧
	std::vector<std::vector<bool>> mapData;
	Math::Vector3 basePos;
	mapData = m_spMapGenerate->Generate(_mapSiz, _roomNum, m_mapTileSiz, _mapType, &m_mapObj, &m_playerSpawnPos, &basePos);
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

			//敵を出す候補の床1つ分
			struct EnemySpawnList {
				Math::Vector3 m_pos;
				int m_roomID = 0;
				int m_floorNum = 0;
				EnemyType m_enemyType = EnemyType::RoomEnemy;
			};

			std::vector<EnemySpawnList> enemySpawnList;
			enemySpawnList.reserve(mapRoomList[i].size());

			for (size_t j = 0; j < mapRoomList[i].size(); j++)
			{
				//何か設置されてたらスキップ
				if (mapRoomList[i][j].m_installation) { continue; }

				EnemySpawnList enemySpawn;
				enemySpawn.m_pos = mapRoomList[i][j].m_pos;
				enemySpawn.m_roomID = mapRoomList[i][j].m_roomID;
				enemySpawn.m_floorNum = j;

				if (mapRoomList[i][j].m_roomType == RoomType::RoomType_Stairs)
				{
					enemySpawn.m_enemyType = EnemyType::RoomStarsEnemy;

					int y = mapRoomList[i].size() / 2;

					enemySpawn.m_pos = mapRoomList[i][y].m_pos;
					mapRoomList[i][y].m_installation = true;
				}
				else
				{
					enemySpawn.m_enemyType = EnemyType::RoomEnemy;
				}

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
				spEnemyManager->SpawnEnemy(enemySpawnList[n].m_enemyType, enemySpawnList[n].m_pos);
				mapRoomList[enemySpawnList[n].m_roomID][enemySpawnList[n].m_floorNum].m_installation = true;
			}
		}
	}


	////////////////////////////////////////////////////
	//宝箱生成
	std::shared_ptr<MapObjManager> spMapObjManager = m_wpMapObjManager.lock();
	if (spMapObjManager)
	{
		std::list<Math::Vector3> treasureChestPosList;

		//////////////////////////////////////////
		// 部屋ごとに chestNum 個置く
		for (size_t i = 0; i < mapRoomList.size(); i++)
		{
			if (mapRoomList[i].empty()) { continue; }
			int chestNum = mapRoomList[i][0].m_roomTreasureChestNum;
			if (chestNum <= 0) { continue; }

			//この部屋の、何も置いていない床の番号を集める
			std::vector<int> freeFloors;
			for (size_t j = 0; j < mapRoomList[i].size(); j++)
			{
				if (!mapRoomList[i][j].m_installation)
				{
					freeFloors.push_back(static_cast<int>(j));
				}
			}

			//シャッフルする(フィッシャー–イェーツ)
			for (int k = static_cast<int>(freeFloors.size()) - 1; k > 0; k--)
			{
				std::swap(freeFloors[k], freeFloors[KdRandom::GetInt(0, k)]);
			}

			//前から chestNum 個置く(床が足りなければ置けるだけ)
			int placeNum = std::min(chestNum, static_cast<int>(freeFloors.size()));
			for (int n = 0; n < placeNum; n++)
			{
				int j = freeFloors[n];

				mapRoomList[i][j].m_installation = true;

				int x = static_cast<int>(mapRoomList[i][j].m_xy.x);
				int y = static_cast<int>(mapRoomList[i][j].m_xy.y);
				mapData[y][x] = false;

				treasureChestPosList.push_back(mapRoomList[i][j].m_pos);
			}
		}

		//////////////////////////////////////////
		// 宝箱を作る
		for (const auto& pos : treasureChestPosList)
		{
			std::shared_ptr<TreasureChest> spTreasureChest = std::make_shared<TreasureChest>();
			spTreasureChest->Init();
			spTreasureChest->SetPos(pos);

			// チャンク番号の計算とセット
			int tileX = static_cast<int>(pos.x / m_mapTileSiz);
			int tileY = static_cast<int>(-pos.z / m_mapTileSiz);
			int cx = tileX / m_chunkSize;
			int cy = tileY / m_chunkSize;
			spTreasureChest->SetChunkNum(Math::Vector2(static_cast<float>(cx), static_cast<float>(cy)));

			auto spUIManage = m_wpUIManager.lock();

			spTreasureChest->SetUIManager(spUIManage);
			spTreasureChest->SetPlayer(m_wpPlayerBase.lock());
			spTreasureChest->SetCamera(m_wpCamera.lock());
			if (spUIManage)
			{
				spTreasureChest->SetUITex(spUIManage->GetTreasureChestUiTex());
			}

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
	m_chunkNum = { -999,-999 };
	m_mapObj.clear();
	m_chunks.clear();

	if (!m_spMapGenerate) { return; }

	//敵が歩ける一覧
	std::vector<std::vector<bool>> mapData;

	Math::Vector3 mapBasePos = {};
	mapData = m_spMapGenerate->GenerateBoss(_mapSiz, m_mapTileSiz, (int)_type, &m_mapObj, &m_playerSpawnPos, &mapBasePos);

	m_nextWarpGatePos = m_spMapGenerate->GetNextWarpGatePos();
	m_escapeWarpGatePos = m_spMapGenerate->GetEscapeWarpGatePos();

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
			spUIMapManager->SetBase3DPos(mapBasePos);
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

Math::Vector3 MapManager::NodeToWorld(const Node* _node) const
{
	if (!_node) return Math::Vector3::Zero;

	float subTileSize = m_mapTileSiz;
	float worldX = subTileSize * _node->m_pos.x + subTileSize * 0.5f;
	float worldZ = -(subTileSize * _node->m_pos.y + subTileSize * 0.5f);

	return Math::Vector3(worldX, 0.0f, worldZ);
}

Node* MapManager::WorldToNode(const Math::Vector3& _worldPos)
{
	float subTileSize = m_mapTileSiz;

	// X は右へプラス
	int x = static_cast<int>(floor(_worldPos.x / subTileSize));

	// Z は下へマイナス → -Z がノード番号
	int y = static_cast<int>(floor((-_worldPos.z) / subTileSize));

	if (m_nodes.empty() || m_nodes[0].empty()) { return nullptr; }

	int width = static_cast<int>(m_nodes[0].size());
	int height = static_cast<int>(m_nodes.size());

	if (x < 0 || y < 0 || x >= width || y >= height)
	{
		return nullptr;
	}

	return &m_nodes[y][x];
}

std::vector<Node*> MapManager::FindPath(Node* _start, Node* _goal)
{
	if (!_start || !_goal || !_start->m_walkable || !_goal->m_walkable) return {};
	if (_start == _goal) return { _start };

	// 探索IDをインクリメント（全ノードループ初期化を不要にする）
	m_currentSearchId++;
	if (m_currentSearchId == 0)
	{
		// uint32_t のオーバーフロー対策（42億回に1回）
		for (auto& row : m_nodes)
		{
			for (auto& node : row)
			{
				node.m_searchId = 0;
			}
		}
		m_currentSearchId = 1;
	}

	std::priority_queue<PathQueueItem, std::vector<PathQueueItem>, std::greater<PathQueueItem>> openQueue;

	// 開始ノードの初期化
	_start->m_searchId = m_currentSearchId;
	_start->m_gCost = 0.0f;
	_start->m_hCost = Heuristic(_start, _goal);
	_start->m_parent = nullptr;
	_start->m_inClosed = false;

	openQueue.push({ _start->FCost(), _start->m_hCost, _start });

	while (!openQueue.empty())
	{
		PathQueueItem item = openQueue.top();
		openQueue.pop();
		Node* current = item.m_node;

		// 既にクローズドリストに入っている（より良い経路で処理済み）ならスキップ
		if (current->m_inClosed && current->m_searchId == m_currentSearchId)
		{
			continue;
		}

		// ゴールに到達したら経路復元
		if (current == _goal)
		{
			return BuildPath(_goal);
		}

		current->m_inClosed = true;

		// 隣接ノードを取得 (周囲8方向)
		auto neighbors = GetNeighbors(current);

		for (auto* neighbor : neighbors)
		{
			if (!neighbor || !neighbor->m_walkable) continue;

			// 初めて訪問したノードなら初期化
			if (neighbor->m_searchId != m_currentSearchId)
			{
				neighbor->m_searchId = m_currentSearchId;
				neighbor->m_gCost = FLT_MAX;
				neighbor->m_inClosed = false;
				neighbor->m_parent = nullptr;
			}

			if (neighbor->m_inClosed) continue;

			// 直交か斜めかで移動コストを設定 (直交=1.0, 斜め=1.414)
			float stepCost = (current->m_pos.x != neighbor->m_pos.x && current->m_pos.y != neighbor->m_pos.y) ? 1.41421356f : 1.0f;
			float newCost = current->m_gCost + stepCost;

			// 新しいルートの方が安いなら更新
			if (newCost < neighbor->m_gCost)
			{
				neighbor->m_gCost = newCost;
				neighbor->m_hCost = Heuristic(neighbor, _goal);
				neighbor->m_parent = current;
				openQueue.push({ neighbor->FCost(), neighbor->m_hCost, neighbor });
			}
		}
	}

	// 経路なし
	return {};
}

std::vector<Node*> MapManager::GetNeighbors(Node* _node)
{
	std::vector<Node*> neighbors;
	if (!_node) return neighbors;

	int x = static_cast<int>(_node->m_pos.x);
	int y = static_cast<int>(_node->m_pos.y);
	int width = static_cast<int>(m_nodes[0].size());
	int height = static_cast<int>(m_nodes.size());

	// 周囲8方向のオフセット (上下左右 ＋ 斜め4方向)
	static const int dirX[8] = { 0,  0, -1,  1, -1,  1, -1,  1 };
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
				if (!m_nodes[y][nx].m_walkable || !m_nodes[ny][x].m_walkable)
				{
					continue;
				}
			}

			neighbors.push_back(&m_nodes[ny][nx]);
		}
	}

	return neighbors;
}

void MapManager::CreateNodeGrid(int _width, int _height, float _tileSize)
{
	// 1タイルあたり 3x3（中央＋周囲8マス = 全9ノード）に分割
	int gridWidth = _width;
	int gridHeight = _height;

	m_nodes.resize(gridHeight);
	for (int y = 0; y < gridHeight; y++)
	{
		m_nodes[y].resize(gridWidth);

		for (int x = 0; x < gridWidth; x++)
		{
			Node& node = m_nodes[y][x];

			// グリッド座標
			node.m_pos = Math::Vector2(static_cast<float>(x), static_cast<float>(y));

			// 初期値(通れないで初期化)
			node.m_walkable = false;

			// A* 用の初期化
			node.m_gCost = FLT_MAX;
			node.m_hCost = 0;
			node.m_parent = nullptr;
		}
	}
}

void MapManager::ApplyWalkableFromMap(const std::vector<std::vector<bool>>& _mapData)
{
	int height = static_cast<int>(_mapData.size());
	if (height == 0) return;
	int width = static_cast<int>(_mapData[0].size());
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			// 床ではない（壁/空きマス）場合はスキップ
			if (!_mapData[y][x]) continue;
			// 隣接マス（上下左右）が「マップ外」または「床ではない（＝壁がある）」かを判定
			bool wallNorth = (y == 0) || !_mapData[y - 1][x];          // 上側（Y-1）に壁
			bool wallSouth = (y == height - 1) || !_mapData[y + 1][x];  // 下側（Y+1）に壁
			bool wallWest = (x == 0) || !_mapData[y][x - 1];          // 左側（X-1）に壁
			bool wallEast = (x == width - 1) || !_mapData[y][x + 1];  // 右側（X+1）に壁
			m_nodes[y][x].m_walkable = _mapData[y][x];
			//// タイル内の 3x3 サブノードに対してフラグを設定
			//for (int dy = 0; dy < 3; dy++)
			//{
			//	for (int dx = 0; dx < 3; dx++)
			//	{
			//		int subX = x * 3 + dx;
			//		int subY = y * 3 + dy;
			//		if (subY >= static_cast<int>(m_nodes.size()) || subX >= static_cast<int>(m_nodes[subY].size()))
			//		{
			//			continue;
			//		}
			//		// 基本は通行可能 (true) に初期化
			//		bool isWalkable = true;
			//		// 壁が立っている側のサブノード列は通行不可 (false) に設定
			//		if (wallNorth && dy == 0) isWalkable = false; // 上端（dy = 0）
			//		if (wallSouth && dy == 2) isWalkable = false; // 下端（dy = 2）
			//		if (wallWest && dx == 0) isWalkable = false; // 左端（dx = 0）
			//		if (wallEast && dx == 2) isWalkable = false; // 右端（dx = 2）
			//		m_nodes[subY][subX].walkable = isWalkable;
			//	}
			//}
		}
	}
}

std::vector<Node*> MapManager::BuildPath(Node* _goal) const
{
	std::vector<Node*> path;
	Node* current = _goal;

	while (current != nullptr)
	{
		path.push_back(current);
		current = current->m_parent;
	}

	std::reverse(path.begin(), path.end());
	return path;
}
