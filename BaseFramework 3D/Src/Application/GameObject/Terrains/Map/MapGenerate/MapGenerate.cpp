#include "MapGenerate.h"
#include<array>
#include<queue>
#include<set>
#include"../FloorBase/FloorBase.h"
#include"../WallBase/WallBase.h"
#include"../Stairs/StairsBase.h"
#include"../Slope/Slope.h"
//
#include"../../MapObj/MapObjManager.h"
#include"../../MapObj/Torch/Torch.h"

//コンストラクタ
//部屋サイズの設定ファイルと、床・坂・壁・階段のモデルを読み込む
MapGenerate::MapGenerate()
{
	m_roomSizPath = "Asset/Data/ObjeData/Terrains/Map/MapSiz.json";

	LoadRoomSiz(m_roomSizPath);


	if (!m_spFloorModel)
	{
		m_spFloorModel = std::make_shared<KdModelWork>();
		m_spFloorModel->SetModelData("Asset/Models/Terrains/Map/Castle/Floor/Base.gltf");
	}

	if (!m_spSlopeModel)
	{
		m_spSlopeModel = std::make_shared<KdModelWork>();
		m_spSlopeModel->SetModelData("Asset/Models/Terrains/Map/Castle/Slope/Slope.gltf");
	}

	if (!m_spWallModel)
	{
		m_spWallModel = std::make_shared<KdModelWork>();
		m_spWallModel->SetModelData("Asset/Models/Terrains/Map/Castle/Wall/Base.gltf");
	}

	if (!m_spStairsModel)
	{
		m_spStairsModel = std::make_shared<KdModelWork>();
		m_spStairsModel->SetModelData("Asset/Models/Terrains/Map/Castle/Stairs/Stairs.gltf");
	}
}

//===================================================
// マップ生成
//===================================================

//////////////////////////////////////////////////////
// 通常の階のマップ生成
// 1. マップを区画に分け、各区画に部屋か交差点を1つ置く
// 2. 隣の区画どうしを最小全域木でつなぎ、小さな輪の辺を足す
// 3. くねくね通路で掘る(輪の辺の一部はT字路にする)
// 4. 切れている所をつなぎ、太い通路を細くする
// 5. 高さを調整して坂を置き、異常があれば作り直す
// 6. スポーン部屋・階段部屋を決めて、床・壁・天井を作る
//////////////////////////////////////////////////////
std::vector<std::vector<bool>> MapGenerate::Generate(Math::Vector2 _mapSiz, int _roomNum, float _tileSiz, int _type, std::list<std::shared_ptr<MapBase>>* _ret, Math::Vector3* _playerSpawnPos, Math::Vector3* _basePos)
{
	m_chunks.clear();

	if (_roomNum <= kMinRoomNum)
	{
		_roomNum = kMinRoomNum;
	}


	if (m_roomMin <= 0 && m_roomMax <= 0)
	{
		m_roomMax = 1;
		m_roomMin = 0;
	}

	m_roomNum = 0;

	m_roomInfo.clear();
	m_roomInfoList.clear();
	m_dummyNodeList.clear();
	m_areaGrid.clear();

	//マップのサイズを作る
	//TileType::Noneで初期化(何もない)
	std::vector<std::vector<FloorInfo>> map(static_cast<size_t>(_mapSiz.y), std::vector<FloorInfo>(static_cast<size_t>(_mapSiz.x)));
	std::vector<std::vector<int>> roomIDVector(static_cast<size_t>(_mapSiz.y), std::vector<int>(static_cast<size_t>(_mapSiz.x), -1));

	//戻り値用のデータ
	std::vector<std::vector<bool>> returnMapDate(static_cast<size_t>(_mapSiz.y), std::vector<bool>(static_cast<size_t>(_mapSiz.x), false));


	//チャンク対応
	int chunkW = std::max(1, static_cast<int>(std::ceil(_mapSiz.x / static_cast<float>(kChunkSize))));
	int chunkH = std::max(1, static_cast<int>(std::ceil(_mapSiz.y / static_cast<float>(kChunkSize))));

	m_chunks.resize(chunkH);
	for (auto& row : m_chunks)
	{
		row.resize(chunkW);
	}


	/////////////////////////////////////////////////////
	// 部屋と交差点を置く(区画分割)

	//次に使う部屋ID(部屋を置くたびに1進む。交差点では進めない)
	int roomID = kInitialRoomID;

	//部屋(と交差点)を置く。どちらの置き方にするかは抽選で決める
	bool useArea = (KdRandom::GetInt(1, 100) <= kAreaLayoutRate);
	if (useArea)
	{
		//区画分割による生成
		PlaceRoomsByArea(_mapSiz, _roomNum, map, roomIDVector, roomID);
	}
	else
	{
		//単純なランダム生成
		PlaceRoomsRandom(_mapSiz, _roomNum, map, roomIDVector, roomID);
	}

	//部屋IDごとの部屋の種類(添え字は部屋ID。交差点は含まない)
	std::vector<RoomType> roomTypeList(roomID);

	/////////////////////////////////////////////////////
	//安全な部屋にする部屋を1つだけ先に決める(部屋IDは kInitialRoomID から始まる)
	int safeRoomID = KdRandom::GetInt(kInitialRoomID, roomID - 1);

	//置いた部屋の数(roomID は次に使うIDなので、初期値を引く)
	int placedRoomNum = roomID - kInitialRoomID;

	//部屋の数が最低数しかないなら、安全な部屋は作らない
	bool useSafeRoom = (kMinRoomNum < placedRoomNum);

	for (int i = 0; i < roomID; i++)
	{
		//部屋の数が最低数なら敵部屋で埋める
		if (!useSafeRoom)
		{
			roomTypeList[i] = RoomType_EnemyRoom;
		}
		//安全な部屋に選んだ部屋
		else if (i == safeRoomID)
		{
			roomTypeList[i] = RoomType_SafeRoom;
		}
		//それ以外は敵の部屋・宝箱部屋から引く
		else
		{
			roomTypeList[i] = (RoomType)KdRandom::GetInt((int)RoomType_EnemyRoom, (int)RoomType_TreasureChestRoom);
		}
	}

	/////////////////////////////////////////////////////
	// どの部屋とどの部屋を通路でつなぐか決める

	//部屋と交差点をまとめたリスト(この並びの番号を ConnectPair や corridorOwner で使う)
	std::vector<RoomInfo> connectNodeList;
	connectNodeList.reserve(m_roomInfo.size() + m_dummyNodeList.size());

	// 先に本物の部屋、その後ろに交差点(この順番を崩さない)
	connectNodeList.insert(connectNodeList.end(), m_roomInfo.begin(), m_roomInfo.end());
	connectNodeList.insert(connectNodeList.end(), m_dummyNodeList.begin(), m_dummyNodeList.end());

	//つなぐ組(最小全域木の辺が先、小さな輪の辺が後)
	std::vector<ConnectPair> pairs = GetRoomConnectionPairs(connectNodeList);

	/////////////////////////////////////////////////////
	// 通路を掘る

	//通路のマスごとに、その通路がつないでいる2つのノード番号(connectNodeList の番号)を記録する
	//まだ通路でないマスは {-1, -1}。T字路の合流先を探すときに使う
	std::vector<std::vector<std::pair<int, int>>> corridorOwner(
		map.size(), std::vector<std::pair<int, int>>(map[0].size(), { -1, -1 }));

	//通路のマスをマップに書き込む(空いているマスだけ。既存の床は上書きしない)
	// _path ... 通路のマスの並び(x, y がマス、z が高さ)
	// _nodeA / _nodeB ... この通路がつなぐ2つのノード番号
	auto applyPathToMap = [&](const std::vector<Math::Vector3>& _path, int _nodeA, int _nodeB) {
		for (const auto& a : _path)
		{
			int x = static_cast<int>(a.x);
			int y = static_cast<int>(a.y);

			if (y >= 0 && y < static_cast<int>(map.size()) &&
				x >= 0 && x < static_cast<int>(map[y].size()))
			{
				if (map[y][x].m_tileType == TileType::None)
				{
					map[y][x].m_tileType = TileType::Floor;
					map[y][x].m_heightLevel = static_cast<int>(a.z);

					// このマスはどの2部屋の通路か
					corridorOwner[y][x] = { _nodeA, _nodeB };
				}
			}
		}
		};

	for (auto& p : pairs)
	{
		//////////////////////////////////////////
		// T字路
		// 輪の辺の一部を、部屋Bそのものではなく「部屋Bから出ている別の通路の途中」につなぐ
		if (p.m_isLoop && !p.m_a.m_isDummy && KdRandom::GetInt(1, 100) <= kTJunctionRate)
		{
			// 合流先の候補を集める
			std::vector<std::pair<int, int>> tCandidateList;
			for (int y = 0; y < static_cast<int>(map.size()); y++)
			{
				for (int x = 0; x < static_cast<int>(map[y].size()); x++)
				{
					const auto& owner = corridorOwner[y][x];

					// 部屋Bにつながっていて、部屋Aとはつながっていない通路だけ
					bool hasB = (owner.first == p.m_nodeB || owner.second == p.m_nodeB);
					bool hasA = (owner.first == p.m_nodeA || owner.second == p.m_nodeA);
					if (!hasB || hasA) { continue; }

					// まっすぐな通路の途中だけ(曲がり角・分かれ道は除く)
					if (!IsStraightCorridor(x, y, map)) { continue; }

					// どの部屋からも kTMinFromRoom マス以上離れている所だけ
					if (IsNearRoom(x, y, kTMinFromRoom - 1)) { continue; }

					// 部屋Aから kTMinDistFromA マス以上離れている所だけ
					int distA = std::abs(x - static_cast<int>(p.m_a.m_center.x)) + std::abs(y - static_cast<int>(p.m_a.m_center.y));
					if (distA < kTMinDistFromA) { continue; }

					tCandidateList.push_back({ x, y });
				}
			}

			if (!tCandidateList.empty())
			{
				// 候補からランダムに1マス選ぶ
				auto [tx, ty] = tCandidateList[KdRandom::GetInt(0, static_cast<int>(tCandidateList.size()) - 1)];

				// 合流先を1×1の一時的な部屋(交差点と同じ形)にする
				RoomInfo target;
				target.m_roomID = -1;
				target.m_center = { (float)tx, (float)ty };
				target.m_roomEnd.m_farLeft = tx;
				target.m_roomEnd.m_farRight = tx;
				target.m_roomEnd.m_topEnd = ty;
				target.m_roomEnd.m_downEnd = ty;
				target.m_heightLevel = map[ty][tx].m_heightLevel;
				target.m_areaX = p.m_b.m_areaX;
				target.m_areaY = p.m_b.m_areaY;
				target.m_isDummy = true;

				// 部屋Aから合流先へ掘る(既存の通路に当たったら、そこでT字に合流して止まる)
				std::vector<Math::Vector3> ansT = GenerateCorridorPath(p.m_a, target, map);
				applyPathToMap(ansT, p.m_nodeA, p.m_nodeB);

				// 到達判定と、逆向きの掘り直しはしない
				continue;
			}
			// 候補がなければ、下の普通の処理で部屋Bへつなぐ
		}

		//////////////////////////////////////////
		// 普通の通路(部屋A → 部屋B)

		// 部屋Aから部屋B方向へ通路生成
		std::vector<Math::Vector3> ansA = GenerateCorridorPath(p.m_a, p.m_b, map);
		applyPathToMap(ansA, p.m_nodeA, p.m_nodeB);

		// 部屋Bに到達したか判定(部屋Bの中か、上下左右の隣で止まっていれば届いた扱い)
		bool reachedRoomB = false;
		if (!ansA.empty())
		{
			Math::Vector2 lastPos = { ansA.back().x, ansA.back().y };
			reachedRoomB = IsReachedRoom(lastPos, p.m_b);
		}

		// 途中で既存通路に当たって部屋Bに到達しなかった場合、部屋Bからも部屋A方向（既存通路）へ向けて接続
		if (!reachedRoomB)
		{
			std::vector<Math::Vector3> ansB = GenerateCorridorPath(p.m_b, p.m_a, map);
			applyPathToMap(ansB, p.m_nodeA, p.m_nodeB);
		}
	}


	// どこかでつながりが切れていたら、途中で止まらない通路で強制的につなぐ
	ConnectAllRegions(map);

	// 2x2等の太い通路を検出して1マス幅にスリム化
	CleanupWideCorridors(map);



	// 全グリッドの高さ調整（スロープ化可能な3マス以上の直線部分のみ高さ変化を許可）
	bool changed = true;
	int maxPasses = 100;
	int mapH = static_cast<int>(map.size());
	int mapW = mapH > 0 ? static_cast<int>(map[0].size()) : 0;

	auto isValidGrid = [&](int _nx, int _ny) {
		return _ny >= 0 && _ny < mapH && _nx >= 0 && _nx < mapW;
		};

	auto canSlopeVertical = [&](int _x, int _y) {
		if (!isValidGrid(_x, _y)) return false;
		if (map[_y][_x].m_tileType != TileType::Floor) return false;

		bool up = isValidGrid(_x, _y - 1) && (map[_y - 1][_x].m_tileType != TileType::None);
		bool down = isValidGrid(_x, _y + 1) && (map[_y + 1][_x].m_tileType != TileType::None);

		bool leftNone = !isValidGrid(_x - 1, _y) || (map[_y][_x - 1].m_tileType == TileType::None);
		bool rightNone = !isValidGrid(_x + 1, _y) || (map[_y][_x + 1].m_tileType == TileType::None);

		return up && down && leftNone && rightNone;
		};

	auto canSlopeHorizontal = [&](int _x, int _y) {
		if (!isValidGrid(_x, _y)) return false;
		if (map[_y][_x].m_tileType != TileType::Floor) return false;

		bool left = isValidGrid(_x - 1, _y) && (map[_y][_x - 1].m_tileType != TileType::None);
		bool right = isValidGrid(_x + 1, _y) && (map[_y][_x + 1].m_tileType != TileType::None);

		bool upNone = !isValidGrid(_x, _y - 1) || (map[_y - 1][_x].m_tileType == TileType::None);
		bool downNone = !isValidGrid(_x, _y + 1) || (map[_y + 1][_x].m_tileType == TileType::None);

		return left && right && upNone && downNone;
		};

	static const int dx[] = { 0, 0, -1, 1 };
	static const int dy[] = { -1, 1, 0, 0 };

	while (changed && maxPasses-- > 0)
	{
		changed = false;
		for (int y = 0; y < mapH; y++)
		{
			for (int x = 0; x < mapW; x++)
			{
				if (map[y][x].m_tileType == TileType::None) continue;

				for (int d = 0; d < 4; d++)
				{
					int nx = x + dx[d];
					int ny = y + dy[d];

					if (!isValidGrid(nx, ny)) continue;
					if (map[ny][nx].m_tileType == TileType::None) continue;

					int diff = map[y][x].m_heightLevel - map[ny][nx].m_heightLevel;
					if (diff == 0) continue;

					bool isVertDir = (dx[d] == 0);
					bool canSlopeInDir = isVertDir ? (canSlopeVertical(x, y) && canSlopeVertical(nx, ny))
						: (canSlopeHorizontal(x, y) && canSlopeHorizontal(nx, ny));

					// 方向沿いにスロープが置けない場合、高さ差は0（完全フラット）に強制
					if (!canSlopeInDir)
					{
						if (map[y][x].m_tileType == TileType::Floor)
						{
							map[y][x].m_heightLevel = map[ny][nx].m_heightLevel;
							changed = true;
						}
						else if (map[ny][nx].m_tileType == TileType::Floor)
						{
							map[ny][nx].m_heightLevel = map[y][x].m_heightLevel;
							changed = true;
						}
					}
					// スロープ可能な方向なら高さ差を最大1に制限
					else
					{
						if (diff > 1)
						{
							map[y][x].m_heightLevel = map[ny][nx].m_heightLevel + 1;
							changed = true;
						}
						else if (diff < -1)
						{
							map[y][x].m_heightLevel = map[ny][nx].m_heightLevel - 1;
							changed = true;
						}
					}
				}
			}
		}
	}

	//Slopeを設置
	SlopeCheck(&map);

	//作り切れたかチェック
	if (!MapCheck(map))
	{
		// 異常がある場合、再度生成
		return Generate(_mapSiz, _roomNum, _tileSiz, _type, _ret, _playerSpawnPos, _basePos);
	}
	//異常がないから続く


	/////////////////////////////////////////////////////
	//プレイヤーのスポーン部屋を決める
	int playerSpawnRoomID = KdRandom::GetInt(kInitialRoomID, roomID - 1);

	for (auto& room : m_roomInfo)
	{
		if (room.m_roomID == playerSpawnRoomID)
		{
			room.m_playerSpawnRoom = true;
			roomTypeList[room.m_roomID] = RoomType_PlayerSpawn;
			break;
		}
	}


	/////////////////////////////////////////////////////
	//階段設置部屋を決める
	int setStairsRoomID = playerSpawnRoomID;
	if (roomID - 1 > kInitialRoomID)
	{
		while (true)
		{
			setStairsRoomID = KdRandom::GetInt(kInitialRoomID, roomID - 1);
			if (playerSpawnRoomID != setStairsRoomID)
			{
				break;
			}
		}
	}

	for (auto& room : m_roomInfo)
	{
		if (room.m_roomID == setStairsRoomID)
		{
			room.m_stairsRoom = true;
			roomTypeList[room.m_roomID] = RoomType_Stairs;
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
				float xPos = _tileSiz * x + _tileSiz * 0.5f;
				float yPos = _tileSiz * map[y][x].m_heightLevel + _tileSiz * 0.5f;
				float zPos = -(_tileSiz * y + _tileSiz * 0.5f);


				if (y == 0 && x == 0)
				{
					*_basePos = { xPos,yPos,zPos };
				}

				std::shared_ptr<FloorBase> spMapA = std::make_shared<FloorBase>();


				Math::Vector3 pos = { xPos,yPos,zPos };
				Math::Vector3 floorPos = pos;
				if (map[y][x].m_tileType == TileType::Slope)
				{
					floorPos.y -= _tileSiz;
				}
				spMapA->SetModel(m_spFloorModel);
				spMapA->Init();
				spMapA->SetPos(floorPos);
				spMapA->SetMapObjType(MapObjType::Ground);

				if (map[y][x].m_tileType == TileType::Floor)
				{
					spMapA->SetGroundType(GroundType::Floor);

					_ret->push_back(spMapA);

					int cx = std::min(chunkW - 1, x / kChunkSize);
					int cy = std::min(chunkH - 1, y / kChunkSize);

					m_chunks[cy][cx].push_back(spMapA);
				}
				else if (map[y][x].m_tileType == TileType::Room)
				{
					spMapA->SetGroundType(GroundType::Room);
					rID = roomIDVector[y][x];
					spMapA->SetRoomID(rID);


					int cx = std::min(chunkW - 1, x / kChunkSize);
					int cy = std::min(chunkH - 1, y / kChunkSize);

					//プレイヤースポーン位置を保存
					if (rID == playerSpawnRoomID)
					{
						spMapA->SetRoomType(RoomType_SafeRoom);
					}
					else
					{
						spMapA->SetRoomType(roomTypeList[rID]);
					}


					// 部屋ごとに部屋IDとワールド座標情報を保存
					if (rID >= 0 && rID < static_cast<int>(m_roomInfoList.size()))
					{
						RoomI room;
						room.m_roomID = rID;
						room.m_pos = pos;
						room.m_roomType = static_cast<int>(roomTypeList[rID]);
						room.m_xy = { (float)x,(float)y };
						room.m_installation = false;
						room.m_chunk = { (float)cx,(float)cy };
						room.m_spMap = spMapA;



						//通路につながってるか確認
						if (y - 1 >= 0 && y + 1 < map.size() &&
							x - 1 >= 0 && x + 1 < map[y].size())
						{
							//通路につながってたら設置物がある判定に
							if (map[y - 1][x].m_tileType == TileType::Floor)
							{
								room.m_installation = true;
							}
							if (map[y + 1][x].m_tileType == TileType::Floor)
							{
								room.m_installation = true;
							}
							if (map[y][x - 1].m_tileType == TileType::Floor)
							{
								room.m_installation = true;
							}
							if (map[y][x + 1].m_tileType == TileType::Floor)
							{
								room.m_installation = true;
							}
						}


						m_roomInfoList[rID].push_back(room);

					}

					_ret->push_back(spMapA);

					//部屋のチャンクは後で入れる
					//m_chunks[cy][cx].push_back(mapA);

				}

				if (map[y][x].m_tileType == TileType::Slope)
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


					_ret->push_back(spSlope);


					int cx = std::min(chunkW - 1, x / kChunkSize);
					int cy = std::min(chunkH - 1, y / kChunkSize);
					m_chunks[cy][cx].push_back(spSlope);


				}







				//壁を生成
				// 4方向の定義データ構造
				struct WallDirectionInfo
				{
					int m_dx, m_dy;              // マップインデックスの移動量
					Math::Vector3 m_offset;    // 座標オフセット
					float m_rotY;              // Y軸回転角度
					bool m_allowStairs;        // 階段の生成を許可するか
				};

				const float halfTile = _tileSiz / 2.0f;
				// 上、下、左、右の定義
				std::vector<WallDirectionInfo> wallDirs = {
				{  0, -1, { 0.0f, 0.0f,  halfTile },   0.0f, true }, // 上 (0度)
				{  0,  1, { 0.0f, 0.0f, -halfTile }, 180.0f, true }, // 下 (180度)
				{ -1,  0, {-halfTile, 0.0f, 0.0f  }, 270.0f, true }, // 左 (270度)
				{  1,  0, { halfTile, 0.0f, 0.0f  }, 90.0f, true }, // 右 (90度)
				};

				bool isStairsRoom = (roomIDVector[y][x] == setStairsRoomID);


				//階段の設定位置をランダムにしたいから配列シャッフル
				for (size_t i = 0; i < wallDirs.size(); ++i)
				{
					int rndIndex = KdRandom::GetInt(0, wallDirs.size() - 1);
					std::swap(wallDirs[i], wallDirs[rndIndex]);
				}

				//松明の設置許可
				bool torchFlg = true;

				for (const auto& dir : wallDirs)
				{
					int nx = x + dir.m_dx;
					int ny = y + dir.m_dy;
					// 隣接マスに壁が必要か判定
					if (IsNeedWall(nx, ny, map, map[y][x].m_heightLevel, x, y))
					{
						int startH = map[y][x].m_heightLevel;
						float startYPos = _tileSiz * startH + _tileSiz * 0.5f;
						Math::Vector3 wallPos = { xPos + dir.m_offset.x, startYPos, zPos + dir.m_offset.z };

						if (map[y][x].m_tileType == TileType::Slope)
						{
							int cx = std::min(chunkW - 1, x / kChunkSize);
							int cy = std::min(chunkH - 1, y / kChunkSize);

							wallPos.y -= _tileSiz;
							std::shared_ptr<MapBase>spWall = CreateWallOrStairs(wallPos, dir.m_rotY, false, _ret, rID, x, y, map, { static_cast<float>(cx) ,static_cast<float>(cy) });
							wallPos.y += _tileSiz;


							m_chunks[cy][cx].push_back(spWall);
						}


						int cx = std::min(chunkW - 1, x / kChunkSize);
						int cy = std::min(chunkH - 1, y / kChunkSize);
						Math::Vector2 chunkNum = { static_cast<float>(cx) ,static_cast<float>(cy) };

						//一番下の段から一番上までの段数
						int layerNum = kHeightLevelMax - startH + 1;

						bool createStairs = dir.m_allowStairs && isStairsRoom && !stairsPlaced;
						if (createStairs)
						{
							//一番下の段は階段にする
							stairsPlaced = true;
							std::shared_ptr<MapBase>spStairs = CreateWallOrStairs(wallPos, dir.m_rotY, true, _ret, rID, x, y, map, chunkNum);
							m_chunks[cy][cx].push_back(spStairs);

							//その上を1本の壁にする(松明は付けない)
							if (layerNum - 1 > 0)
							{
								Math::Vector3 upperPos = wallPos;
								upperPos.y += _tileSiz;
								std::shared_ptr<MapBase>spWall = CreateWallOrStairs(upperPos, dir.m_rotY, false, _ret, rID, x, y, map, chunkNum, nullptr, layerNum - 1);
								m_chunks[cy][cx].push_back(spWall);
							}
						}
						else
						{
							//一番下の段から一番上までを1本の壁にする
							//(松明は一番下の段の高さに付き、付けたら torchFlg が false になってこのマスにはもう付けない)
							std::shared_ptr<MapBase>spWall = CreateWallOrStairs(wallPos, dir.m_rotY, false, _ret, rID, x, y, map, chunkNum, &torchFlg, layerNum);
							m_chunks[cy][cx].push_back(spWall);
						}
					}
				}


				{
					//天井
					std::shared_ptr<FloorBase> spMapA = std::make_shared<FloorBase>();

					Math::Vector3 floorPos = pos;


					for (int i = map[y][x].m_heightLevel; i <= kHeightLevelMax; i++)
					{
						floorPos.y += _tileSiz;
					}

					floorPos.y += 1;

					spMapA->SetModel(m_spFloorModel);
					spMapA->Init();
					spMapA->SetPos(floorPos);
					spMapA->SetMapObjType(MapObjType::None);
					int cx = std::min(chunkW - 1, x / kChunkSize);
					int cy = std::min(chunkH - 1, y / kChunkSize);
					_ret->push_back(spMapA);
					m_chunks[cy][cx].push_back(spMapA);
				}

			}
			/////////////////////////////////////////////////////
		}
	}


	// 1つでもチャンクに同じ部屋の床が入っていたら、部屋の床全てをそのチャンクに入れる
	for (const auto& room : m_roomInfoList)
	{
		// この部屋の床がまたがっているチャンクを重複なしで集める
		std::vector<Math::Vector2> roomChunks;
		for (const auto& floor : room)
		{
			if (std::find(roomChunks.begin(), roomChunks.end(), floor.m_chunk) == roomChunks.end())
			{
				roomChunks.push_back(floor.m_chunk);
			}
		}

		// 集めた各チャンクに、部屋の床を全部登録する
		for (const auto& chunk : roomChunks)
		{
			auto& dst = m_chunks[static_cast<int>(chunk.y)][static_cast<int>(chunk.x)];
			for (const auto& floor : room)
			{
				dst.push_back(floor.m_spMap);
			}
		}
	}




	bool playerSpawn = false;
	//部屋ごとの色々を計算
	for (size_t i = 0; i < m_roomInfoList.size(); i++)
	{
		if (m_roomInfoList[i].empty()) { continue; }

		if (!playerSpawn)
		{
			//プレイヤーのスポーン位置
			if (m_roomInfoList[i][0].m_roomID == playerSpawnRoomID)
			{
				int j = KdRandom::GetInt(0, m_roomInfoList[i].size() - 1);

				m_roomInfoList[i][j].m_installation = true;
				*_playerSpawnPos = m_roomInfoList[i][j].m_pos;
				_playerSpawnPos->y += 1;

				//スポーン位置を決めたので、もう探さない
				playerSpawn = true;
			}
		}



		for (size_t j = 0; j < m_roomInfoList[i].size(); j++)
		{
			//////////////////////////////////////////////////////
			//部屋ごとの敵出現数を計算
			switch (m_roomInfoList[i][j].m_roomType)
			{
			case RoomType_EnemyRoom:
				m_roomInfoList[i][j].m_roomEnemyNum = static_cast<int>(std::round(m_roomInfoList[i].size() * m_roomEnemyPercent.m_enemyRoom));
				if (m_roomInfoList[i][j].m_roomEnemyNum == 0) { m_roomInfoList[i][j].m_roomEnemyNum = 1; }
				break;
			case RoomType_TreasureChestRoom:
				m_roomInfoList[i][j].m_roomEnemyNum = static_cast<int>(std::round(m_roomInfoList[i].size() * m_roomEnemyPercent.m_notEnemyRoom));
				if (m_roomInfoList[i][j].m_roomEnemyNum == 0) { m_roomInfoList[i][j].m_roomEnemyNum = 1; }
				break;
			case RoomType_SafeRoom:
			case RoomType_PlayerSpawn:
				m_roomInfoList[i][j].m_roomEnemyNum = static_cast<int>(std::round(m_roomInfoList[i].size() * m_roomEnemyPercent.m_safeRoom));
				break;
			case RoomType_Stairs:
				m_roomInfoList[i][j].m_roomEnemyNum = 1;
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
				num = static_cast<float>(m_roomInfoList[i].size()) * m_roomTreasureChestPercent.m_treasureChestRoom;
				mine = m_roomTreasureChestNum.m_treasureChestRoomMinNum;
				max = m_roomTreasureChestNum.m_treasureChestRoomMaxNum;
				break;

			default:
				num = static_cast<float>(m_roomInfoList[i].size()) * m_roomTreasureChestPercent.m_notTreasureChestRoom;
				mine = m_roomTreasureChestNum.m_notTreasureChestRoomMinNum;
				max = m_roomTreasureChestNum.m_notTreasureChestRoomMaxNum;
				break;
			}

			num = std::clamp(num,
				std::min(mine, max),
				std::max(mine, max));
			m_roomInfoList[i][j].m_roomTreasureChestNum = static_cast<int>(std::ceil(num));

		}
	}

	return returnMapDate;
}

//ボス部屋のマップ生成(マップ全体が1つの大きな部屋)
std::vector<std::vector<bool>> MapGenerate::GenerateBoss(Math::Vector2 _mapSiz, float _tileSiz, int _type, std::list<std::shared_ptr<MapBase>>* _ret, Math::Vector3* _playerSpawnPos, Math::Vector3* _basePos)
{
	m_chunks.clear();
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


	//チャンク対応
	int chunkW = std::max(1, static_cast<int>(std::ceil(_mapSiz.x / static_cast<float>(kChunkSize))));
	int chunkH = std::max(1, static_cast<int>(std::ceil(_mapSiz.y / static_cast<float>(kChunkSize))));

	m_chunks.resize(chunkH);
	for (auto& row : m_chunks)
	{
		row.resize(chunkW);
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
			float xPos = _tileSiz * x + _tileSiz * 0.5f;
			float yPos = _tileSiz * map[y][x].m_heightLevel + _tileSiz * 0.5f;
			float zPos = -(_tileSiz * y + _tileSiz * 0.5f);


			if (x == centerX && y == map.size() - 1)
			{
				*_playerSpawnPos = { xPos ,0,zPos };
			}

			if (x == centerX && y == centerY)
			{
				m_bossSpawnPos = { xPos ,0,zPos };
			}

			// 脱出用ゲート (中央の左側)
			if (x == centerX - 1 && y == centerY)
			{
				m_escapeWarpGatePos = { xPos, 0, zPos };
			}

			// 次の階層用ゲート (中央の右側)
			if (x == centerX + 1 && y == centerY)
			{
				m_nextWarpGatePos = { xPos, 0, zPos };
			}



			if (y == 0 && x == 0)
			{
				*_basePos = { xPos,0,zPos };
			}




			std::shared_ptr<FloorBase> spMapA = std::make_shared<FloorBase>();


			Math::Vector3 pos = { xPos,0,zPos };

			spMapA->Init();
			spMapA->SetPos(pos);
			spMapA->SetMapObjType(MapObjType::Ground);
			spMapA->SetGroundType(GroundType::Room);
			spMapA->SetRoomID(0);
			_ret->push_back(spMapA);

			int cx = std::min(chunkW - 1, x / kChunkSize);
			int cy = std::min(chunkH - 1, y / kChunkSize);
			m_chunks[cy][cx].push_back(spMapA);




			//壁を生成
			// 4方向の定義データ構造
			struct WallDirectionInfo
			{
				int m_dx, m_dy;              // マップインデックスの移動量
				Math::Vector3 m_offset;    // 座標オフセット
				float m_rotY;              // Y軸回転角度
				bool m_allowStairs;        // 階段の生成を許可するか
			};

			const float halfTile = _tileSiz / 2.0f;
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
				int nx = x + dir.m_dx;
				int ny = y + dir.m_dy;
				// 隣接マスに壁が必要か判定
				if (IsNeedWall(nx, ny, map, map[y][x].m_heightLevel, x, y))
				{


					Math::Vector3 wallPos = { xPos + dir.m_offset.x, 0, zPos + dir.m_offset.z };

					int startH = map[y][x].m_heightLevel;
					bool torchFlg = true;

					//一番下の段から一番上までを1本の壁にする
					int layerNum = kHeightLevelMax - startH + 1;

					int cx = std::min(chunkW - 1, x / kChunkSize);
					int cy = std::min(chunkH - 1, y / kChunkSize);
					std::shared_ptr<MapBase>spWall = CreateWallOrStairs(wallPos, dir.m_rotY, false, _ret, 0, x, y, map, { static_cast<float>(cx) ,static_cast<float>(cy) }, &torchFlg, layerNum);

					m_chunks[cy][cx].push_back(spWall);

				}

			}

			{
				std::shared_ptr<FloorBase> spMapA = std::make_shared<FloorBase>();

				float yPos = _tileSiz * kHeightLevelMax;

				Math::Vector3 pos = { xPos,yPos,zPos };

				spMapA->Init();
				spMapA->SetPos(pos);
				spMapA->SetMapObjType(MapObjType::None);
				spMapA->SetGroundType(GroundType::Room);
				spMapA->SetRoomID(0);
				_ret->push_back(spMapA);
				int cx = std::min(chunkW - 1, x / kChunkSize);
				int cy = std::min(chunkH - 1, y / kChunkSize);
				m_chunks[cy][cx].push_back(spMapA);
			}

		}
	}

	return mapDate;
}

//===================================================
// 設定ファイル
//===================================================

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
	m_roomMin = data["room"]["sizeMin"].get<int>();
	m_roomMax = data["room"]["sizeMax"].get<int>();

}

void MapGenerate::SaveRoomSiz(std::string _filePath)
{
	nlohmann::json data;
	data["room"]["sizeMin"] = m_roomMin;
	data["room"]["sizeMax"] = m_roomMax;

	// 保存（書き込み）
	std::ofstream ofs(_filePath);
	ofs << data.dump(4);
}

//===================================================
// 区画分割(部屋と交差点の配置)
//===================================================

void MapGenerate::PlaceRoomsByArea(const Math::Vector2& _mapSiz, int& _roomNum, std::vector<std::vector<FloorInfo>>& _map, std::vector<std::vector<int>>& _roomIDVector, int& _roomID)
{
	//区画の数を決める(区画が足りないときは _roomNum が減る)
	DecideAreaDiv(_mapSiz, _roomNum);

	//マップを区画に分ける
	DivideAreas(_mapSiz);

	//どの区画を交差点にするか決める
	DecideDummyAreas(_roomNum);

	//区画ごとに、部屋か交差点を1つ置く
	for (auto& row : m_areaGrid)
	{
		for (auto& area : row)
		{
			if (area.m_isDummy)
			{
				PlaceDummyInArea(area, _map);
			}
			else if (!PlaceRoomInArea(area, _map, _roomIDVector, _roomID))
			{
				// 区画が小さくて部屋が入らなかったら交差点にする
				area.m_isDummy = true;
				PlaceDummyInArea(area, _map);
			}
		}
	}
}

void MapGenerate::PlaceRoomsRandom(const Math::Vector2& _mapSiz, int& _roomNum, std::vector<std::vector<FloorInfo>>& _map, std::vector<std::vector<int>>& _roomIDVector, int& _roomID)
{
	int mapSizX = static_cast<int>(_mapSiz.x);
	int mapSizY = static_cast<int>(_mapSiz.y);

	// 1部屋につき最大50回試行、できなかったらスキップ
	const int maxTry = 50;

	while (true)
	{
		// ===== 毎回まっさらな状態から作り直す =====
		for (auto& row : _map)
		{
			std::fill(row.begin(), row.end(), FloorInfo{});
		}
		for (auto& row : _roomIDVector)
		{
			std::fill(row.begin(), row.end(), -1);
		}
		m_roomInfo.clear();
		_roomID = kInitialRoomID;
		m_roomNum = 0;
		// ==========================================

		for (int i = 0; i < _roomNum; i++)
		{
			for (int t = 0; t < maxTry; t++)
			{
				//////////////////////////////////////////
				// 部屋のサイズ決定
				int roomW = KdRandom::GetInt(m_roomMin, m_roomMax);
				int roomH = KdRandom::GetInt(m_roomMin, m_roomMax);

				//////////////////////////////////////////
				// 部屋の左上番号
				int maxX = std::max(0, mapSizX - roomW - 1);
				int maxY = std::max(0, mapSizY - roomH - 1);

				int roomX = KdRandom::GetInt(0, maxX);
				int roomY = KdRandom::GetInt(0, maxY);

				//////////////////////////////////////////
				// 部屋が空いてるかチェック(周りに margin マス空いているか)
				bool canPlace = true;

				//部屋同士何タイル開けるか(通路に坂を置ける長さを残す)
				int minMargin = kHeightLevel + 2;
				int aX = KdRandom::GetInt(minMargin, minMargin + 2);
				int aY = KdRandom::GetInt(minMargin, minMargin + 2);

				for (int y = roomY - aY; y < roomY + roomH + aY && canPlace; y++)
				{
					for (int x = roomX - aX; x < roomX + roomW + aX; x++)
					{
						//マップの外ならスキップ
						if (y < 0 || y >= mapSizY) { continue; }
						if (x < 0 || x >= mapSizX) { continue; }

						if (_map[y][x].m_tileType != TileType::None)
						{
							canPlace = false;
							break;
						}
					}
				}

				if (!canPlace) { continue; }

				//////////////////////////////////////////
				// 空いてるなら作る
				int heightLevel = KdRandom::GetInt(0, kHeightLevel);

				for (int y = roomY; y < roomY + roomH; y++)
				{
					for (int x = roomX; x < roomX + roomW; x++)
					{
						_map[y][x].m_tileType = TileType::Room; // 部屋
						_map[y][x].m_heightLevel = heightLevel;
						_roomIDVector[y][x] = _roomID; // 部屋ID
					}
				}

				//部屋の情報を登録する
				RoomInfo info;
				info.m_roomID = _roomID;
				info.m_center = { (float)(roomX + (roomW - 1) / 2), (float)(roomY + (roomH - 1) / 2) };
				info.m_roomEnd.m_farLeft = roomX;
				info.m_roomEnd.m_farRight = roomX + roomW - 1;
				info.m_roomEnd.m_topEnd = roomY;
				info.m_roomEnd.m_downEnd = roomY + roomH - 1;
				info.m_heightLevel = heightLevel;

				//区画は使わない
				info.m_areaX = 0;
				info.m_areaY = 0;
				info.m_isDummy = false;

				m_roomInfo.push_back(info);

				m_roomNum++;
				_roomID++;
				break; // この部屋は成功したので次の部屋へ
			}
		}

		//最低限の部屋を超えてたら終わり
		if (kMinRoomNum <= m_roomNum)
		{
			break;
		}
	}
}

//区画の数を決めて m_areaDivX / m_areaDivY に入れる
//区画が正方形に近く、余る区画(交差点)が少ない組み合わせを選ぶ
void MapGenerate::DecideAreaDiv(const Math::Vector2& _mapSiz, int& _roomNum)
{
	//部屋の最大サイズを奇数に
	int roomMaxOdd = m_roomMax;
	if (roomMaxOdd % 2 == 0)
	{
		roomMaxOdd++;
	}

	//区画1つに必要な広さ(部屋の最大サイズ＋両側の余白)
	int minAreaSize;
	minAreaSize = roomMaxOdd + kAreaPadding * 2;


	//置ける区画数の上限(縦横)
	int mapDivX = std::max(1, static_cast<int>(_mapSiz.x) / minAreaSize);
	int mapDivY = std::max(1, static_cast<int>(_mapSiz.y) / minAreaSize);

	//一番点数の小さい組み合わせ(点数が小さいほど良い)
	int bestDivX = 0;
	int bestDivY = 0;

	float bestScore = FLT_MAX;

	for (int divX = 1; divX <= mapDivX; divX++)
	{
		for (int divY = 1; divY <= mapDivY; divY++)
		{
			int areaNum = divX * divY;

			//部屋が全部入る組み合わせだけ調べる
			if (areaNum >= _roomNum)
			{
				float areaW = _mapSiz.x / divX;
				float areaH = _mapSiz.y / divY;

				//点数 = 区画の縦横比のずれ(正方形なら0) + 余る区画の数 × 重み
				float score = std::abs(std::log(areaW / areaH)) + kAreaAspectWeight * (areaNum - _roomNum);

				if (score < bestScore)
				{
					bestScore = score;

					bestDivX = divX;
					bestDivY = divY;
				}
			}
		}
	}

	if (bestDivX == 0)
	{
		// どの組み合わせも部屋数に届かない → 置けるだけ区画を取って、部屋数を減らす
		bestDivX = mapDivX;
		bestDivY = mapDivY;
		_roomNum = mapDivX * mapDivY;
	}

	m_areaDivX = bestDivX;
	m_areaDivY = bestDivY;
}

//マップを区画に分けて m_areaGrid を作る
void MapGenerate::DivideAreas(const Math::Vector2& _mapSiz)
{
	//区画1つの基本の大きさ(割り切れない余りは右端の列・下端の行に足す)
	int baseW = std::max(1, static_cast<int>(_mapSiz.x) / m_areaDivX);
	int baseH = std::max(1, static_cast<int>(_mapSiz.y) / m_areaDivY);


	//区画の一覧を [行][列] の大きさにする
	m_areaGrid.resize(m_areaDivY);
	for (auto& row : m_areaGrid)
	{
		row.resize(m_areaDivX);
	}

	for (int y = 0; y < m_areaDivY; y++)
	{
		for (int x = 0; x < m_areaDivX; x++)
		{
			AreaInfo& area = m_areaGrid[y][x];

			//区画の番号
			area.m_areaX = x;
			area.m_areaY = y;


			//区画の左上のマス
			area.m_left = x * baseW;
			area.m_top = y * baseH;

			//右端の列はマップの右端まで(余りを含める)
			if (x == m_areaDivX - 1)
			{
				area.m_right = static_cast<int>(_mapSiz.x) - 1;
			}
			else
			{
				area.m_right = area.m_left + baseW - 1;
			}

			//下端の行はマップの下端まで(余りを含める)
			if (y == m_areaDivY - 1)
			{
				area.m_bottom = static_cast<int>(_mapSiz.y) - 1;
			}
			else
			{
				area.m_bottom = area.m_top + baseH - 1;
			}

			//交差点にするかは DecideDummyAreas で決める
			area.m_isDummy = false;
		}
	}
}

//どの区画を交差点にするか決める(区画数 - _roomNum 個をランダムに選ぶ)
void MapGenerate::DecideDummyAreas(int _roomNum)
{
	//交差点にする区画の数(0以下ならループに入らない)
	int dummyNum = m_areaDivX * m_areaDivY - _roomNum;

	int dummyCount = 0;
	//まだ交差点でない区画をランダムに選んで、交差点にする
	while (dummyCount < dummyNum)
	{
		int x = KdRandom::GetInt(0, m_areaDivX - 1);
		int y = KdRandom::GetInt(0, m_areaDivY - 1);

		AreaInfo& area = m_areaGrid[y][x];
		if (!area.m_isDummy)
		{
			area.m_isDummy = true;
			dummyCount++;
		}
	}
}

//区画に部屋を置いて m_roomInfo に登録する
//区画が小さくて部屋が入らなければfalse(呼び出し側で交差点にする)
bool MapGenerate::PlaceRoomInArea(const AreaInfo& _area, std::vector<std::vector<FloorInfo>>& _map, std::vector<std::vector<int>>& _roomIDVector, int& _roomID)
{
	//////////////////////////////////////////
	// 部屋を置ける広さ(区画から両側の余白を引いたもの)
	int innerW = (_area.m_right - _area.m_left + 1) - kAreaPadding * 2;
	int innerH = (_area.m_bottom - _area.m_top + 1) - kAreaPadding * 2;

	//////////////////////////////////////////
	// 部屋の大きさの上限
	int upperW = std::min(m_roomMax, innerW);
	int upperH = std::min(m_roomMax, innerH);

	// 部屋が入らないなら置かない(呼び出し側で交差点にする)
	if (upperW < 1 || upperH < 1)
	{
		return false;
	}

	// 部屋の大きさの下限(上限を超えない・0にならない)
	int lowerW = std::max(1, std::min(m_roomMin, upperW));
	int lowerH = std::max(1, std::min(m_roomMin, upperH));

	//////////////////////////////////////////
	// 部屋の大きさを決める
	int roomW = KdRandom::GetInt(lowerW, upperW);
	int roomH = KdRandom::GetInt(lowerH, upperH);

	// 奇数にする(+1で上限を超えるなら-1)
	if (roomW % 2 == 0)
	{
		if (roomW + 1 <= upperW) { roomW++; }
		else { roomW--; }
	}
	if (roomH % 2 == 0)
	{
		if (roomH + 1 <= upperH) { roomH++; }
		else { roomH--; }
	}

	//////////////////////////////////////////
	// 部屋の左上の位置を決める(余白の内側に収まる範囲)
	int roomX = KdRandom::GetInt(_area.m_left + kAreaPadding, _area.m_right - kAreaPadding - roomW + 1);
	int roomY = KdRandom::GetInt(_area.m_top + kAreaPadding, _area.m_bottom - kAreaPadding - roomH + 1);

	// 部屋の高さ(何階)
	int heightLevel = KdRandom::GetInt(0, kHeightLevel);

	//////////////////////////////////////////
	// マップに書き込む
	for (int y = roomY; y < roomY + roomH; y++)
	{
		for (int x = roomX; x < roomX + roomW; x++)
		{
			_map[y][x].m_tileType = TileType::Room; // 部屋
			_map[y][x].m_heightLevel = heightLevel;
			_roomIDVector[y][x] = _roomID; // 部屋ID
		}
	}

	//////////////////////////////////////////
	// 部屋の情報を登録する
	RoomInfo info;

	info.m_roomID = _roomID;
	info.m_center = { (float)(roomX + (roomW - 1) / 2), (float)(roomY + (roomH - 1) / 2) };

	//2D上の左右の情報を入れる
	info.m_roomEnd.m_farLeft = roomX;
	info.m_roomEnd.m_farRight = roomX + roomW - 1;

	//2D上の上下の情報を入れる
	info.m_roomEnd.m_topEnd = roomY;
	info.m_roomEnd.m_downEnd = roomY + roomH - 1;

	//3D上の上下(何階)
	info.m_heightLevel = heightLevel;

	//どの区画にあるか
	info.m_areaX = _area.m_areaX;
	info.m_areaY = _area.m_areaY;
	info.m_isDummy = false;

	m_roomInfo.push_back(info);

	//////////////////////////////////////////
	// 次の部屋へ
	_roomID++;
	m_roomNum++;

	return true;
}

//区画に交差点(1マスの通路)を置いて m_dummyNodeList に登録する
void MapGenerate::PlaceDummyInArea(const AreaInfo& _area, std::vector<std::vector<FloorInfo>>& _map)
{
	//置ける範囲(区画の余白の内側)
	int xMin = _area.m_left + kAreaPadding;
	int xMax = _area.m_right - kAreaPadding;

	int yMin = _area.m_top + kAreaPadding;
	int yMax = _area.m_bottom - kAreaPadding;

	// 区画が小さすぎて範囲が逆転したら、区画の真ん中にする
	if (xMax < xMin) { xMin = xMax = (_area.m_left + _area.m_right) / 2; }
	if (yMax < yMin) { yMin = yMax = (_area.m_top + _area.m_bottom) / 2; }


	//位置と高さを決める
	int dummyX = KdRandom::GetInt(xMin, xMax);
	int dummyY = KdRandom::GetInt(yMin, yMax);

	int heightLevel = KdRandom::GetInt(0, kHeightLevel);

	//マップに書き込む(部屋IDがないので Room ではなく Floor にする)
	_map[dummyY][dummyX].m_tileType = TileType::Floor;
	_map[dummyY][dummyX].m_heightLevel = heightLevel;


	//////////////////////////////////////////
	// 交差点の情報を登録する
	RoomInfo info;

	// 交差点には部屋IDを振らない
	info.m_roomID = -1;
	info.m_center = { (float)dummyX, (float)dummyY };

	// 端は全部この1マス(1×1の部屋として扱う)
	info.m_roomEnd.m_farLeft = dummyX;
	info.m_roomEnd.m_farRight = dummyX;
	info.m_roomEnd.m_topEnd = dummyY;
	info.m_roomEnd.m_downEnd = dummyY;

	//3D上の上下(何階)
	info.m_heightLevel = heightLevel;

	//どの区画にあるか
	info.m_areaX = _area.m_areaX;
	info.m_areaY = _area.m_areaY;
	info.m_isDummy = true;

	// 本物の部屋の m_roomInfo には入れない
	m_dummyNodeList.push_back(info);
}

//===================================================
// つなぐ組(最小全域木＋小さな輪)
//===================================================

//////////////////////////////////////////////////////
// つなぐ組を決める
// 隣の区画どうしの辺から、最小全域木(クラスカル法)で全部がつながる辺を選び、
// さらに2×2区画の小さな輪になる辺を足す
//////////////////////////////////////////////////////
std::vector<ConnectPair> MapGenerate::GetRoomConnectionPairs(const std::vector<RoomInfo>& _nodeList)
{
	// つなぐ相手がいなければ何もしない(下の size() - 1 が壊れないように)
	if (_nodeList.size() < 2) { return {}; }

	//2つのノードをつなぐ辺(距離はマンハッタン距離)
	struct Edge
	{
		// 部屋A(i番目)
		int m_roomA;

		// 部屋B(j番目)
		int m_roomB;

		// 部屋Aと部屋Bの距離
		float m_dist;
	};

	//つなぐ候補の辺(隣の区画どうしだけ)
	std::vector<Edge> edges;

	for (size_t i = 0; i < _nodeList.size() - 1; i++)
	{
		for (size_t j = i + 1; j < _nodeList.size(); j++)
		{
			// 隣の区画どうしでなければ、つなぐ候補にしない
			//if (!IsAdjacentArea(_nodeList[i], _nodeList[j])) { continue; }

			float dist = std::abs(_nodeList[i].m_center.x - _nodeList[j].m_center.x) + std::abs(_nodeList[i].m_center.y - _nodeList[j].m_center.y);

			Edge edge;
			edge.m_roomA = static_cast<int>(i);
			edge.m_roomB = static_cast<int>(j);
			edge.m_dist = dist;
			edges.push_back(edge);
		}
	}

	//distが短い順に並べる
	std::sort(edges.begin(), edges.end(), [](const Edge& _a, const Edge& _b) {return _a.m_dist < _b.m_dist; });

	//return用
	std::vector<ConnectPair> pairList;

	//Union-Find用の親の番号(最初は全員が自分自身の親)
	std::vector<int> parent(_nodeList.size());
	for (size_t i = 0; i < _nodeList.size(); i++)
	{
		parent[i] = static_cast<int>(i);
	}

	// 部屋ごとに「木でつながっている相手の番号」を入れる
	std::vector<std::vector<int>> treeAdjList(_nodeList.size());

	//木に入らなかった辺(輪の候補の元)
	std::vector<Edge> loopEdges;

	// 距離の短い順に辺を見ていき、輪ができない辺だけを木に入れる
	for (const auto& e : edges)
	{
		int rootA = FindRoot(parent, e.m_roomA);
		int rootB = FindRoot(parent, e.m_roomB);

		if (rootA != rootB)
		{
			ConnectPair pair;
			pair.m_a = _nodeList[e.m_roomA];
			pair.m_b = _nodeList[e.m_roomB];
			pair.m_nodeA = e.m_roomA;
			pair.m_nodeB = e.m_roomB;
			pair.m_isLoop = false;	// 最小全域木の辺
			pairList.push_back(pair);

			// 木のつながりを両方向に記録する
			treeAdjList[e.m_roomA].push_back(e.m_roomB);
			treeAdjList[e.m_roomB].push_back(e.m_roomA);

			// グループ結合
			UnionSet(parent, rootA, rootB);
		}
		else
		{
			loopEdges.push_back(e);
		}
	}

	// 木の上で kLoopMaxTreeDist 本以内の2部屋だけを、輪の候補にする
	std::vector<Edge> loopCandidateList;
	for (const auto& e : loopEdges)
	{
		int dist = GetTreeDist(treeAdjList, e.m_roomA, e.m_roomB);
		if (dist >= 1 && dist <= kLoopMaxTreeDist)
		{
			loopCandidateList.push_back(e);
		}
	}
	// 候補をシャッフルする(フィッシャー–イェーツ)
	for (int i = static_cast<int>(loopCandidateList.size()) - 1; i >= 1; i--)
	{
		int j = KdRandom::GetInt(0, i);
		std::swap(loopCandidateList[i], loopCandidateList[j]);
	}

	//足す輪の数と、輪の辺を足したノードの印
	int loopAddNum = std::max(1, static_cast<int>(std::round(_nodeList.size() * kLoopRatio)));
	std::vector<bool> loopUsedFlg(_nodeList.size(), false);
	int loopCount = 0;

	// 前から見て、輪の辺を足していく
	for (const auto& e : loopCandidateList)
	{
		if (loopCount >= loopAddNum) { break; }

		// 1つの部屋に輪が集中しないように、すでに輪の辺を足した部屋は飛ばす
		if (loopUsedFlg[e.m_roomA] || loopUsedFlg[e.m_roomB]) { continue; }

		ConnectPair pair;
		pair.m_a = _nodeList[e.m_roomA];
		pair.m_b = _nodeList[e.m_roomB];
		pair.m_nodeA = e.m_roomA;
		pair.m_nodeB = e.m_roomB;
		pair.m_isLoop = true;	// 輪のために足した辺
		pairList.push_back(pair);

		loopUsedFlg[e.m_roomA] = true;
		loopUsedFlg[e.m_roomB] = true;
		loopCount++;
	}

	return pairList;
}

//2つの部屋(交差点)の区画が、上下左右で隣り合っているか
bool MapGenerate::IsAdjacentArea(const RoomInfo& _a, const RoomInfo& _b) const
{
	int diffX = std::abs(_a.m_areaX - _b.m_areaX);
	int diffY = std::abs(_a.m_areaY - _b.m_areaY);

	// 上下左右に1つずれているときだけ隣(斜め・自分自身は含めない)
	return (diffX + diffY) == 1;
}

//木の上で、2つのノードの間に何本の通路があるか(たどり着けなければ-1)
int MapGenerate::GetTreeDist(const std::vector<std::vector<int>>& _treeAdjList, int _from, int _to) const
{
	//BFS(幅優先探索)で、_from から木をたどって _to を探す
	//キューには (ノード番号, そこまでの通路の本数) を入れる
	std::vector<bool> visited(_treeAdjList.size(), false);
	std::queue<std::pair<int, int>> q;

	q.push({ _from, 0 });
	visited[_from] = true;


	while (!q.empty())
	{
		auto [node, dist] = q.front();
		q.pop();

		if (node == _to)
		{
			return dist;
		}

		// 木でつながっている相手を、距離+1 でキューに入れる
		for (int next : _treeAdjList[node])
		{
			if (visited[next]) { continue; }

			visited[next] = true;
			q.push({ next, dist + 1 });
		}
	}


	return -1;
}

// 親(root)を返す
int MapGenerate::FindRoot(std::vector<int>& _parent, int _x)
{
	// 親が自分自身なら root
	if (_parent[_x] == _x)
	{
		return _x;
	}

	// 親の親をたどる（経路圧縮）
	_parent[_x] = FindRoot(_parent, _parent[_x]);
	return _parent[_x];
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

//===================================================
// 通路を掘る
//===================================================

//////////////////////////////////////////////////////
// 部屋Aから部屋Bへの通路のマスの並び(高さつき)を作る
// 形は BuildWigglePath で作り、_stopAtExisting なら既存の床に当たった所で止める
//////////////////////////////////////////////////////
std::vector<Math::Vector3> MapGenerate::GenerateCorridorPath(const RoomInfo& _a, const RoomInfo& _b, const std::vector<std::vector<FloorInfo>>& _map, bool _stopAtExisting)
{
	int hA = _a.m_heightLevel;

	//予備用に、最後に作ったパスと終わりの高さを覚えておく
	std::vector<Math::Vector2> lastPath;
	int lastHB = _b.m_heightLevel;

	for (int retry = 0; retry < kSlopeRetryMax; retry++)
	{
		//① くねくね通路のマスの並びを作る(部屋Aの出口 → 部屋Bの入口)
		std::vector<Math::Vector2> path2D = BuildWigglePath(_a, _b, _map);
		if (path2D.empty()) { return {}; }

		//② 既存の床に当たったら、そこで切る(終わりの高さはつながる床に合わせる)
		int hB = _b.m_heightLevel;
		if (_stopAtExisting)
		{
			CutPathAtExisting(path2D, _a, _map, hB);
		}

		lastPath = path2D;
		lastHB = hB;

		//③ 必要な段差の数と、置く場所(まっすぐな所)を決める
		int needNum = std::abs(hB - hA);
		std::vector<int> slopeIndices;
		if (!PickSlopeIndices(FindSlopeCandidates(path2D), needNum, slopeIndices))
		{
			//まっすぐな所が足りない → 形を作り直す
			continue;
		}

		//④ 段差の場所を越えるたびに、次のマスから1段ずつ高さを変える
		int step = (hB > hA) ? 1 : -1;
		int curH = hA;
		size_t next = 0;

		std::vector<Math::Vector3> ans;
		ans.reserve(path2D.size());
		for (int i = 0; i < static_cast<int>(path2D.size()); i++)
		{
			ans.push_back({ path2D[i].x, path2D[i].y, static_cast<float>(curH) });

			//このマスが段差の場所なら、次のマスから1段変える(このマスが坂になる)
			if (next < slopeIndices.size() && slopeIndices[next] == i)
			{
				curH += step;
				next++;
			}
		}

		return ans;
	}

	//⑤ 何度作ってもまっすぐな所が足りなければ、今までどおり線形補間にする
	return AssignHeightsLinear(lastPath, hA, lastHB);
}

//位置が部屋の中か、部屋の上下左右の隣にあるか(通路が部屋に届いたか)
bool MapGenerate::IsReachedRoom(const Math::Vector2& _pos, const RoomInfo& _room) const
{
	int x = static_cast<int>(_pos.x);
	int y = static_cast<int>(_pos.y);

	// (_x, _y) が部屋の中ならtrue
	auto inRoom = [&](int _x, int _y)
		{
			return _x >= _room.m_roomEnd.m_farLeft && _x <= _room.m_roomEnd.m_farRight &&
				_y >= _room.m_roomEnd.m_topEnd && _y <= _room.m_roomEnd.m_downEnd;
		};

	// 自分が部屋の中なら届いている
	if (inRoom(x, y)) { return true; }

	// 上下左右のどれかが部屋の中なら、隣で止まっているので届いている扱い
	static const int dx[] = { 0, 0, -1, 1 };
	static const int dy[] = { -1, 1, 0, 0 };
	for (int d = 0; d < 4; d++)
	{
		if (inRoom(x + dx[d], y + dy[d])) { return true; }
	}

	return false;
}

//===================================================
// くねくね通路
//===================================================

//くねくね通路のマスの並びを作る
//部屋Aの出口 → まっすぐ kWiggleMinRun マス → 階段状に曲がる(ときどき寄り道) → まっすぐ kWiggleMinRun マス → 部屋Bの入口
std::vector<Math::Vector2> MapGenerate::BuildWigglePath(const RoomInfo& _a, const RoomInfo& _b, const std::vector<std::vector<FloorInfo>>& _map) const
{
	//出る辺と入る辺
	ExitSide sideA, sideB;
	DecideExitSide(_a, _b, sideA, sideB);

	//部屋Aの出口(まっすぐ出る区間がふさがっていたら選び直す)
	Math::Vector2 start;
	for (int i = 0; i < kEdgeRetryMax; i++)
	{
		start = PickEdgePoint(_a, sideA);
		if (IsExitClear(start, sideA, _map))
		{
			break;
		}
	}

	//部屋Bの入口(出口との辺に沿ったずれが1マスだと段差ができるので、0か2以上になるまで選び直す)
	Math::Vector2 end;
	int offset = 1;
	for (int i = 0; i < kEdgeRetryMax; i++)
	{
		end = PickEdgePoint(_b, sideB);
		if (sideA == ExitSide::Left || sideA == ExitSide::Right)
		{
			offset = static_cast<int>(std::abs(end.y - start.y));
		}
		else if (sideA == ExitSide::Top || sideA == ExitSide::Bottom)
		{
			offset = static_cast<int>(std::abs(end.x - start.x));
		}

		if (offset != 1)
		{
			break;
		}
	}

	std::vector<Math::Vector2> path2D;
	path2D.push_back(start);

	// 部屋Aからまっすぐ kWiggleMinRun マス出る
	int outAX = 0;
	int outAY = 0;
	GetExitDir(sideA, outAX, outAY);

	Math::Vector2 runStartPos;
	runStartPos.x = start.x + outAX * kWiggleMinRun;
	runStartPos.y = start.y + outAY * kWiggleMinRun;
	AppendLine(path2D, static_cast<int>(runStartPos.x), static_cast<int>(runStartPos.y));

	// 部屋Bに入る前の、まっすぐな区間の始まり(まだ足さない)
	int outBX = 0;
	int outBY = 0;
	GetExitDir(sideB, outBX, outBY);

	Math::Vector2 runEndPos;
	runEndPos.x = end.x + outBX * kWiggleMinRun;
	runEndPos.y = end.y + outBY * kWiggleMinRun;

	// 最初は部屋から出た向きと直角に進む(上下に出たなら横から)
	bool moveX = (outAY != 0);

	// 無限ループ防止
	int guard = 0;
	int guardMax = 50;

	// runEndPos まで、横と縦を交互にランダムな長さで進む(階段状)
	while (guard < guardMax)
	{
		guard++;
		// 今の位置と、runEndPos までの差
		int curX = static_cast<int>(path2D.back().x);
		int curY = static_cast<int>(path2D.back().y);
		int diffX = static_cast<int>(runEndPos.x) - curX;
		int diffY = static_cast<int>(runEndPos.y) - curY;

		// 着いたら終わり
		if (diffX == 0 && diffY == 0) { break; }

		// (A) 進む番の軸が、もうそろっているとき
		if ((moveX && diffX == 0) || (!moveX && diffY == 0))
		{
			// まだ進まなければいけない方の残り
			int remOther = moveX ? std::abs(diffY) : std::abs(diffX);

			// まっすぐ並んだときの寄り道(少し進む → 横へずれる → 進む → 元の列に戻る)
			if (remOther >= kWiggleMinRun * 3 && KdRandom::GetInt(1, 100) <= kWiggleBumpRate)
			{
				int step = KdRandom::GetInt(kWiggleMinRun, remOther - kWiggleMinRun * 2);

				// ずれる量と向き(左右または上下どちらか)
				int bump = KdRandom::GetInt(1, kWiggleBumpMax);
				if (KdRandom::GetInt(0, 1) == 0) { bump = -bump; }

				if (moveX)
				{
					// x はそろっている → 縦に進む途中で横へずれる
					int dirY = (diffY > 0) ? 1 : -1;
					int y1 = curY + dirY * step;					// 1. 縦に step マス
					int y2 = y1 + dirY * kWiggleMinRun;				// 3. さらに縦に kWiggleMinRun マス
					AppendLine(path2D, curX, y1);					// 1.
					AppendLine(path2D, curX + bump, y1);			// 2. 横へずれる
					AppendLine(path2D, curX + bump, y2);			// 3.
					AppendLine(path2D, curX, y2);					// 4. 元の列に戻る
				}
				else
				{
					// y はそろっている → 横に進む途中で縦へずれる
					int dirX = (diffX > 0) ? 1 : -1;
					int x1 = curX + dirX * step;
					int x2 = x1 + dirX * kWiggleMinRun;
					AppendLine(path2D, x1, curY);
					AppendLine(path2D, x1, curY + bump);
					AppendLine(path2D, x2, curY + bump);
					AppendLine(path2D, x2, curY);
				}
			}

			moveX = !moveX;
			continue;
		}
		// (B) 進む長さ n を決める(どの直線も kWiggleMinRun マス以上にする)
		int rem;
		int other;
		if (moveX)
		{
			rem = std::abs(diffX);
			other = std::abs(diffY);
		}
		else
		{
			rem = std::abs(diffY);
			other = std::abs(diffX);
		}

		int n = 0;
		if (other == 0 || rem <= kWiggleMinRun * 2)
		{
			n = rem;
		}
		else
		{
			n = KdRandom::GetInt(kWiggleMinRun, rem - kWiggleMinRun);
		}
		// (C) n マス進んで、番を交代する

		if (moveX)
		{

			int dir = (diffX > 0) ? 1 : -1;
			AppendLine(path2D, curX + dir * n, curY);

		}
		else
		{

			int dir = (diffY > 0) ? 1 : -1;
			AppendLine(path2D, curX, curY + dir * n);
		}

		moveX = !moveX;
	}
	// 入口の手前まで(たいていループの中で着いている)、続けて入口まで足して返す
	AppendLine(path2D, static_cast<int>(runEndPos.x), static_cast<int>(runEndPos.y));
	AppendLine(path2D, static_cast<int>(end.x), static_cast<int>(end.y));


	return path2D;
}

//相手の部屋がどちらにあるかで、出る辺と入る辺を決める
void MapGenerate::DecideExitSide(const RoomInfo& _a, const RoomInfo& _b, ExitSide& _sideA, ExitSide& _sideB) const
{

	// ① 部屋Bの中心 − 部屋Aの中心 の差(diffX, diffY)を出す
	int diffX = static_cast<int>(_b.m_center.x - _a.m_center.x);
	int diffY = static_cast<int>(_b.m_center.y - _a.m_center.y);

	// ② 横の差の大きさが縦以上なら、左右の辺
	if (std::abs(diffX) >= std::abs(diffY))
	{
		if (diffX > 0)
		{
			// Bが右にある
			_sideA = ExitSide::Right;
			_sideB = ExitSide::Left;
		}
		else
		{
			// Bが左にある
			_sideA = ExitSide::Left;
			_sideB = ExitSide::Right;
		}
	}
	// ③ そうでなければ上下の辺
	else
	{
		if (diffY > 0)
		{
			// Bが下にある(yは下に行くほど大きい)
			_sideA = ExitSide::Bottom;
			_sideB = ExitSide::Top;
		}
		else
		{
			// Bが上にある
			_sideA = ExitSide::Top;
			_sideB = ExitSide::Bottom;
		}
	}
}

//辺の上のランダムな位置を返す(角は除く。交差点なら中心のマス)
Math::Vector2 MapGenerate::PickEdgePoint(const RoomInfo& _room, ExitSide _side) const
{
	// ① 交差点なら中心をそのまま返す
	if (_room.m_isDummy)
	{
		return _room.m_center;
	}

	// ② 部屋の端を短い名前にする(left, right, top, bottom)
	int left = _room.m_roomEnd.m_farLeft;	// 部屋の左端のマス
	int right = _room.m_roomEnd.m_farRight;	// 部屋の右端のマス
	int top = _room.m_roomEnd.m_topEnd;	// 部屋の上端のマス
	int bottom = _room.m_roomEnd.m_downEnd;	// 部屋の下端のマス


	// ③ 角を除いた範囲(左右の辺なら top+1〜bottom-1、上下の辺なら left+1〜right-1)
	//    部屋が小さくて範囲が逆転したら、端(top や left)を使う
	int xMin = left + 1;
	int xMax = right - 1;
	if (xMax < xMin) { xMin = xMax = left; }

	int yMin = top + 1;
	int yMax = bottom - 1;
	if (yMax < yMin) { yMin = yMax = top; }


	// ④ 辺ごとに、固定する方の座標と、ランダムに選ぶ方の座標を決めて返す
	switch (_side)
	{
	case ExitSide::Left:
		return { (float)left, (float)KdRandom::GetInt(yMin, yMax) };
	case ExitSide::Right:
		return { (float)right, (float)KdRandom::GetInt(yMin, yMax) };
	case ExitSide::Top:
		return { (float)KdRandom::GetInt(xMin, xMax), (float)top };
	case ExitSide::Bottom:
		return { (float)KdRandom::GetInt(xMin, xMax), (float)bottom };
	default:
		break;
	}

	// ここには来ないが、念のため中心を返す
	return _room.m_center;
}

//辺から外向きの向き(1マス分)を返す
void MapGenerate::GetExitDir(ExitSide _side, int& _dx, int& _dy) const
{
	// 辺ごとに外向きの向きを入れる
	// Left(-1, 0) / Right(1, 0) / Top(0, -1) / Bottom(0, 1) / それ以外(0, 0)


	switch (_side)
	{
	case ExitSide::Left:
		_dx = -1;
		_dy = 0;
		break;
	case ExitSide::Right:
		_dx = 1;
		_dy = 0;
		break;
	case ExitSide::Top:
		_dx = 0;
		_dy = -1;
		break;
	case ExitSide::Bottom:
		_dx = 0;
		_dy = 1;
		break;
	default:
		_dx = 0;
		_dy = 0;
		break;
	}

}

//出口から kWiggleMinRun マスまっすぐ出る区間と、その両横が空いているか
//ふさがっていると、掘り始めてすぐ自分の部屋の通路に合流して短い出っ張りになる
bool MapGenerate::IsExitClear(const Math::Vector2& _pos, ExitSide _side, const std::vector<std::vector<FloorInfo>>& _map) const
{
	int dx = 0;
	int dy = 0;
	GetExitDir(_side, dx, dy);

	// 外向きを90度回した向き(まっすぐ出る区間の両横)
	int sideX = dy;
	int sideY = dx;

	// (_x, _y) に既存の床があればtrue(マップの外は空いている扱い)
	auto hasFloor = [&](int _x, int _y)
		{
			if (_y < 0 || _y >= static_cast<int>(_map.size())) { return false; }
			if (_x < 0 || _x >= static_cast<int>(_map[_y].size())) { return false; }
			return _map[_y][_x].m_tileType != TileType::None;
		};

	int startX = static_cast<int>(_pos.x);
	int startY = static_cast<int>(_pos.y);

	// 出口からまっすぐ kWiggleMinRun マス出る区間と、その両横を調べる
	for (int k = 1; k <= kWiggleMinRun; k++)
	{
		int x = startX + dx * k;
		int y = startY + dy * k;

		if (hasFloor(x, y)) { return false; }                 // まっすぐ出た先
		if (hasFloor(x + sideX, y + sideY)) { return false; } // 片側
		if (hasFloor(x - sideX, y - sideY)) { return false; } // 反対側
	}

	// どこにも床がなければ空いている
	return true;
}

//パスの最後の位置から目標まで、1マスずつ足す(横をそろえてから縦。斜めには進まない)
void MapGenerate::AppendLine(std::vector<Math::Vector2>& _path2D, int _x, int _y) const
{
	// パスが空なら、どこから伸ばすか分からないので何もしない
	if (_path2D.empty()) { return; }

	// パスの最後のマスを int で取り出す(x, y)
	int x = static_cast<int>(_path2D.back().x);
	int y = static_cast<int>(_path2D.back().y);

	//  (x, y) が目標 (_x, _y) と同じになるまでくり返す
	while (x != _x || y != _y)
	{
		// 先に x をそろえる
		if (x < _x)
		{
			x++;
		}
		else if (x > _x)
		{
			x--;
		}
		// x がそろってから y を動かす(斜めに進まないように)
		else
		{
			if (y < _y)
			{
				y++;
			}
			else
			{
				y--;
			}
		}

		_path2D.push_back({ (float)x, (float)y });
	}
}

//===================================================
// T字路
//===================================================

//両隣が床で、直角方向が空いている通路のマスか(まっすぐな通路の途中か)
bool MapGenerate::IsStraightCorridor(int _x, int _y, const std::vector<std::vector<FloorInfo>>& _map) const
{
	// (_x, _y) に既存の床があればtrue(マップの外は空いている扱い)
	auto hasFloor = [&](int _x, int _y)
		{
			if (_y < 0 || _y >= static_cast<int>(_map.size())) { return false; }
			if (_x < 0 || _x >= static_cast<int>(_map[_y].size())) { return false; }
			return _map[_y][_x].m_tileType != TileType::None;
		};

	// 合流先は通路のマスだけ(部屋や坂は除く)
	if (!hasFloor(_x, _y) || _map[_y][_x].m_tileType != TileType::Floor) { return false; }

	bool up = hasFloor(_x, _y - 1);
	bool down = hasFloor(_x, _y + 1);
	bool left = hasFloor(_x - 1, _y);
	bool right = hasFloor(_x + 1, _y);

	// 横にまっすぐ、または縦にまっすぐな通路の途中ならtrue
	bool horizontal = left && right && !up && !down;
	bool vertical = up && down && !left && !right;

	return horizontal || vertical;
}

//どれかの部屋から _dist マス以内にあるか
bool MapGenerate::IsNearRoom(int _x, int _y, int _dist) const
{
	// 本物の部屋(m_roomInfo)を1つずつ見る
	for (const auto& room : m_roomInfo)
	{
		// 部屋の範囲を上下左右に _dist マスずつ広げた四角の中なら、近い
		bool nearX = (room.m_roomEnd.m_farLeft - _dist <= _x) && (_x <= room.m_roomEnd.m_farRight + _dist);
		bool nearY = (room.m_roomEnd.m_topEnd - _dist <= _y) && (_y <= room.m_roomEnd.m_downEnd + _dist);

		if (nearX && nearY)
		{
			return true;
		}
	}

	// どの部屋の近くでもない
	return false;
}

//===================================================
// 通路の仕上げ
//===================================================

void MapGenerate::ConnectAllRegions(std::vector<std::vector<FloorInfo>>& _map)
{
	const int mapH = static_cast<int>(_map.size());
	if (mapH == 0 || m_roomInfo.size() <= 1) return;
	const int mapW = static_cast<int>(_map[0].size());

	static const int dx[] = { 0, 0, -1, 1 };
	static const int dy[] = { -1, 1, 0, 0 };

	for (int safety = 0; safety < 100; safety++)
	{
		// 歩けるマスを連結成分ごとにラベル付け
		std::vector<std::vector<int>> label(mapH, std::vector<int>(mapW, -1));
		int labelNum = 0;
		for (int y = 0; y < mapH; y++)
		{
			for (int x = 0; x < mapW; x++)
			{
				if (_map[y][x].m_tileType == TileType::None || label[y][x] != -1) continue;

				std::queue<std::pair<int, int>> q;
				q.push({ x, y });
				label[y][x] = labelNum;
				while (!q.empty())
				{
					auto [cx, cy] = q.front(); q.pop();
					for (int d = 0; d < 4; d++)
					{
						int nx = cx + dx[d], ny = cy + dy[d];
						if (nx < 0 || nx >= mapW || ny < 0 || ny >= mapH) continue;
						if (_map[ny][nx].m_tileType == TileType::None || label[ny][nx] != -1) continue;
						label[ny][nx] = labelNum;
						q.push({ nx, ny });
					}
				}
				labelNum++;
			}
		}

		// 各部屋がどの成分に属するか
		std::vector<int> roomLabel(m_roomInfo.size());
		for (size_t i = 0; i < m_roomInfo.size(); i++)
		{
			roomLabel[i] = label[(int)m_roomInfo[i].m_center.y][(int)m_roomInfo[i].m_center.x];
		}
		const int mainLabel = roomLabel[0];

		// 本体側の部屋と、つながっていない部屋の最短ペアを探す
		int bestA = -1, bestB = -1;
		float bestDist = FLT_MAX;
		for (size_t i = 0; i < m_roomInfo.size(); i++)
		{
			if (roomLabel[i] != mainLabel) continue;
			for (size_t j = 0; j < m_roomInfo.size(); j++)
			{
				if (roomLabel[j] == mainLabel) continue;
				float dist = std::abs(m_roomInfo[i].m_center.x - m_roomInfo[j].m_center.x)
					+ std::abs(m_roomInfo[i].m_center.y - m_roomInfo[j].m_center.y);
				if (dist < bestDist) { bestDist = dist; bestA = (int)i; bestB = (int)j; }
			}
		}

		if (bestA < 0) return; // 全部屋が連結済み

		// 途中で止まらない通路で強制的につなぐ
		auto path = GenerateCorridorPath(m_roomInfo[bestA], m_roomInfo[bestB], _map, false);
		for (const auto& p : path)
		{
			int x = (int)p.x, y = (int)p.y;
			if (y < 0 || y >= mapH || x < 0 || x >= mapW) continue;
			if (_map[y][x].m_tileType == TileType::None)
			{
				_map[y][x].m_tileType = TileType::Floor;
				_map[y][x].m_heightLevel = (int)p.z;
			}
		}
	}
}

void MapGenerate::CleanupWideCorridors(std::vector<std::vector<FloorInfo>>& _map)
{
	int mapH = static_cast<int>(_map.size());
	int mapW = mapH > 0 ? static_cast<int>(_map[0].size()) : 0;

	bool changed = true;
	int passes = 0;
	while (changed && passes < 10)
	{
		changed = false;
		passes++;

		for (int y = 0; y < mapH - 1; y++)
		{
			for (int x = 0; x < mapW - 1; x++)
			{
				// 2x2 ブロック（(x,y), (x+1,y), (x,y+1), (x+1,y+1)）で4マスがFloor以上（歩行マス）か判定
				if (_map[y][x].m_tileType != TileType::None &&
					_map[y + 1][x].m_tileType != TileType::None &&
					_map[y][x + 1].m_tileType != TileType::None &&
					_map[y + 1][x + 1].m_tileType != TileType::None)
				{
					// 少なくとも1つがRoomなら部屋の一部として許容（部屋の周囲の角が削られないようにする）
					if (_map[y][x].m_tileType == TileType::Room ||
						_map[y + 1][x].m_tileType == TileType::Room ||
						_map[y][x + 1].m_tileType == TileType::Room ||
						_map[y + 1][x + 1].m_tileType == TileType::Room)
					{
						continue;
					}

					// 2x2の純粋な通路塊が存在する場合、4マスのうち削除可能なFloorマスを1つ消去
					std::pair<int, int> candidates[4] = {
						{x, y}, {x + 1, y}, {x, y + 1}, {x + 1, y + 1}
					};

					for (const auto& cand : candidates)
					{
						if (CanRemoveCorridorTile(cand.first, cand.second, _map))
						{
							_map[cand.second][cand.first].m_tileType = TileType::None;
							changed = true;
							break;
						}
					}
				}
			}
		}
	}
}

bool MapGenerate::CanRemoveCorridorTile(int _x, int _y, std::vector<std::vector<FloorInfo>>& _map)
{
	int mapH = static_cast<int>(_map.size());
	int mapW = mapH > 0 ? static_cast<int>(_map[0].size()) : 0;

	// 通常の通路(Floor)のみ削除検討。RoomやSlopee、Noneは対象外
	if (_map[_y][_x].m_tileType != TileType::Floor) return false;

	static const int dx[] = { 0, 0, -1, 1 };
	static const int dy[] = { -1, 1, 0, 0 };

	std::vector<std::pair<int, int>> neighbors;
	for (int d = 0; d < 4; d++)
	{
		int nx = _x + dx[d];
		int ny = _y + dy[d];
		if (ny >= 0 && ny < mapH && nx >= 0 && nx < mapW)
		{
			if (_map[ny][nx].m_tileType != TileType::None)
			{
				neighbors.push_back({ nx, ny });
			}
		}
	}

	// 行き止まり（隣接1個以下）や0個の場合は削除しない
	if (neighbors.size() <= 1) return false;

	// (x, y)をNoneにしたと仮定して、全隣接歩行マスが他の経路で繋がっているかBFSで判定
	FloorInfo backup = _map[_y][_x];
	_map[_y][_x].m_tileType = TileType::None;

	bool allConnected = true;
	for (size_t i = 1; i < neighbors.size(); i++)
	{
		std::queue<std::pair<int, int>> q;
		std::set<std::pair<int, int>> visited;

		q.push(neighbors[0]);
		visited.insert(neighbors[0]);

		bool reached = false;
		while (!q.empty())
		{
			auto cur = q.front();
			q.pop();

			if (cur == neighbors[i])
			{
				reached = true;
				break;
			}

			for (int d = 0; d < 4; d++)
			{
				int nx = cur.first + dx[d];
				int ny = cur.second + dy[d];
				if (ny >= 0 && ny < mapH && nx >= 0 && nx < mapW)
				{
					if (_map[ny][nx].m_tileType != TileType::None && visited.find({ nx, ny }) == visited.end())
					{
						// マンハッタン距離が離れすぎていないか確認（ローカルな迂回路探索）
						if (std::abs(nx - _x) + std::abs(ny - _y) <= 6)
						{
							visited.insert({ nx, ny });
							q.push({ nx, ny });
						}
					}
				}
			}
		}

		if (!reached)
		{
			allConnected = false;
			break;
		}
	}

	// 復元
	_map[_y][_x] = backup;

	return allConnected;
}

//===================================================
// 高さ・坂
//===================================================

void MapGenerate::SlopeCheck(std::vector<std::vector<FloorInfo>>* _map)
{
	int height = static_cast<int>(_map->size());
	if (height == 0) return;
	int width = static_cast<int>((*_map)[0].size());

	// 元のマップ情報をコピーして参照用にする（書き換えによるドミノ連鎖判定を防止）
	const std::vector<std::vector<FloorInfo>> origMap = *_map;

	auto isValid = [&](int _nx, int _ny) {
		return _ny >= 0 && _ny < height && _nx >= 0 && _nx < width;
		};

	auto canSlopeVertical = [&](int _x, int _y) {
		if (!isValid(_x, _y)) return false;
		if (origMap[_y][_x].m_tileType != TileType::Floor) return false;

		bool up = isValid(_x, _y - 1) && (origMap[_y - 1][_x].m_tileType != TileType::None);
		bool down = isValid(_x, _y + 1) && (origMap[_y + 1][_x].m_tileType != TileType::None);

		bool leftNone = !isValid(_x - 1, _y) || (origMap[_y][_x - 1].m_tileType == TileType::None);
		bool rightNone = !isValid(_x + 1, _y) || (origMap[_y][_x + 1].m_tileType == TileType::None);

		return up && down && leftNone && rightNone;
		};

	auto canSlopeHorizontal = [&](int _x, int _y) {
		if (!isValid(_x, _y)) return false;
		if (origMap[_y][_x].m_tileType != TileType::Floor) return false;

		bool left = isValid(_x - 1, _y) && (origMap[_y][_x - 1].m_tileType != TileType::None);
		bool right = isValid(_x + 1, _y) && (origMap[_y][_x + 1].m_tileType != TileType::None);

		bool upNone = !isValid(_x, _y - 1) || (origMap[_y - 1][_x].m_tileType == TileType::None);
		bool downNone = !isValid(_x, _y + 1) || (origMap[_y + 1][_x].m_tileType == TileType::None);

		return left && right && upNone && downNone;
		};

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (origMap[y][x].m_tileType != TileType::Floor) continue;

			int heightLevel = origMap[y][x].m_heightLevel;

			// --- 縦直線スロープ (y+1 と y-1) ---
			if (canSlopeVertical(x, y))
			{
				int upH = origMap[y + 1][x].m_heightLevel;
				int downH = origMap[y - 1][x].m_heightLevel;

				// 自分より高い隣が1つだけ、反対側は自分と同じ高さ、のときだけ坂にする
				if (upH == heightLevel + 1 && downH == heightLevel)
				{
					(*_map)[y][x].m_tileType = TileType::Slope;
					(*_map)[y][x].m_heightLevel = upH;
					(*_map)[y][x].m_angle = 0;      // 今と同じ向きの値
				}
				else if (downH == heightLevel + 1 && upH == heightLevel)
				{
					(*_map)[y][x].m_tileType = TileType::Slope;
					(*_map)[y][x].m_heightLevel = downH;
					(*_map)[y][x].m_angle = 180;
				}
			}
			// --- 横直線スロープ (x-1 と x+1) ---
			else if (canSlopeHorizontal(x, y))
			{
				int rightH = origMap[y][x + 1].m_heightLevel;
				int leftH = origMap[y][x - 1].m_heightLevel;

				// 右が1段高く、左は同じ高さ
				if (rightH == heightLevel + 1 && leftH == heightLevel)
				{
					(*_map)[y][x].m_tileType = TileType::Slope;
					(*_map)[y][x].m_heightLevel = rightH;
					(*_map)[y][x].m_angle = 270;
				}
				// 左が1段高く、右は同じ高さ
				else if (leftH == heightLevel + 1 && rightH == heightLevel)
				{
					(*_map)[y][x].m_tileType = TileType::Slope;
					(*_map)[y][x].m_heightLevel = leftH;
					(*_map)[y][x].m_angle = 90;
				}
			}
		}
	}
}

//===================================================
// できあがりのチェック
//===================================================

bool MapGenerate::MapCheck(std::vector<std::vector<FloorInfo>>& _map)
{


	//---- 高さの異常チェック -----
	for (int i = 0; i < _map.size(); i++)
	{
		for (int j = 0; j < _map[i].size(); j++)
		{
			if (_map[i][j].m_tileType == TileType::None) { continue; }

			// 下のマス
			if (i + 1 < _map.size() && j < _map[i + 1].size()
				&& _map[i + 1][j].m_tileType != TileType::None)
			{
				int gap = _map[i][j].m_heightLevel - _map[i + 1][j].m_heightLevel;
				if (abs(gap) > 0)
				{
					if (_map[i][j].m_tileType != TileType::Slope && _map[i + 1][j].m_tileType != TileType::Slope)
					{
						return false;
					}

					if (abs(gap) > 1)
					{
						return false;
					}
				}
			}

			// 右のマス
			if (j + 1 < _map[i].size()
				&& _map[i][j + 1].m_tileType != TileType::None)
			{
				int gap = _map[i][j].m_heightLevel - _map[i][j + 1].m_heightLevel;
				if (abs(gap) > 0)
				{
					if (_map[i][j].m_tileType != TileType::Slope && _map[i][j + 1].m_tileType != TileType::Slope)
					{
						return false;
					}

					if (abs(gap) > 1)
					{
						return false;
					}
				}
			}
		}
	}

	//---- 全部の部屋に歩いて行けるかチェック -----
	if (!m_roomInfo.empty())
	{
		int mapH = static_cast<int>(_map.size());
		int mapW = static_cast<int>(_map[0].size());

		static const int dx[] = { 0, 0, -1, 1 };
		static const int dy[] = { -1, 1, 0, 0 };

		//1つ目の部屋の中心から、歩いて行けるマスを全部たどる(BFS)
		std::vector<std::vector<bool>> visited(mapH, std::vector<bool>(mapW, false));
		std::queue<std::pair<int, int>> q;

		int sx = static_cast<int>(m_roomInfo[0].m_center.x);
		int sy = static_cast<int>(m_roomInfo[0].m_center.y);
		q.push({ sx, sy });
		visited[sy][sx] = true;

		while (!q.empty())
		{
			auto [x, y] = q.front();
			q.pop();

			for (int d = 0; d < 4; d++)
			{
				int nx = x + dx[d];
				int ny = y + dy[d];
				if (nx < 0 || nx >= mapW || ny < 0 || ny >= mapH) { continue; }
				if (visited[ny][nx]) { continue; }
				if (_map[ny][nx].m_tileType == TileType::None) { continue; }

				//同じ高さか、どちらかが坂で1段差なら進める
				int diff = std::abs(_map[y][x].m_heightLevel - _map[ny][nx].m_heightLevel);
				bool isSlope = (_map[y][x].m_tileType == TileType::Slope || _map[ny][nx].m_tileType == TileType::Slope);
				if (diff != 0 && !(isSlope && diff == 1)) { continue; }

				visited[ny][nx] = true;
				q.push({ nx, ny });
			}
		}

		//どれか1つでもたどり着けない部屋があれば異常
		for (const auto& room : m_roomInfo)
		{
			if (!visited[static_cast<int>(room.m_center.y)][static_cast<int>(room.m_center.x)])
			{
				return false;
			}
		}
	}

	//---- 部屋の数チェック -----
	//スポーン部屋・階段部屋・その他が別々に必要
	if (static_cast<int>(m_roomInfo.size()) < kMinRoomNum)
	{
		return false;
	}

	//---- 坂の向きと高さチェック -----
	for (int y = 0; y < static_cast<int>(_map.size()); y++)
	{
		for (int x = 0; x < static_cast<int>(_map[y].size()); x++)
		{
			if (_map[y][x].m_tileType != TileType::Slope) { continue; }

			//高い側と低い側のマス(SlopeCheck の向きと同じ)
			int hx = x, hy = y, lx = x, ly = y;
			switch (_map[y][x].m_angle)
			{
			case 0:   hy = y + 1; ly = y - 1; break;
			case 180: hy = y - 1; ly = y + 1; break;
			case 270: hx = x + 1; lx = x - 1; break;
			case 90:  hx = x - 1; lx = x + 1; break;
			default: return false;
			}

			//両側ともマップの中で、床がある
			if (hy < 0 || hy >= static_cast<int>(_map.size()) || hx < 0 || hx >= static_cast<int>(_map[0].size())) { return false; }
			if (ly < 0 || ly >= static_cast<int>(_map.size()) || lx < 0 || lx >= static_cast<int>(_map[0].size())) { return false; }
			if (_map[hy][hx].m_tileType == TileType::None || _map[ly][lx].m_tileType == TileType::None) { return false; }

			//高い側は坂と同じ高さ、低い側は1段低い
			int h = _map[y][x].m_heightLevel;
			if (_map[hy][hx].m_heightLevel != h) { return false; }
			if (_map[ly][lx].m_heightLevel != h - 1) { return false; }
		}
	}


	//異常なし
	return true;
}

//===================================================
// 壁・階段・松明
//===================================================

bool MapGenerate::IsNeedWall(int _nx, int _ny, const std::vector<std::vector<FloorInfo>>& _map, int _heightLevel, int _x, int _y)
{
	// マップ範囲外なら壁を作る
	if (_ny < 0 || _ny >= static_cast<int>(_map.size())) { return true; }
	if (_nx < 0 || _nx >= static_cast<int>(_map[_ny].size())) { return true; }

	// 隣のマスが None（空き地）なら壁を作る
	if (_map[_ny][_nx].m_tileType == TileType::None) { return true; }

	// 歩行可能マス同士（Room, Floor, Slopee）の間には壁を作らない（通路を塞がせない）
	return false;
}

std::shared_ptr<MapBase> MapGenerate::CreateWallOrStairs(const Math::Vector3& _pos, float _rotYDegree, bool _isStairs, std::list<std::shared_ptr<MapBase>>* _ret, int _roomID, int _x, int _y, std::vector<std::vector<FloorInfo>>& _map, Math::Vector2 _chunkNum, bool* _torchFlg, int _layerNum)
{
	if (_isStairs)
	{
		auto spStairs = std::make_shared<StairsBase>();
		spStairs->SetModel(m_spStairsModel);
		spStairs->Init();
		spStairs->SetPos(_pos);
		if (_rotYDegree != 0.0f)
		{
			spStairs->SetRotation(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(_rotYDegree)));
		}

		spStairs->SetMapObjType(MapObjType::Stairs);
		spStairs->SetRoomID(_roomID);
		_ret->push_back(spStairs);

		return spStairs;
	}
	else
	{
		auto spWall = std::make_shared<WallBase>();
		spWall->SetModel(m_spWallModel);
		spWall->Init();


		//モデルの一番下の y(原点基準)
		float wallBottomY = 0.0f;	
		if (auto spData = m_spWallModel->GetData())
		{
			bool first = true;
			for (int nodeIndex : spData->GetDrawMeshNodeIndices())
			{
				const auto& node = spData->GetOriginalNodes()[nodeIndex];
				if (!node.m_spMesh) { continue; }
				const DirectX::BoundingBox& box = node.m_spMesh->GetBoundingBox();
				float bottom = box.Center.y - box.Extents.y;
				if (first || bottom < wallBottomY) { wallBottomY = bottom; first = false; }
			}
		}
		Math::Vector3 wallPos = _pos;
		wallPos.y += -wallBottomY * static_cast<float>(_layerNum - 1);

		//順番は 位置 → 拡大 → 回転(SetRotation は今の位置と拡大を残して行列を作り直す)
		spWall->SetPos(wallPos);
		spWall->SetScale(Math::Vector3(1.0f, static_cast<float>(_layerNum), 1.0f));	//縦に伸ばす
		spWall->SetUVTiling({ 1.0f, static_cast<float>(_layerNum) });				//テクスチャを縦にくり返す
		if (_rotYDegree != 0.0f)
		{
			spWall->SetRotation(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(_rotYDegree)));
		}

		spWall->SetMapObjType(MapObjType::Wall);
		_ret->push_back(spWall);



		if (_torchFlg == nullptr)
		{
			return spWall;
		}


		if (!*_torchFlg)
		{
			return spWall;
		}


		//松明を置くかどうか
		bool placeTorch = false;



		if (_map[_y][_x].m_tileType == TileType::Room)
		{
			// コーナーなら置く
			if (IsCornerWall(_x, _y, _map))
			{
				placeTorch = true;
			}


			//周りになかったら置く
			if (IsSetTorch(_x, _y, _map, 3))
			{
				placeTorch = true;
			}
		}
		else
		{

			// コーナーなら置く
			if (IsCornerWall(_x, _y, _map))
			{
				placeTorch = true;
			}	//T字路なら置く
			else if (IsTIntersection(_x, _y, _map))
			{
				placeTorch = true;
			}

			//周りになかったら置く
			if (IsSetTorch(_x, _y, _map, 2))
			{
				placeTorch = true;
			}




		}

		bool placed = placeTorch && SetTorch(_rotYDegree, _pos, spWall, _chunkNum);
		if (placed)
		{
			_map[_y][_x].m_setTorchFlg = true;
		}

		*_torchFlg = !placed;

		return spWall;
	}
}

bool MapGenerate::SetTorch(float _rotYDegree, Math::Vector3 _pos, std::shared_ptr<KdGameObject> _spObj, Math::Vector2 _chunk)
{
	std::shared_ptr<MapObjManager>spMapObjManager = m_wpMapObjManager.lock();
	if (!spMapObjManager) { return false; }

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
	if (_spObj->Intersects(rayInfo, &retRayList))
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
		spTorch->SetChunkNum(_chunk);
		spTorch->SetRotation(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(_rotYDegree)));
		spTorch->SetCamera(m_wpCamera.lock());
		spMapObjManager->AddMapObj(spTorch);
		return true;
	}

	return false;
}

bool MapGenerate::IsCornerWall(int _x, int _y, const std::vector<std::vector<FloorInfo>>& _map)
{
	int height = static_cast<int>(_map.size());
	int width = static_cast<int>(_map[0].size());

	auto isValid = [&](int _nx, int _ny) {
		return _ny >= 0 && _ny < height && _nx >= 0 && _nx < width;
		};

	bool up = isValid(_x, _y - 1) && _map[_y - 1][_x].m_tileType != TileType::None;
	bool down = isValid(_x, _y + 1) && _map[_y + 1][_x].m_tileType != TileType::None;
	bool left = isValid(_x - 1, _y) && _map[_y][_x - 1].m_tileType != TileType::None;
	bool right = isValid(_x + 1, _y) && _map[_y][_x + 1].m_tileType != TileType::None;

	return (up != down) && (left != right);
}

bool MapGenerate::IsTIntersection(int _x, int _y, const std::vector<std::vector<FloorInfo>>& _map)
{
	int height = static_cast<int>(_map.size());
	int width = static_cast<int>(_map[0].size());

	auto isValid = [&](int _nx, int _ny) {
		return _ny >= 0 && _ny < height && _nx >= 0 && _nx < width;
		};

	int intersectionCount = 0;

	//上
	if (isValid(_x, _y - 1) && _map[_y - 1][_x].m_tileType != TileType::None)
	{
		intersectionCount++;
	}

	//下
	if (isValid(_x, _y + 1) && _map[_y + 1][_x].m_tileType != TileType::None)
	{
		intersectionCount++;
	}

	//左
	if (isValid(_x - 1, _y) && _map[_y][_x - 1].m_tileType != TileType::None)
	{
		intersectionCount++;
	}

	//右
	if (isValid(_x + 1, _y) && _map[_y][_x + 1].m_tileType != TileType::None)
	{
		intersectionCount++;
	}

	if (intersectionCount == 3)
	{
		return true;
	}

	return false;
}

bool MapGenerate::IsSetTorch(int _x, int _y, const std::vector<std::vector<FloorInfo>>& _map, int _num)
{
	int height = static_cast<int>(_map.size());
	int width = static_cast<int>(_map[0].size());

	auto isValid = [&](int _nx, int _ny) {
		return _ny >= 0 && _ny < height && _nx >= 0 && _nx < width;
		};


	//周囲にあったら設置できない

	//上下
	for (int i = -_num; i <= _num; i++)
	{
		if (isValid(_x, _y + i) && _map[_y + i][_x].m_setTorchFlg)
		{
			return false;
		}

	}

	//左右
	for (int i = -_num; i <= _num; i++)
	{
		if (isValid(_x + i, _y) && _map[_y][_x + i].m_setTorchFlg)
		{
			return false;
		}

	}



	return true;
}

//===================================================
// 通路の高さ(段差をまっすぐな所にだけ置く)
//===================================================

//パスの中で、段差(坂)を置けるマスの番号を集める(前後と一直線に並んでいる所だけ)
std::vector<int> MapGenerate::FindSlopeCandidates(const std::vector<Math::Vector2>& _path2D) const
{
	std::vector<int> candidates;

	//両端(部屋の出入口のマス)は除くので、1 〜 size-2 を見る
	for (int i = 1; i + 1 < static_cast<int>(_path2D.size()); i++)
	{
		const auto& prev = _path2D[i - 1];
		const auto& cur = _path2D[i];
		const auto& next = _path2D[i + 1];

		//横一直線か、縦一直線なら置ける
		bool sameY = (prev.y == cur.y && cur.y == next.y);
		bool sameX = (prev.x == cur.x && cur.x == next.x);

		if (sameX || sameY)
		{
			candidates.push_back(i);
		}
	}

	return candidates;
}

//段差を置く場所を、kSlopeMinGap 以上あけて必要な数だけ選ぶ
bool MapGenerate::PickSlopeIndices(const std::vector<int>& _candidates, int _needNum, std::vector<int>& _result) const
{
	_result.clear();
	if (_needNum <= 0) { return true; }

	//候補をシャッフルして、段差の場所がいつも同じにならないようにする
	std::vector<int> shuffled = _candidates;
	for (int i = static_cast<int>(shuffled.size()) - 1; i > 0; i--)
	{
		std::swap(shuffled[i], shuffled[KdRandom::GetInt(0, i)]);
	}

	//前から見て、すでに選んだ場所と kSlopeMinGap 以上離れていれば選ぶ
	for (int idx : shuffled)
	{
		bool farEnough = true;
		for (int picked : _result)
		{
			if (std::abs(idx - picked) < kSlopeMinGap)
			{
				farEnough = false;
				break;
			}
		}
		if (!farEnough) { continue; }

		_result.push_back(idx);
		if (static_cast<int>(_result.size()) >= _needNum) { break; }
	}

	//足りなかった
	if (static_cast<int>(_result.size()) < _needNum) { return false; }

	//高さを前から割り振るので、小さい順に並べる
	std::sort(_result.begin(), _result.end());
	return true;
}

//パスを、既存の床に当たった所(または隣に来た所)で切る
void MapGenerate::CutPathAtExisting(std::vector<Math::Vector2>& _path2D, const RoomInfo& _a, const std::vector<std::vector<FloorInfo>>& _map, int& _hB) const
{
	static const int dx[] = { 0, 0, -1, 1 };
	static const int dy[] = { -1, 1, 0, 0 };

	// (_x, _y) が部屋Aの中ならtrue
	auto inRoomA = [&](int _x, int _y)
		{
			return _x >= _a.m_roomEnd.m_farLeft && _x <= _a.m_roomEnd.m_farRight &&
				_y >= _a.m_roomEnd.m_topEnd && _y <= _a.m_roomEnd.m_downEnd;
		};

	// 部屋Aを出た後、部屋(Room B他)または既存の通路(Floor/Slope)に当たったらそこで切る
	for (int i = 0; i < static_cast<int>(_path2D.size()); i++)
	{
		int x = static_cast<int>(_path2D[i].x);
		int y = static_cast<int>(_path2D[i].y);

		if (y < 0 || y >= static_cast<int>(_map.size()) ||
			x < 0 || x >= static_cast<int>(_map[y].size()))
		{
			continue;
		}

		if (inRoomA(x, y)) { continue; }

		// 直接既存床に到達
		if (_map[y][x].m_tileType != TileType::None)
		{
			_hB = _map[y][x].m_heightLevel;
			_path2D.resize(i + 1);
			return;
		}

		// 隣接マス(上下左右)に部屋A以外の既存の床があれば、このマスで合流して切る
		for (int d = 0; d < 4; d++)
		{
			int nx = x + dx[d];
			int ny = y + dy[d];
			if (ny < 0 || ny >= static_cast<int>(_map.size()) ||
				nx < 0 || nx >= static_cast<int>(_map[ny].size()))
			{
				continue;
			}

			// 直前のマスは見ない
			if (i > 0 && nx == static_cast<int>(_path2D[i - 1].x) && ny == static_cast<int>(_path2D[i - 1].y)) { continue; }

			if (!inRoomA(nx, ny) && _map[ny][nx].m_tileType != TileType::None)
			{
				_hB = _map[ny][nx].m_heightLevel;
				_path2D.resize(i + 1);
				return;
			}
		}
	}
}

//高さを線形補間で割り振る(段差の場所が足りないときの予備)
std::vector<Math::Vector3> MapGenerate::AssignHeightsLinear(const std::vector<Math::Vector2>& _path2D, int _hA, int _hB) const
{
	std::vector<Math::Vector3> ans;

	int totalSteps = static_cast<int>(_path2D.size());
	if (totalSteps == 0) { return ans; }

	ans.reserve(totalSteps);

	int curH = _hA;
	ans.push_back({ _path2D[0].x, _path2D[0].y, static_cast<float>(curH) });

	for (int i = 1; i < totalSteps; i++)
	{
		int stepsLeftAfter = (totalSteps - 1) - i;
		int neededDiff = _hB - curH;

		// 残りのステップ数に対して高さ差が追いつかない場合（必要高さ差 >= 残りステップ数+1）、強制的に1ずつ高さを変化
		if (std::abs(neededDiff) >= stepsLeftAfter + 1)
		{
			int step = (neededDiff > 0) ? 1 : -1;
			curH += step;
		}
		else
		{
			// 通常は線形補間で目標高さを決定し、差分を反映
			float ratio = static_cast<float>(i) / static_cast<float>(totalSteps - 1);
			float hFloat = static_cast<float>(_hA) + static_cast<float>(_hB - _hA) * ratio;
			int targetH = static_cast<int>(std::round(hFloat));

			int step = std::clamp(targetH - curH, -1, 1);
			curH += step;
		}

		ans.push_back({ _path2D[i].x, _path2D[i].y, static_cast<float>(curH) });
	}

	return ans;
}
