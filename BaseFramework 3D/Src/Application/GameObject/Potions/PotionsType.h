#pragma once

//=============================================
// ポーションの定義
//
// ポーションを増やす・消すときに触るのは、このファイルの
//   ① enum PotionsType
//   ② kPotionDefs(表)
// の2か所だけ。あとは
//   ・画像を Asset/Textures/GameUI/Item/Potion/<名前>/ に置く
//   ・使ったときの処理を PotionUseController の表に足す(効果があるものだけ)
//   ・値段をショップのエディタで入れる
// JSONは「名前」で読むので、番号を振り直す必要はない
//=============================================

//ポーションの種類(この順番がゲーム内の番号になる)
enum PotionsType
{
	//即座に回復
	PotionsType_InstantHeal = 0,

	//攻撃力が一時的にアップ
	PotionsType_AttackBoostPotion,

	// スタミナ即座に回復
	PotionsType_InstantStaminaHeal,

	// スタミナ量増加（最大スタミナ増加）
	PotionsType_StaminaMaxUP,

	// スタミナ回復量アップ（時間制限）
	PotionsType_StaminaRegenBoost,


	//ポーション種類数
	PotionsTypeMax
};

//ポーション1種類分の情報
struct PotionDef
{
	PotionsType m_type;		// 種類
	const char* m_name;		// 名前(画像フォルダ名・JSONの"name")
};

//ポーションの一覧(enum と同じ順番で書く)
inline constexpr PotionDef kPotionDefs[] =
{
	{ PotionsType_InstantHeal,        "InstantHealPotion"        },
	{ PotionsType_AttackBoostPotion,  "AttackBoostPotion"        },
	{ PotionsType_InstantStaminaHeal, "InstantStaminaHealPotion" },
	{ PotionsType_StaminaMaxUP,       "StaminaMaxUPPotion"       },
	{ PotionsType_StaminaRegenBoost,  "StaminaRegenBoostPotion"  },
};

//enum と表の数がずれていたらコンパイルエラーにする(追加し忘れ防止)
static_assert(sizeof(kPotionDefs) / sizeof(kPotionDefs[0]) == PotionsTypeMax,
	"PotionsType と kPotionDefs の数が合っていません");

//番号からポーションの名前を返す
// _id ... ポーションの番号
//戻り値 ... 名前(範囲外なら空文字)
inline const char* GetPotionSTName(int _id)
{
	if (_id < 0 || _id >= PotionsTypeMax) { return ""; }
	return kPotionDefs[_id].m_name;
}

//名前からポーションの番号を返す
// _name ... ポーションの名前
//戻り値 ... 番号(見つからなければ -1)
inline int FindPotionID(const std::string& _name)
{
	for (int i = 0; i < PotionsTypeMax; i++)
	{
		if (_name == kPotionDefs[i].m_name) { return i; }
	}
	return -1;
}

//JSONのポーション1件から番号を返す
//"name" があれば名前で探す(消したポーションは -1)。無ければ古い形式として "id" を使う
// _item ... JSONの1件
//戻り値 ... 番号(使えなければ -1)
inline int GetJsonPotionID(const nlohmann::json& _item)
{
	if (_item.contains("name") && _item["name"].is_string())
	{
		return FindPotionID(_item["name"].get<std::string>());
	}
	return _item.value("id", -1);
}

//JSONのアイテム1件から番号を返す
//"potions" の配列だけ名前で探し、それ以外(素材・武器)は今まで通り "id" を使う
// _item ... JSONの1件
// _key  ... その1件が入っていた配列の名前("potions" / "materials" など)
//戻り値 ... 番号(使えなければ -1)
inline int GetJsonItemID(const nlohmann::json& _item, const std::string& _key)
{
	if (_key == "potions") { return GetJsonPotionID(_item); }
	return _item.value("id", -1);
}
