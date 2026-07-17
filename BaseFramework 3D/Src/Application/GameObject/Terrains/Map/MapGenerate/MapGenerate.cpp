#include "MapGenerate.h"
#include"../MapBase.h"


MapGenerate::MapGenerate()
{
	m_roomSizPath = "Asset/Data/ObjeData/Terrains/Map/MapSiz.json";

	LoadRoomSiz(m_roomSizPath);

	SaveRoomSiz(m_roomSizPath);
}

void MapGenerate::Generate(Math::Vector2 _mapSiz, int roomNum, float tileSiz, MapType _type, std::list<std::shared_ptr<MapBase>>* ret)
{
	if (roomMine <= 0 && roomMax <= 0)
	{
		roomMax = 1;
		roomMine = 0;
	}



	//マップのサイズを作る
	//TileType::Noneで初期化(何もない)
	std::vector<std::vector<int>> map(static_cast<size_t>(_mapSiz.y), std::vector<int>(static_cast<size_t>(_mapSiz.x), static_cast<int>(TileType::None)));


	// 1部屋につき最大50回試行、できなかったらスキップ
	int maxTry = 50;

	for (int i = 0; i < roomNum; i++)
	{
		for (int t = 0; t < maxTry; t++)
		{
			/////////////////////////////////////////////////////
		
			// 部屋のサイズ決定
			
			//横
			int roomW = rand() % (roomMax - roomMine) + roomMine;

			//縦
			int roomH = rand() % (roomMax - roomMine) + roomMine;

			/////////////////////////////////////////////////////


			/////////////////////////////////////////////////////
			
			// 部屋の左上番号
			int X = _mapSiz.x;
			int Y = _mapSiz.y;
			int roomX = rand() % (X - roomW - 1);
			int roomY = rand() % (Y - roomH - 1);

			// 部屋が空いてるかチェック
			bool canPlace = true;

			//部屋同士何タイル開けるか
			int aX = rand() % 5 + 1;
			int aY = rand() % 3 + 1;

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
					}
				}
				break; // この部屋は成功したので次の部屋へ
			}

			/////////////////////////////////////////////////////
		}
	}



	for (int y = 0; y < map.size(); y++)
	{
		for (int x = 0; x < map[y].size(); x++)
		{

			/////////////////////////////////////////////////////
			
			//床と通路を保存
			if (map[y][x] != static_cast<int>(TileType::None))
			{
				float xPos = -(_mapSiz.x / 2.0f * tileSiz);
				float zPos = -(_mapSiz.y / 2.0f * tileSiz);

				std::shared_ptr<MapBase> mapA = std::make_shared<MapBase>();


				xPos += tileSiz * x;
				zPos += tileSiz * y;

				Math::Vector3 pos = { xPos,0,zPos };

				mapA->Init();
				mapA->SetPos(pos);
				ret->push_back(mapA);
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
