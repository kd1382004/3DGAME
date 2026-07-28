#include "MapGenerate.h"
#include"../FloorBase/FloorBase.h"
#include"../WallBase/WallBase.h"
#include"../Stairs/StairsBase.h"

MapGenerate::MapGenerate()
{
	m_roomSizPath = "Asset/Data/ObjeData/Terrains/Map/MapSiz.json";

	LoadRoomSiz(m_roomSizPath);
}

void MapGenerate::Generate(Math::Vector2 _mapSiz, int roomNum, float tileSiz, MapType _type, std::list<std::shared_ptr<MapBase>>* ret, Math::Vector3* _playerSpawnPos)
{
	if (roomMine <= 0 && roomMax <= 0)
	{
		roomMax = 1;
		roomMine = 0;
	}

	m_roomInfo.clear();

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


				roomID++;
				break; // この部屋は成功したので次の部屋へ
			}

			/////////////////////////////////////////////////////
		}
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
					mapA->SerRoomID(roomIDVector[y][x]);


					//プレイヤースポーン位置を保存
					if (roomIDVector[y][x] == playerSpwanRoomID)
					{
						*_playerSpawnPos = pos;
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

std::vector<std::pair<RoomInfo, RoomInfo>> MapGenerate::GetRoomConnectionPairs(std::vector<RoomInfo> _roomInfo)
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

	for (int i = 0;i < _roomInfo.size() - 1;i++)
	{
		for (int j = i + 1;j < _roomInfo.size();j++)
		{
			float dist = abs(_roomInfo[i].m_center.x - _roomInfo[j].m_center.x) + abs(_roomInfo[i].m_center.y - _roomInfo[j].m_center.y);

			Edge edge;
			edge.roomA = i;
			edge.roomB = j;
			edge.dist = dist;
			edges.push_back(edge);
		}
	}

	//distが短い順に並べる
	std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {return a.dist < b.dist;});

	//return用
	std::vector<std::pair<RoomInfo, RoomInfo>> pairList;

	std::vector<int> parent;

	parent.resize(_roomInfo.size());

	for (int i = 0; i < _roomInfo.size(); i++)
	{
		parent[i] = i;
	}

	//すべてのペアのリストを入れる
	std::vector<Edge> loopEdges;

	// 距離の短い順に Edge を見ていく
	for (auto& e : edges)
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
	std::sort(loopEdges.begin(), loopEdges.end(), [](const Edge& a, const Edge& b) {return a.dist < b.dist;});

	/////////////////////////
	//上位30%だけ残す
	int siz = loopEdges.size() * 0.3;

	auto it = loopEdges.begin() + siz;

	while (it != loopEdges.end())
	{
		// 無効なオブジェクトをリストから削除
		it = loopEdges.erase(it);
	}
	//上位30%だけ残す
	/////////////////////////

	int i = 0;

	float percent;
	for (auto& e : loopEdges)
	{

		percent = (1 - e.dist / loopEdges.back().dist) * 100;

		if (KdRandom::GetInt(1, 100) < percent && i < 5)
		{
			i++;
			pairList.push_back({ _roomInfo[e.roomA], _roomInfo[e.roomB] });
		}

	}


	return pairList;
}

std::vector<Math::Vector2> MapGenerate::GenerateCorridorPath(RoomInfo _A, RoomInfo _B)
{
	//return用
	std::vector<Math::Vector2> ans;



	// A の端候補
	std::vector<Math::Vector2> Aends;
	float roomH = (float)((_A.m_roomEnd.topEnd + _A.m_roomEnd.downEnd) / 2.0f);
	float roomW = (float)((_A.m_roomEnd.FarLeft + _A.m_roomEnd.FarRight) / 2.0f);
	Aends.push_back({ (float)_A.m_roomEnd.FarLeft , roomH });
	Aends.push_back({ (float)_A.m_roomEnd.FarRight, roomH });
	Aends.push_back({ roomW , (float)_A.m_roomEnd.topEnd });
	Aends.push_back({ roomW, (float)_A.m_roomEnd.downEnd });

	// B の端候補
	std::vector<Math::Vector2> Bends;
	roomH = (float)((_B.m_roomEnd.topEnd + _B.m_roomEnd.downEnd) / 2.0f);
	roomW = (float)((_B.m_roomEnd.FarLeft + _B.m_roomEnd.FarRight) / 2.0f);
	Bends.push_back({ (float)_B.m_roomEnd.FarLeft , roomH });
	Bends.push_back({ (float)_B.m_roomEnd.FarRight, roomH });
	Bends.push_back({ roomW , (float)_B.m_roomEnd.topEnd });
	Bends.push_back({ roomW, (float)_B.m_roomEnd.downEnd });


	// 最短端ペアを探す
	float bestDist = FLT_MAX;
	Math::Vector2 bestA, bestB;

	for (auto& a : Aends)
	{
		for (auto& b : Bends)
		{
			float dist = fabs(a.x - b.x) + fabs(a.y - b.y);
			if (dist < bestDist)
			{
				bestDist = dist;
				bestA = a;
				bestB = b;
			}
		}
	}

	// L字通路を作る（横 → 縦）
	// 横方向
	int xStart = (int)bestA.x;
	int xEnd = (int)bestB.x;
	int yMid = (int)bestA.y;

	if (xStart <= xEnd)
	{
		for (int x = xStart; x <= xEnd; x++)
		{
			ans.push_back({ (float)x, (float)yMid });
		}
	}
	else
	{
		for (int x = xStart; x >= xEnd; x--)
		{
			ans.push_back({ (float)x, (float)yMid });
		}
	}

	// 縦方向
	int yStart = (int)bestA.y;
	int yEnd = (int)bestB.y;
	int xMid = (int)bestB.x;

	if (yStart <= yEnd)
	{
		for (int y = yStart; y <= yEnd; y++)
		{
			ans.push_back({ (float)xMid, (float)y });
		}
	}
	else
	{
		for (int y = yStart; y >= yEnd; y--)
		{
			ans.push_back({ (float)xMid, (float)y });
		}
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

