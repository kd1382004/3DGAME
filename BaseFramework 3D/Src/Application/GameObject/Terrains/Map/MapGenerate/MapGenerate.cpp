#include "MapGenerate.h"
#include<array>
#include"../FloorBase/FloorBase.h"
#include"../WallBase/WallBase.h"
#include"../Stairs/StairsBase.h"

MapGenerate::MapGenerate()
{
	m_roomSizPath = "Asset/Data/ObjeData/Terrains/Map/MapSiz.json";

	LoadRoomSiz(m_roomSizPath);
}

std::vector<std::vector<int>> MapGenerate::Generate(Math::Vector2 _mapSiz, int roomNum, float tileSiz, MapType _type, std::list<std::shared_ptr<MapBase>>* ret, Math::Vector3* _playerSpawnPos)
{
	if (roomMine <= 0 && roomMax <= 0)
	{
		roomMax = 1;
		roomMine = 0;
	}

	m_roomNum = 0;

	m_roomInfo.clear();
	m_roomInfoList.clear();

	//マップのサイズを作る
	//TileType::Noneで初期化(何もない)
	std::vector<std::vector<int>> map(static_cast<size_t>(_mapSiz.y), std::vector<int>(static_cast<size_t>(_mapSiz.x), static_cast<int>(TileType::None)));
	std::vector<std::vector<int>> roomIDVector(static_cast<size_t>(_mapSiz.y), std::vector<int>(static_cast<size_t>(_mapSiz.x), -1));




	/////////////////////////////////////////////////////


	/////////////////////////////////////////////////////

	/////////////////////////////////////////////////////
	//部屋を作る

	// 1部屋につき最大50回試行、できなかったらスキップ
	int maxTry = 50;

	//部屋ID
	int roomID = m_ionitialRoomID;

	for (int i = 0; i < roomNum; i++)
	{
		for (int t = 0; t < maxTry; t++)
		{
			/////////////////////////////////////////////////////

			// 部屋のサイズ決定

			//横

			int roomW = KdRandom::GetInt(roomMine, roomMax);

			//縦
			int roomH = KdRandom::GetInt(roomMine, roomMax);

			//縦横のサイズを奇数にする(部屋の中心をわかりやすくするため)
			if (roomW % 2 == 0)
			{
				roomW += 1;
			}

			if (roomH % 2 == 0)
			{
				roomH += 1;
			}

			/////////////////////////////////////////////////////


			/////////////////////////////////////////////////////

			// 部屋の左上番号
			int X = _mapSiz.x;
			int Y = _mapSiz.y;

			int roomX = KdRandom::GetInt(0, X - roomW - 1);
			int roomY = KdRandom::GetInt(0, Y - roomH - 1);

			// 部屋が空いてるかチェック
			bool canPlace = true;

			//部屋同士何タイル開けるか
			int aX = KdRandom::GetInt(3, 8);
			int aY = KdRandom::GetInt(2, 5);

			for (int y = roomY - aY; y < roomY + roomH + aY; y++)
			{
				for (int x = roomX - aX; x < roomX + roomW + aX; x++)
				{
					//マップサイズ以上or以下ならスキップ
					if (y < 0 || y >= _mapSiz.y) continue;
					if (x < 0 || x >= _mapSiz.x) continue;

					if (map[y][x] != static_cast<int>(TileType::None))
					{
						canPlace = false;
						break;
					}
				}

				if (!canPlace) { break; }
			}

			// 空いてるなら作る
			if (canPlace)
			{
				for (int y = roomY; y < roomY + roomH; y++)
				{
					for (int x = roomX; x < roomX + roomW; x++)
					{
						map[y][x] = static_cast<int>(TileType::Room); // 部屋
						roomIDVector[y][x] = roomID; // 部屋

						//各部屋の中心を求める
						if ((roomH - 1) / 2 + roomY == y && (roomW - 1) / 2 + roomX == x)
						{
							//roomIDと中心とそれぞれの部屋の端を入れる
							RoomInfo info;

							info.m_roomID = roomID;//roomIDを入れる
							info.m_center = { (float)x,(float)y };

							//左右の情報を入れる
							info.m_roomEnd.FarRight = roomX + roomW - 1;
							info.m_roomEnd.FarLeft = roomX;

							//上下の情報を入れる
							info.m_roomEnd.topEnd = roomY;
							info.m_roomEnd.downEnd = roomY + roomH - 1;

							m_roomInfo.push_back(info);
						}

					}
				}

				m_roomNum++;
				roomID++;
				break; // この部屋は成功したので次の部屋へ
			}

			/////////////////////////////////////////////////////
		}
	}

	//部屋の数
	std::vector<RoomType> roomTypeList(roomID);

	for (int i = 0;i < roomID;i++)
	{
		//何用の部屋か決める
		roomTypeList[i] = (RoomType)KdRandom::GetInt((int)RoomType_EnemyRoom, (int)RoomType_SafeRoom);
	}


	/////////////////////////////////////////////////////

	//部屋のX値でソート(左から右へ)
	std::sort(m_roomInfo.begin(), m_roomInfo.end(), [](const RoomInfo& a, const RoomInfo& b) {return a.m_center.x < b.m_center.x;});

	/////////////////////////////////////////////////////


	/////////////////////////////////////////////////////

	std::vector<std::pair<RoomInfo, RoomInfo>> pairs = GetRoomConnectionPairs(m_roomInfo);

	for (auto& p : pairs)
	{
		std::vector<Math::Vector2> ans = GenerateCorridorPath(p.first, p.second);

		for (auto& a : ans)
		{

			if (map[(int)a.y][(int)a.x] == static_cast<int>(TileType::None))
			{
				map[(int)a.y][(int)a.x] = static_cast<int>(TileType::Floor);
			}

		}

	}


	/////////////////////////////////////////////////////
	//プレイヤーのスポーン部屋を決める
	int playerSpwanRoomID = KdRandom::GetInt(m_ionitialRoomID, roomID - 1);


	for (auto room : m_roomInfo)
	{
		if (room.m_roomID == playerSpwanRoomID)
		{
			room.m_playerSpwanRoom = true;
			break;
		}
	}


	/////////////////////////////////////////////////////
	//階段設置部屋を決める
	int SetStairsRoomID;
	while (true)
	{
		SetStairsRoomID = KdRandom::GetInt(m_ionitialRoomID, roomID - 1);
		if (playerSpwanRoomID != SetStairsRoomID)
		{
			for (auto room : m_roomInfo)
			{
				if (room.m_roomID == SetStairsRoomID)
				{
					room.m_stairsRoom = true;
				}
			}

			break;
		}
	}

	//階段設置したかどうか
	bool stairsPlaced=false;



	/////////////////////////////////////////////////////


	// 部屋ごとにタイル情報を管理するためリサイズ
	m_roomInfoList.resize(roomID);

	for (int y = 0; y < map.size(); y++)
	{
		for (int x = 0; x < map[y].size(); x++)
		{
			/////////////////////////////////////////////////////
			if (map[y][x] != static_cast<int>(TileType::None))
			{
				//床と通路を保存
				float xPos = -(_mapSiz.x / 2.0f * tileSiz);
				float zPos = (_mapSiz.y / 2.0f * tileSiz);

				std::shared_ptr<FloorBase> mapA = std::make_shared<FloorBase>();

				xPos += tileSiz * x;
				zPos -= tileSiz * y;

				Math::Vector3 pos = { xPos,0,zPos };

				mapA->Init();
				mapA->SetPos(pos);
				mapA->SetMapObjType(MapObjType::Ground);

				if (map[y][x] == static_cast<int>(TileType::Floor))
				{
					mapA->SetGroundType(GroundType::Floor);
				}
				else if (map[y][x] == static_cast<int>(TileType::Room))
				{
					mapA->SetGroundType(GroundType::Room);
					int rID = roomIDVector[y][x];
					mapA->SerRoomID(rID);

					//プレイヤースポーン位置を保存
					if (rID == playerSpwanRoomID)
					{
						*_playerSpawnPos = pos;
					}
					else
					{
						mapA->SetRoomType(roomTypeList[rID]);


						// 部屋ごとに部屋IDとワールド座標情報を保存
						if (rID >= 0 && rID < static_cast<int>(m_roomInfoList.size()))
						{
							RoomI room;
							room.m_roomID = rID;
							room.m_pos = pos;
							room.m_roomType = static_cast<int>(roomTypeList[rID]);


							m_roomInfoList[rID].push_back(room);
						}

					}
				}



				ret->push_back(mapA);




				//壁を生成
				// 4方向の定義データ構造
				struct WallDirectionInfo
				{
					int dx, dy;              // マップインデックスの移動量
					Math::Vector3 offset;    // 座標オフセット
					float rotY;              // Y軸回転角度
					bool allowStairs;        // 階段の生成を許可するか
				};

				const float halfTile = tileSiz / 2.0f;
				// 上、下、左、右の定義
				std::vector<WallDirectionInfo> wallDirs = {
				{  0, -1, { 0.0f, 0.0f,  halfTile },   0.0f, true }, // 上 (0度)
				{  0,  1, { 0.0f, 0.0f, -halfTile }, 180.0f, true }, // 下 (180度)
				{ -1,  0, {-halfTile, 0.0f, 0.0f  }, 270.0f, true }, // 左 (270度)
				{  1,  0, { halfTile, 0.0f, 0.0f  }, 90.0f, true }, // 右 (90度)
				};

				bool isStairsRoom = (roomIDVector[y][x] == SetStairsRoomID);


				//階段の設定位置をランダムにしたいから配列シャッフル
				for (size_t i = 0; i < wallDirs.size(); ++i)
				{
					int rndIndex = KdRandom::GetInt(0, wallDirs.size() - 1);
					std::swap(wallDirs[i], wallDirs[rndIndex]); 
				}

				for (const auto& dir : wallDirs)
				{
					int nx = x + dir.dx;
					int ny = y + dir.dy;
					// 隣接マスに壁が必要か判定
					if (IsNeedWall(nx, ny, map))
					{
						// まだ階段が設置されておらず、階段部屋かつ許可方向の場合のみ階段を作る
						bool createStairs = dir.allowStairs && isStairsRoom && !stairsPlaced;
						if (createStairs)
						{
							stairsPlaced = true; // 1つ作ったらフラグを立てて2つ目以降を作らない
						}

						Math::Vector3 wallPos = { xPos + dir.offset.x, 0.0f, zPos + dir.offset.z };
						CreateWallOrStairs(wallPos, dir.rotY, createStairs, ret);
					}
				}
			}
			/////////////////////////////////////////////////////
		}
	}

	// 部屋の生成・全タイル保存完了後、部屋ごとの敵出現数を一括計算
	for (size_t i = 0; i < m_roomInfoList.size(); i++)
	{
		if (m_roomInfoList[i].empty()) { continue; }

		for (size_t j = 0; j < m_roomInfoList[i].size(); j++)
		{
			switch (m_roomInfoList[i][j].m_roomType)
			{
			case RoomType_EnemyRoom:
				m_roomInfoList[i][j].m_roomEnemyNum = static_cast<int>(m_roomInfoList[i].size() * m_roomEnemyPercent.m_EnemyRoom);
				break;
			case RoomType_TreasureChestRoom:
			case RoomType_TrapRoom:
				m_roomInfoList[i][j].m_roomEnemyNum = static_cast<int>(m_roomInfoList[i].size() * m_roomEnemyPercent.m_NotEnemyRoom);
				if (m_roomInfoList[i][j].m_roomEnemyNum == 0) { m_roomInfoList[i][j].m_roomEnemyNum = 1; }
				break;
			case RoomType_SafeRoom:
				m_roomInfoList[i][j].m_roomEnemyNum = static_cast<int>(m_roomInfoList[i].size() * m_roomEnemyPercent.m_SafeRoom);
				break;
			default:
				break;
			}
		}
	}


	return map;
}
 

void MapGenerate::LoadRoomSiz(std::string _filePath)
{
	std::ifstream ifs(_filePath);
	if (!ifs.is_open())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] 部屋サイズファイルが見つかりません : %s\n"), _filePath.c_str());
		return;
	}

	nlohmann::json data = nlohmann::json::parse(ifs, nullptr, false);

	if (data.is_discarded())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] 部屋サイズファイルの読み込みに失敗しました : %s\n"), _filePath.c_str());
		return;
	}

	// JSONから部屋サイズを取得
	roomMine = data["room"]["sizeMin"].get<int>();
	roomMax = data["room"]["sizeMax"].get<int>();

}

void MapGenerate::SaveRoomSiz(std::string _filePath)
{
	;
	nlohmann::json data;
	data["room"]["sizeMin"] = roomMine;
	data["room"]["sizeMax"] = roomMax;

	// 保存（書き込み）
	std::ofstream ofs(_filePath);
	ofs << data.dump(4);
}

std::vector<std::pair<RoomInfo, RoomInfo>> MapGenerate::GetRoomConnectionPairs(const std::vector<RoomInfo>& _roomInfo)
{
	//マンハッタン距離でペアの距離を計算
	struct Edge
	{
		// 部屋A(i番目)
		int roomA;

		// 部屋B(j番目)
		int roomB;

		// 部屋Aと部屋Bの距離
		float dist;
	};

	//すべてのペアのリストを入れる
	std::vector<Edge> edges;
	if (_roomInfo.size() > 1)
	{
		edges.reserve(_roomInfo.size() * (_roomInfo.size() - 1) / 2);
	}

	for (size_t i = 0; i < _roomInfo.size() - 1; i++)
	{
		for (size_t j = i + 1; j < _roomInfo.size(); j++)
		{
			float dist = std::abs(_roomInfo[i].m_center.x - _roomInfo[j].m_center.x) + std::abs(_roomInfo[i].m_center.y - _roomInfo[j].m_center.y);

			Edge edge;
			edge.roomA = static_cast<int>(i);
			edge.roomB = static_cast<int>(j);
			edge.dist = dist;
			edges.push_back(edge);
		}
	}

	//distが短い順に並べる
	std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {return a.dist < b.dist; });

	//return用
	std::vector<std::pair<RoomInfo, RoomInfo>> pairList;
	pairList.reserve(_roomInfo.size());

	std::vector<int> parent(_roomInfo.size());
	for (size_t i = 0; i < _roomInfo.size(); i++)
	{
		parent[i] = static_cast<int>(i);
	}

	//すべてのペアのリストを入れる
	std::vector<Edge> loopEdges;

	// 距離の短い順に Edge を見ていく
	for (const auto& e : edges)
	{
		int rootA = FindRoot(parent, e.roomA);
		int rootB = FindRoot(parent, e.roomB);

		// ループしないなら採用
		if (rootA != rootB)
		{
			pairList.push_back({ _roomInfo[e.roomA], _roomInfo[e.roomB] });

			// グループ結合
			UnionSet(parent, rootA, rootB);
		}
		else
		{
			loopEdges.push_back(e);
		}
	}

	// 部屋Aと部屋Bの距離が短い順にする
	std::sort(loopEdges.begin(), loopEdges.end(), [](const Edge& a, const Edge& b) {return a.dist < b.dist; });

	/////////////////////////
	//上位30%だけ残す (resize で一括処理し超高速化)
	size_t siz = static_cast<size_t>(loopEdges.size() * 0.3f);
	if (loopEdges.size() > siz)
	{
		loopEdges.resize(siz);
	}
	/////////////////////////

	int count = 0;
	if (!loopEdges.empty())
	{
		float maxDist = loopEdges.back().dist;
		for (const auto& e : loopEdges)
		{
			float percent = (maxDist > 0.0f) ? (1.0f - e.dist / maxDist) * 100.0f : 0.0f;

			if (KdRandom::GetInt(1, 100) < percent && count < 5)
			{
				count++;
				pairList.push_back({ _roomInfo[e.roomA], _roomInfo[e.roomB] });
			}
		}
	}

	return pairList;
}

std::vector<Math::Vector2> MapGenerate::GenerateCorridorPath(const RoomInfo& _A, const RoomInfo& _B)
{
	//return用
	std::vector<Math::Vector2> ans;

	// A の端候補 (std::array を使用して動的メモリ割り当てを排除)
	float roomHA = (_A.m_roomEnd.topEnd + _A.m_roomEnd.downEnd) / 2.0f;
	float roomWA = (_A.m_roomEnd.FarLeft + _A.m_roomEnd.FarRight) / 2.0f;
	std::array<Math::Vector2, 4> Aends = {
		Math::Vector2{ (float)_A.m_roomEnd.FarLeft, roomHA },
		Math::Vector2{ (float)_A.m_roomEnd.FarRight, roomHA },
		Math::Vector2{ roomWA, (float)_A.m_roomEnd.topEnd },
		Math::Vector2{ roomWA, (float)_A.m_roomEnd.downEnd }
	};

	// B の端候補
	float roomHB = (_B.m_roomEnd.topEnd + _B.m_roomEnd.downEnd) / 2.0f;
	float roomWB = (_B.m_roomEnd.FarLeft + _B.m_roomEnd.FarRight) / 2.0f;
	std::array<Math::Vector2, 4> Bends = {
		Math::Vector2{ (float)_B.m_roomEnd.FarLeft, roomHB },
		Math::Vector2{ (float)_B.m_roomEnd.FarRight, roomHB },
		Math::Vector2{ roomWB, (float)_B.m_roomEnd.topEnd },
		Math::Vector2{ roomWB, (float)_B.m_roomEnd.downEnd }
	};

	// 最短端ペアを探す
	float bestDist = FLT_MAX;
	Math::Vector2 bestA{}, bestB{};

	for (const auto& a : Aends)
	{
		for (const auto& b : Bends)
		{
			float dist = std::abs(a.x - b.x) + std::abs(a.y - b.y);
			if (dist < bestDist)
			{
				bestDist = dist;
				bestA = a;
				bestB = b;
			}
		}
	}

	// L字通路を作る（横 → 縦）
	int xStart = (int)bestA.x;
	int xEnd = (int)bestB.x;
	int yMid = (int)bestA.y;

	int xStep = (xStart <= xEnd) ? 1 : -1;
	for (int x = xStart; x != xEnd + xStep; x += xStep)
	{
		ans.push_back({ (float)x, (float)yMid });
	}

	// 縦方向
	int yStart = (int)bestA.y;
	int yEnd = (int)bestB.y;
	int xMid = (int)bestB.x;

	int yStep = (yStart <= yEnd) ? 1 : -1;
	for (int y = yStart; y != yEnd + yStep; y += yStep)
	{
		ans.push_back({ (float)xMid, (float)y });
	}

	return ans;
}

// 親(root)を返す
int MapGenerate::FindRoot(std::vector<int>& _parent, int x)
{
	// 親が自分自身なら root
	if (_parent[x] == x)
	{
		return x;
	}

	// 親の親をたどる（経路圧縮）
	_parent[x] = FindRoot(_parent, _parent[x]);
	return _parent[x];
}

// グループを結合する
void MapGenerate::UnionSet(std::vector<int>& _parent, int _a, int _b)
{
	int rootA = FindRoot(_parent, _a);
	int rootB = FindRoot(_parent, _b);

	// すでに同じグループなら何もしない
	if (rootA == rootB)
	{
		return;
	}

	// 結合（親にする）
	_parent[rootB] = rootA;
}

bool MapGenerate::IsNeedWall(int nx, int ny, const std::vector<std::vector<int>>& map)
{
	// マップ範囲外なら壁を作る
	if (ny < 0 || ny >= static_cast<int>(map.size())) { return true; }
	if (nx < 0 || nx >= static_cast<int>(map[ny].size())) { return true; }

	// 隣のマスが None（空き地）なら壁を作る
	if(map[ny][nx] == static_cast<int>(TileType::None)){ return true;}

	return false;
}

void MapGenerate::CreateWallOrStairs(const Math::Vector3& pos, float rotYDegree, bool isStairs, std::list<std::shared_ptr<MapBase>>* ret)
{
	if (isStairs)
	{
		auto stairs = std::make_shared<StairsBase>();
		stairs->Init();
		stairs->SetPos(pos);
		if (rotYDegree != 0.0f)
		{
			stairs->SetRotation(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(rotYDegree)));
		}

		ret->push_back(stairs);
	}
	else
	{
		auto wall = std::make_shared<WallBase>();
		wall->Init();
		wall->SetPos(pos);
		if (rotYDegree != 0.0f)
		{
			wall->SetRotation(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(rotYDegree)));
		}

		wall->SetMapObjType(MapObjType::Wall);
		ret->push_back(wall);
	}
}


