#pragma once
#include"../UIBase.h"

class GameScene;

//ミニマップの上に今の階数を出す(下に線を引く)
class FloorNumUI :public UIBase
{
public:
	FloorNumUI() {};
	~FloorNumUI()override {};

	void Init()override;
	void DrawSprite()override;

	//階数を取るためのゲームシーンをセット
	void SetGameScene(std::shared_ptr<GameScene> _spGameScene) { m_wpGameScene = _spGameScene; }

private:
	std::weak_ptr<GameScene> m_wpGameScene;

	//「階層」の文字
	std::shared_ptr<KdTexture> m_spFloorTex;

	//下線
	std::shared_ptr<KdTexture> m_spUnderlineTex;

	//表示位置(数字の右端)
	const Math::Vector2 kPos = { -590.0f, 315.0f };

	//数字の大きさ(元の画像は1文字 9×14)
	const float kNumSize = 2.0f;

	//数字と「階」の文字のすき間
	const float kTextGap = 4.0f;

	//下線と文字のすき間
	const float kLineGap = 4.0f;

	//下線を数字の左端よりどれだけ左から始めるか
	const float kLineExtend = 6.0f;
};
