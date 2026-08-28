#include "MapGenerate.h"
#include<array>
#include"../FloorBase/FloorBase.h"
#include"../WallBase/WallBase.h"
#include"../Stairs/StairsBase.h"
#include"../Slope/Slope.h"
//
#include"../../MapObj/MapObjManager.h"
#include"../../MapObj/Torch/Torch.h"
MapGenerate::MapGenerate()
{
	m_roomSizPath = "Asset/Data/ObjeData/Terrains/Map/MapSiz.json";

	LoadRoomSiz(m_roomSizPath);


	if (!m_spFloorModel)
	{
		m_spFloorModel = std::make_shared<KdModelWork>();
		m_spFloorModel->SetModelData("Asset/Models/Terrains/Map/Floor/Base.gltf");
	}

	if (!m_spSlopeModel)
	{
		m_spSlopeModel = std::make_shared<KdModelWork>();
		m_spSlopeModel->SetModelData("Asset/Models/Terrains/Map/Castle/Slope/Slope.gltf");
	}

	if (!m_spWallModel)
	{
		m_spWallModel = std::make_shared<KdModelWork>();
		m_spWallModel->SetModelData("Asset/Models/Terrains/Map/Wall/Base.gltf");
	}

	if (!m_spStairsModel)
	{
		m_spStairsModel = std::make_shared<KdModelWork>();
		m_spStairsModel->SetModelData("Asset/Models/Terrains/Map/Stairs/Stairs.gltf");
	}
}

std::vector<std::vector<bool>> MapGenerate::Generate(Math::Vector2 _mapSiz, int roomNum, float tileSiz, int _type, std::list<std::shared_ptr<MapBase>>* ret, Math::Vector3* _playerSpawnPos, Math::Vector3* _basePos)
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
	std::vector<std::vector<FloorInfo>> map(static_cast<size_t>(_mapSiz.y), std::vector<FloorInfo>(static_cast<size_t>(_mapSiz.x)));
	std::vector<std::vector<int>> roomIDVector(static_cast<size_t>(_mapSiz.y), std::vector<int>(static_cast<size_t>(_mapSiz.x), -1));

	//戻り値用のデータ
	std::vector<std::vector<bool>> returnMapDate(static_cast<size_t>(_mapSiz.y), std::vector<bool>(static_cast<size_t>(_mapSiz.x), false));




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
			int aX = KdRandom::GetInt(5, 8);
			int aY = KdRandom::GetInt(5, 8);

			for (int y = roomY - aY; y < roomY + roomH + aY; y++)
			{
				for (int x = roomX - aX; x < roomX + roomW + aX; x++)
				{
					//マップサイズ以上or以下ならスキップ
					if (y < 0 || y >= _mapSiz.y) continue;
					if (x < 0 || x >= _mapSiz.x) continue;

					if (map[y][x].m_tileType != TileType::None)
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
				int heightLevel = KdRandom::GetInt(0, m_heightLevelMax);

				for (int y = roomY; y < roomY + roomH; y++)
				{
					for (int x = roomX; x < roomX + roomW; x++)
					{
						map[y][x].m_tileType = TileType::Room; // 部屋
						map[y][x].m_heightLevel = heightLevel;
						roomIDVector[y][x] = roomID; // 部屋ID

						//各部屋の中心を求める
						if ((roomH - 1) / 2 + roomY == y && (roomW - 1) / 2 + roomX == x)
						{
							//roomIDと中心とそれぞれの部屋の端を入れる
							RoomInfo info;

							info.m_roomID = roomID;//roomIDを入れる
							info.m_center = { (float)x,(float)y };

							//2D上の左右の情報を入れる
							info.m_roomEnd.FarRight = roomX + roomW - 1;
							info.m_roomEnd.FarLeft = roomX;

							//2D上の上下の情報を入れる
							info.m_roomEnd.topEnd = roomY;
							info.m_roomEnd.downEnd = roomY + roomH - 1;


							//3D上の上下(何階)
							info.m_heightLevel = heightLevel;


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

	/////////////////////////////////////////////////////
	//それぞれの数とかのちに調整
	for (int i = 0; i < roomID; i++)
	{
		//何用の部屋か決める
		roomTypeList[i] = (RoomType)KdRandom::GetInt((int)RoomType_EnemyRoom, (int)RoomType_TrapRoom);
	}

	/////////////////////////////////////////////////////

	/////////////////////////////////////////////////////

	//部屋のX値でソート(左から右へ)
	//std::sort(m_roomInfo.begin(), m_roomInfo.end(), [](const RoomInfo& a, const RoomInfo& b) {return a.m_center.x < b.m_center.x;});


	/////////////////////////////////////////////////////


	/////////////////////////////////////////////////////

	std::vector<std::pair<RoomInfo, RoomInfo>> pairs = GetRoomConnectionPairs(m_roomInfo);

	for (auto& p : pairs)
	{
		std::vector<Math::Vector3> ans = GenerateCorridorPath(p.first, p.second);

		for (auto& a : ans)
		{
			int x = static_cast<int>(a.x);
			int y = static_cast<int>(a.y);

			//   範囲外チェックを追加
			if (y >= 0 && y < static_cast<int>(map.size()) &&
				x >= 0 && x < static_cast<int>(map[y].size()))
			{
				if (map[y][x].m_tileType == TileType::None)
				{
					map[y][x].m_tileType = TileType::Floor;
					map[y][x].m_heightLevel = a.z;



					int height = static_cast<int>(map.size());
					int width = static_cast<int>(map[y].size());

					auto isValid = [&](int nx, int ny) {
						return ny >= 0 && ny < height && nx >= 0 && nx < width;
						};

					if (isValid(x, y + 1))
					{
						if (map[y + 1][x].m_tileType == TileType::Room)
						{
							map[y][x].m_heightLevel = map[y + 1][x].m_heightLevel;
						}
					}

					if (isValid(x, y - 1))
					{
						if (map[y - 1][x].m_tileType == TileType::Room)
						{
							map[y][x].m_heightLevel = map[y - 1][x].m_heightLevel;
						}
					}


					if (isValid(x + 1, y))
					{
						if (map[y][x + 1].m_tileType == TileType::Room)
						{
							map[y][x].m_heightLevel = map[y][x + 1].m_heightLevel;
						}
					}

					if (isValid(x - 1, y))
					{
						if (map[y][x - 1].m_tileType == TileType::Room)
						{
							map[y][x].m_heightLevel = map[y][x - 1].m_heightLevel;
						}
					}

				}
			}
		}
	}



	//Slopeを設置
	SlopeCheck(&map);


	/////////////////////////////////////////////////////
	//プレイヤーのスポーン部屋を決める
	int playerSpwanRoomID = KdRandom::GetInt(m_ionitialRoomID, roomID - 1);


	for (auto room : m_roomInfo)
	{
		if (room.m_roomID == playerSpwanRoomID)
		{
			room.m_playerSpwanRoom = true;
			roomTypeList[room.m_roomID] = RoomType_PlayerSpawn;
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
	bool stairsPlaced = false;



	/////////////////////////////////////////////////////


	// 部屋ごとにタイル情報を管理するためリサイズ
	m_roomInfoList.resize(roomID);
	int rID = 0;
	for (int y = 0; y < map.size(); y++)
	{
		for (int x = 0; x < map[y].size(); x++)
		{
			/////////////////////////////////////////////////////
			if (map[y][x].m_tileType != TileType::None)
			{
				returnMapDate[y][x] = true;


				//床と通路を保存
				float xPos = tileSiz * x + tileSiz * 0.5f;
				float yPos = tileSiz * map[y][x].m_heightLevel + tileSiz * 0.5f;
				float zPos = -(tileSiz * y + tileSiz * 0.5f);


				if (y == 0 && x == 0)
				{
					*_basePos = { xPos,0,zPos };
				}

				std::shared_ptr<FloorBase> mapA = std::make_shared<FloorBase>();


				Math::Vector3 pos = { xPos,yPos,zPos };
				mapA->SetModel(m_spFloorModel);
				mapA->Init();
				mapA->SetPos(pos);
				mapA->SetMapObjType(MapObjType::Ground);

				if (map[y][x].m_tileType == TileType::Floor)
				{
					mapA->SetGroundType(GroundType::Floor);
				}
				else if (map[y][x].m_tileType == TileType::Room)
				{
					mapA->SetGroundType(GroundType::Room);
					rID = roomIDVector[y][x];
					mapA->SerRoomID(rID);

					//プレイヤースポーン位置を保存
					if (rID == playerSpwanRoomID)
					{
						mapA->SetRoomType(RoomType_SafeRoom);
					}
					else
					{
						mapA->SetRoomType(roomTypeList[rID]);
					}


					// 部屋ごとに部屋IDとワールド座標情報を保存
					if (rID >= 0 && rID < static_cast<int>(m_roomInfoList.size()))
					{
						RoomI room;
						room.m_roomID = rID;
						room.m_pos = pos;
						room.m_roomType = static_cast<int>(roomTypeList[rID]);
						room.m_xy = { (float)x,(float)y };
						room.m_Installation = false;





						//通路につながってるか確認
						if (y - 1 >= 0 && y + 1 < map.size() &&
							x - 1 >= 0 && x + 1 < map[y].size())
						{
							//通路につながってたら設置物がある判定に
							if (map[y - 1][x].m_tileType == TileType::Floor)
							{
								room.m_Installation = true;
							}
							if (map[y + 1][x].m_tileType == TileType::Floor)
							{
								room.m_Installation = true;
							}
							if (map[y][x - 1].m_tileType == TileType::Floor)
							{
								room.m_Installation = true;
							}
							if (map[y][x + 1].m_tileType == TileType::Floor)
							{
								room.m_Installation = true;
							}
						}


						m_roomInfoList[rID].push_back(room);

					}



				}
				
				if (map[y][x].m_tileType == TileType::Slopee)
				{
					std::shared_ptr<Slope>spSlope = std::make_shared<Slope>();
					spSlope->SetModel(m_spSlopeModel);
					spSlope->Init();
					spSlope->SetPos(pos);
					spSlope->SetMapObjType(MapObjType::TypeSlope);

					float rotYDegree = map[y][x].m_angle;

					if (rotYDegree != 0.0f)
					{
						spSlope->SetRotation(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(rotYDegree)));
					}


					ret->push_back(spSlope);
					
				}
				else
				{
					ret->push_back(mapA);
				}







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
					if (IsNeedWall(nx, ny, map, map[y][x].m_heightLevel,x,y))
					{
						int startH = map[y][x].m_heightLevel;
						float startYPos = tileSiz * startH + tileSiz * 0.5f;
						Math::Vector3 wallPos = { xPos + dir.offset.x, startYPos, zPos + dir.offset.z };

						if (map[y][x].m_tileType == TileType::Slopee)
						{
							wallPos.y -= tileSiz;
							CreateWallOrStairs(wallPos, dir.rotY, false, ret, rID, x, y, map);
							wallPos.y += tileSiz;
						}

						//置かれたか
						bool torchFlg = false;

						for (int i = startH; i <= m_heightLevelMax; i++)
						{
							bool createStairs = dir.allowStairs && isStairsRoom && !stairsPlaced;
							if (createStairs)
							{
								stairsPlaced = true;
							}

							if (i != startH)
							{
								torchFlg = true;
							}

							CreateWallOrStairs(wallPos, dir.rotY, createStairs, ret, rID, x, y, map, &torchFlg);
							wallPos.y += tileSiz;
						}



					}
				}
			}
			/////////////////////////////////////////////////////
		}
	}




	bool playerSpwan = false;
	//部屋ごとの色々を計算
	for (size_t i = 0; i < m_roomInfoList.size(); i++)
	{
		if (m_roomInfoList[i].empty()) { continue; }

		if (!playerSpwan)
		{
			//プレイヤーのスポーン位置
			if (m_roomInfoList[i][0].m_roomID == playerSpwanRoomID)
			{
				int j = KdRandom::GetInt(0, m_roomInfoList[i].size() - 1);

				m_roomInfoList[i][j].m_Installation = true;
				*_playerSpawnPos = m_roomInfoList[i][j].m_pos;
				_playerSpawnPos->y += 1;
			}
		}



		for (size_t j = 0; j < m_roomInfoList[i].size(); j++)
		{
			//////////////////////////////////////////////////////
			//部屋ごとの敵出現数を計算
			switch (m_roomInfoList[i][j].m_roomType)
			{
			case RoomType_EnemyRoom:
				m_roomInfoList[i][j].m_roomEnemyNum = static_cast<int>(m_roomInfoList[i].size() * m_roomEnemyPercent.m_EnemyRoom);
				if (m_roomInfoList[i][j].m_roomEnemyNum == 0) { m_roomInfoList[i][j].m_roomEnemyNum = 1; }
				break;
			case RoomType_TreasureChestRoom:
			case RoomType_TrapRoom:
				m_roomInfoList[i][j].m_roomEnemyNum = static_cast<int>(m_roomInfoList[i].size() * m_roomEnemyPercent.m_NotEnemyRoom);
				if (m_roomInfoList[i][j].m_roomEnemyNum == 0) { m_roomInfoList[i][j].m_roomEnemyNum = 1; }
				break;
			case RoomType_SafeRoom:
			case RoomType_PlayerSpawn:
				m_roomInfoList[i][j].m_roomEnemyNum = static_cast<int>(m_roomInfoList[i].size() * m_roomEnemyPercent.m_SafeRoom);
				break;
			default:
				break;
			}



			//////////////////////////////////////////////////////
			// 宝箱の数を計算
			float num = 0.0f;
			float mine = 0.0f;
			float max = 0.0f;

			switch (m_roomInfoList[i][j].m_roomType)
			{
			case RoomType_TreasureChestRoom:
				num = static_cast<float>(m_roomInfoList[i].size()) * m_roomTreasuerChestPercent.m_TreasuerChestRoom;
				mine = m_roomTreasuerChestNum.m_TreasuerChestRoomMineNum;
				max = m_roomTreasuerChestNum.m_TreasuerChestRoomMaxNum;
				break;

			default:
				num = static_cast<float>(m_roomInfoList[i].size()) * m_roomTreasuerChestPercent.m_NotTreasuerChestRoom;
				mine = m_roomTreasuerChestNum.m_NotTreasuerChestRoomMineNum;
				max = m_roomTreasuerChestNum.m_NotTreasuerChestRoomMaxNum;
				break;
			}

			num = std::clamp(num, mine, max);
			m_roomInfoList[i][j].m_roomTreasuerChestNum = static_cast<int>(num);

		}
	}

	return returnMapDate;
}

std::vector<std::vector<bool>> MapGenerate::GenerateBoss(Math::Vector2 _mapSiz, float tileSiz, int _type, std::list<std::shared_ptr<MapBase>>* ret, Math::Vector3* _playerSpawnPos, Math::Vector3* _basePos)
{
	m_roomInfo.clear();
	m_roomInfoList.clear();
	m_roomInfoList.resize(1);
	if ((int)_mapSiz.x % 2 == 0)
	{
		_mapSiz.x++;
	}

	if ((int)_mapSiz.y % 2 == 0)
	{
		_mapSiz.y++;
	}

	//マップのサイズを作る
	//TileType::Roomで初期化(ボス戦部屋は巨大な一部屋)
	FloorInfo defaultTile;
	defaultTile.m_tileType = TileType::Room;
	defaultTile.m_heightLevel = 0;

	std::vector<std::vector<FloorInfo>> map(
		static_cast<size_t>(_mapSiz.y),
		std::vector<FloorInfo>(static_cast<size_t>(_mapSiz.x), defaultTile)
	);
	std::vector<std::vector<bool>> mapDate(static_cast<size_t>(_mapSiz.y), std::vector<bool>(static_cast<size_t>(_mapSiz.x), true));

	int centerX = _mapSiz.x / 2;
	int centerY = _mapSiz.y / 2;

	for (int y = 0; y < map.size(); y++)
	{
		for (int x = 0; x < map[y].size(); x++)
		{

			map[y][x].m_tileType = TileType::Room;

			//床保存
			float xPos = tileSiz * x + tileSiz * 0.5f;
			float yPos = tileSiz * map[y][x].m_heightLevel + tileSiz * 0.5f;
			float zPos = -(tileSiz * y + tileSiz * 0.5f);


			if (x == centerX && y == map.size() - 1)
			{
				*_playerSpawnPos = { xPos ,0,zPos };
			}

			if (x == centerX && y == centerY)
			{
				m_bossSpawnPos = { xPos ,0,zPos };
			}



			if (y == 0 && x == 0)
			{
				*_basePos = { xPos,0,zPos };
			}




			std::shared_ptr<FloorBase> mapA = std::make_shared<FloorBase>();


			Math::Vector3 pos = { xPos,0,zPos };

			mapA->Init();
			mapA->SetPos(pos);
			mapA->SetMapObjType(MapObjType::Ground);
			mapA->SetGroundType(GroundType::Room);
			mapA->SerRoomID(0);
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
				if (IsNeedWall(nx, ny, map, map[y][x].m_heightLevel, x, y))
				{
					Math::Vector3 wallPos = { xPos + dir.offset.x, 0, zPos + dir.offset.z };
					CreateWallOrStairs(wallPos, dir.rotY, false, ret, 0, x, y, map);
				}
			}
		}
	}

	return mapDate;
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
	//最低3個は残す
	size_t minKeep = 3; 
	size_t siz = static_cast<size_t>(loopEdges.size() * 0.1f);

	if (siz < minKeep) {
		siz = std::min(loopEdges.size(), minKeep);
	}
	/////////////////////////

	int count = 0;
	if (!loopEdges.empty())
	{
		float maxDist = loopEdges.back().dist;
		for (const auto& e : loopEdges)
		{
			float percent = 0.0f;

			if (maxDist > 0.0f)
			{
				// 距離を0〜1に正規化
				float ratio = e.dist / maxDist;

				// 距離が短いほど大きくなる
				float inverted = 1.0f - ratio;

				// 0〜100の確率に変換
				percent = inverted * 100.0f;

				if (KdRandom::GetInt(1, 100) < percent && count < 5)
				{
					count++;
					pairList.push_back({ _roomInfo[e.roomA], _roomInfo[e.roomB] });
				}
			}
		}
	}

	return pairList;
}

std::vector<Math::Vector3> MapGenerate::GenerateCorridorPath(const RoomInfo& _A, const RoomInfo& _B)
{
	int hA = _A.m_heightLevel;
	int hB = _B.m_heightLevel;
	int hDiff = hB - hA;
	int absHDiff = std::abs(hDiff);
	int stepDir = (hDiff > 0) ? 1 : -1;
	// return用
	std::vector<Math::Vector3> ans;
	// A の端候補 
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


	// L字通路（横 → 縦）
	int xStart = (int)bestA.x;
	int xEnd = (int)bestB.x;
	int yMid = (int)bestA.y;
	int yStart = (int)bestA.y;
	int yEnd = (int)bestB.y;
	int xMid = (int)bestB.x;
	int dx = std::abs(xEnd - xStart);
	int dy = std::abs(yEnd - yStart);
	int total = dx + dy;
	int progress = 0;
	int currentZ = hA; // 最初は始点部屋の高さ hA
	// --- 横方向 ---
	int xStep = (xStart <= xEnd) ? 1 : -1;
	for (int x = xStart; x != xEnd + xStep; x += xStep)
	{
		// 全体の進捗に応じた目標高さ
		int targetZ = (total == 0) ? hA : hA + stepDir * ((progress * absHDiff) / total);
		// 【重要】高低差が常に0か1になるように制限 (1マスにつき最大±1しか変化させない)
		if (targetZ > currentZ + 1)
		{
			currentZ += 1;
		}
		else if (targetZ < currentZ - 1)
		{
			currentZ -= 1;
		}
		else
		{
			currentZ = targetZ;
		}
		ans.push_back({ (float)x, (float)yMid, (float)currentZ });
		progress++;
	}
	// --- 縦方向 ---
	// (角のマス xMid, yStart は横方向のループの最後で追加済みなので、yStart + yStep から開始して重複を防ぐ)
	int yStep = (yStart <= yEnd) ? 1 : -1;
	if (yStart != yEnd)
	{
		for (int y = yStart + yStep; y != yEnd + yStep; y += yStep)
		{
			int targetZ = (total == 0) ? hA : hA + stepDir * ((progress * absHDiff) / total);
			// 【重要】高低差が常に0か1になるように制限 (1マスにつき最大±1しか変化させない)
			if (targetZ > currentZ + 1)
			{
				currentZ += 1;
			}
			else if (targetZ < currentZ - 1)
			{
				currentZ -= 1;
			}
			else
			{
				currentZ = targetZ;
			}
			ans.push_back({ (float)xMid, (float)y, (float)currentZ });
			progress++;
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

bool MapGenerate::IsNeedWall(int nx, int ny, const std::vector<std::vector<FloorInfo>>& map, int _heightLevel, int x, int y)
{
	// マップ範囲外なら壁を作る
	if (ny < 0 || ny >= static_cast<int>(map.size())) { return true; }
	if (nx < 0 || nx >= static_cast<int>(map[ny].size())) { return true; }

	// 隣のマスが None（空き地）なら壁を作る
	if (map[ny][nx].m_tileType == TileType::None) { return true; }



	//隣がSlopeじゃないかつ自分がSlopeじゃないなら壁を作る
	if (map[ny][nx].m_tileType != TileType::Slopee&& map[y][x].m_tileType != TileType::Slopee)
	{
		if (_heightLevel != map[ny][nx].m_heightLevel)
		{
			return true;
		}
	}

	return false;
}

void MapGenerate::CreateWallOrStairs(const Math::Vector3& _pos, float _rotYDegree, bool _isStairs, std::list<std::shared_ptr<MapBase>>* _ret, int _roomID, int _x, int _y, const std::vector<std::vector<FloorInfo>>& map, bool* _flg)
{
	if (_isStairs)
	{
		auto stairs = std::make_shared<StairsBase>();
		stairs->SetModel(m_spStairsModel);
		stairs->Init();
		stairs->SetPos(_pos);
		if (_rotYDegree != 0.0f)
		{
			stairs->SetRotation(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(_rotYDegree)));
		}

		stairs->SetMapObjType(MapObjType::Stairs);
		stairs->SerRoomID(_roomID);
		_ret->push_back(stairs);
	}
	else
	{
		auto wall = std::make_shared<WallBase>();
		wall->SetModel(m_spWallModel);
		wall->Init();
		wall->SetPos(_pos);
		if (_rotYDegree != 0.0f)
		{
			wall->SetRotation(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(_rotYDegree)));
		}

		wall->SetMapObjType(MapObjType::Wall);
		_ret->push_back(wall);



		if (_flg == nullptr)
		{
			return;
		}


		if (*_flg)
		{
			return;
		}


		//松明を置くかどうか
		bool placeTorch = *_flg;


		if (map[_y][_x].m_tileType == TileType::Room)
		{
			// 部屋の壁ならランダムに置く
			if (KdRandom::GetInt(0, 100) < 15) { // 15% くらい
				placeTorch = true;
			}
		}
		else
		{
			// コーナーなら置く
			if (IsCornerWall(_x, _y, map))
			{
				placeTorch = true;
			}
			else
			{
				// ランダム
				if (KdRandom::GetInt(0, 100) < 15) { // 15% くらい
					placeTorch = true;
				}
			}


		}

		if (placeTorch)
		{
			SetTorch(_rotYDegree, _pos, wall);
		}

		*_flg = placeTorch;
	}
}

void MapGenerate::SetTorch(float _rotYDegree, Math::Vector3 _pos, std::shared_ptr<KdGameObject> _obj)
{
	std::shared_ptr<MapObjManager>spMapObjManager = m_wpMapObjManager.lock();
	if (!spMapObjManager) { return; }

	//壁に松明を
	Math::Vector3 pos = _pos;
	pos.y += 5;
	float rad = DirectX::XMConvertToRadians(_rotYDegree);

	float moveX = sin(rad);
	float moveZ = cos(rad);

	pos.x -= moveX * 2;
	pos.z -= moveZ * 2;

	KdCollider::RayInfo rayInfo;
	rayInfo.m_pos = pos;
	Math::Vector3 dir;
	dir.x = moveX;
	dir.y = 0.0f;
	dir.z = moveZ;
	dir.Normalize();
	rayInfo.m_dir = dir;

	rayInfo.m_range = 5;
	rayInfo.m_type = KdCollider::TypeBump;

	float maxOverLap = 0;
	Math::Vector3 hitPos = {};
	bool hit = false;

	std::list<KdCollider::CollisionResult> retRayList;
	if (_obj->Intersects(rayInfo, &retRayList))
	{
		for (auto& ret : retRayList)
		{
			if (maxOverLap < ret.m_overlapDistance)
			{
				maxOverLap = ret.m_overlapDistance;
				pos = ret.m_hitPos;
				hit = true;
			}
		}
	}



	//当たってたら松明設置
	if (hit)
	{
		std::shared_ptr<Torch>spTorch = std::make_shared<Torch>();
		spTorch->Init();
		spTorch->SetPos(pos);
		spTorch->SetRotation(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(_rotYDegree)));

		spMapObjManager->AddMapObj(spTorch);
	}


}


bool MapGenerate::IsCornerWall(int x, int y, const std::vector<std::vector<FloorInfo>>& map)
{
	int height = static_cast<int>(map.size());
	int width = static_cast<int>(map[0].size());

	auto isValid = [&](int nx, int ny) {
		return ny >= 0 && ny < height && nx >= 0 && nx < width;
		};

	bool up = isValid(x, y - 1) && map[y - 1][x].m_tileType != TileType::None;
	bool down = isValid(x, y + 1) && map[y + 1][x].m_tileType != TileType::None;
	bool left = isValid(x - 1, y) && map[y][x - 1].m_tileType != TileType::None;
	bool right = isValid(x + 1, y) && map[y][x + 1].m_tileType != TileType::None;

	return (up || down) && (left || right);
}

void MapGenerate::SlopeCheck(std::vector<std::vector<FloorInfo>>* map)
{
	int height = static_cast<int>(map->size());

	for (int y = 0; y < height; y++)
	{
		int width = static_cast<int>((*map)[y].size());

		auto isValid = [&](int nx, int ny) {
			return ny >= 0 && ny < height && nx >= 0 && nx < width;
			};

		for (int x = 0; x < width; x++)
		{
			if ((*map)[y][x].m_tileType != TileType::Floor)
			{
				continue;
			}




			bool up = isValid(x, y + 1) &&( (*map)[y + 1][x].m_tileType == TileType::Floor|| (*map)[y + 1][x].m_tileType == TileType::Room);
			bool down = isValid(x, y - 1) && ((*map)[y - 1][x].m_tileType == TileType::Floor|| (*map)[y - 1][x].m_tileType == TileType::Room);
			bool left = isValid(x - 1, y) && ((*map)[y][x - 1].m_tileType == TileType::Floor|| (*map)[y][x - 1].m_tileType == TileType::Room);
			bool right = isValid(x + 1, y) && ((*map)[y][x + 1].m_tileType == TileType::Floor|| (*map)[y][x + 1].m_tileType == TileType::Room);

			// 上にまっすぐ
			if (up && down && (!left && !right))
			{
				int heightLevel = (*map)[y][x].m_heightLevel;

				// 安全チェックを追加
				int UpheightLevel = 0;
				if (isValid(x, y + 1))
				{
					UpheightLevel = (*map)[y + 1][x].m_heightLevel;
				}
				else
				{
					continue;
				}
				int DownheightLevel = 0;
				if (isValid(x, y - 1))
				{
					DownheightLevel = (*map)[y - 1][x].m_heightLevel;
				}
				else
				{
					continue;
				}



				if (heightLevel != UpheightLevel)
				{
					(*map)[y][x].m_tileType = TileType::Slopee;

					if (heightLevel < UpheightLevel)
					{
						(*map)[y][x].m_heightLevel = UpheightLevel;
						(*map)[y][x].m_angle = 0;
					}
					else
					{
						(*map)[y][x].m_angle = 180;
					}				
				}
				else if (heightLevel != DownheightLevel)
				{
					(*map)[y][x].m_tileType = TileType::Slopee;

					if (heightLevel < DownheightLevel)
					{
						(*map)[y][x].m_heightLevel = DownheightLevel;
						(*map)[y][x].m_angle = 180;
					}
					else
					{
						(*map)[y][x].m_angle = 0;
					}

					
				}
			}
			else  if (left && right && (!up && !down))// 左右にまっすぐ
			{
				int heightLevel = (*map)[y][x].m_heightLevel;

				// 安全に高さ取得

				// 安全チェックを追加
				int LeftHeightLevel = 0;
				if (isValid(x - 1, y))
				{
					LeftHeightLevel = (*map)[y][x - 1].m_heightLevel;
				}
				else
				{
					continue;
				}
				int RightHeightLevel = 0;
				if (isValid(x + 1, y))
				{
					RightHeightLevel = (*map)[y][x + 1].m_heightLevel;
				}
				else
				{
					continue;
				}

				if (heightLevel != LeftHeightLevel)
				{
					(*map)[y][x].m_tileType = TileType::Slopee;

					if (heightLevel < LeftHeightLevel)
					{
						(*map)[y][x].m_heightLevel = LeftHeightLevel;
						(*map)[y][x].m_angle = 90;
					}
					else
					{
						(*map)[y][x].m_angle = 270;
					}

					

				}
				else if (heightLevel != RightHeightLevel)
				{
					(*map)[y][x].m_tileType = TileType::Slopee;

					if (heightLevel < RightHeightLevel)
					{
						(*map)[y][x].m_heightLevel = RightHeightLevel;
						(*map)[y][x].m_angle = 270;
					}
					else
					{
						(*map)[y][x].m_angle = 90;
					}
				}
			}
		}
	}
}



