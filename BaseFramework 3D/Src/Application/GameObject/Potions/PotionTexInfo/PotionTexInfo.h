#pragma once

//アイテム1つ分の番号・名前と画像(アイコン・説明・名前)
struct Texs
{
	//アイテムの番号
	int m_ID = 0;
	//アイテムの名前(フォルダ名)
	std::string m_name;
	//アイコンの画像
	std::shared_ptr<KdTexture>m_spIcon;
	//説明の画像
	std::shared_ptr<KdTexture>m_spExplanation;
	//名前の画像
	std::shared_ptr<KdTexture>m_spName;
};


class PotionTexInfo
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ(画像を読み込む)
	PotionTexInfo() { Load(); };

	//デストラクタ
	~PotionTexInfo() {};

	//---- 取得 ----

	//ポーションのアイコンを返す
	// _ID ... ポーションの番号
	//戻り値 ... アイコンの画像(無ければnullptr)
	std::shared_ptr<KdTexture> GetPotionIcon(int _ID);

	//ポーションの説明の画像を返す
	// _ID ... ポーションの番号
	//戻り値 ... 説明の画像(無ければnullptr)
	std::shared_ptr<KdTexture> GetPotionExplanation(int _ID);

	//ポーションの名前の画像を返す
	// _ID ... ポーションの番号
	//戻り値 ... 名前の画像(無ければnullptr)
	std::shared_ptr<KdTexture> GetPotionName(int _ID);

	//武器強化のアイコンを返す
	// _ID ... 武器強化の番号
	//戻り値 ... アイコンの画像(無ければnullptr)
	std::shared_ptr<KdTexture> GetWeaponsStrengtheningIcon(int _ID);

	//武器強化の説明の画像を返す
	// _ID ... 武器強化の番号
	//戻り値 ... 説明の画像(無ければnullptr)
	std::shared_ptr<KdTexture> GetWeaponsStrengtheningExplanation(int _ID);

	//武器強化の名前の画像を返す
	// _ID ... 武器強化の番号
	//戻り値 ... 名前の画像(無ければnullptr)
	std::shared_ptr<KdTexture> GetWeaponsStrengtheningName(int _ID);

	//武器強化用アイテムのアイコンを返す
	// _ID ... 武器強化用アイテムの番号
	//戻り値 ... アイコンの画像(無ければnullptr)
	std::shared_ptr<KdTexture> GetWeaponsStrengtheningItemIcon(int _ID);

	// _ID ... 武器強化用アイテムの番号
	std::shared_ptr<KdTexture> GetWeaponsStrengtheningItemName(int _ID);	// 武器強化用アイテムの名前の画像

	// _ID ... 武器強化用アイテムの番号
	std::shared_ptr<KdTexture> GetWeaponsStrengtheningItemExplanation(int _ID);

private:

	//---- メンバ変数 ----

	//ポーションの画像一覧
	std::vector<Texs> m_potionTexs;

	//武器強化の画像一覧
	std::vector<Texs> m_weaponsStrengtheningTexs;

	//武器強化用アイテムの画像一覧
	std::vector<Texs> m_weaponsStrengtheningItemTexs;

	//ポーションの画像フォルダ
	std::string m_potionPath = "Asset/Textures/GameUI/Item/Potion/";

	//武器強化の画像フォルダ
	std::string m_weaponsStrengtheningPath = "Asset/Textures/GameUI/Item/WeaponsStrengthening/";

	//武器強化用アイテムの画像フォルダ
	std::string m_weaponsStrengtheningItemPath = "Asset/Textures/GameUI/Item/WeaponsStrengtheningItem/";

	//アイコン画像のファイル名
	std::string m_icon = "/Icon.png";

	//説明画像のファイル名
	std::string m_explanation = "/SelectExplanationBase.png";

	//名前画像のファイル名
	std::string m_name = "/SelectNameBase.png";

	//---- その他の処理 ----

	//jsonの一覧から画像を全部読み込む
	void Load();
};
