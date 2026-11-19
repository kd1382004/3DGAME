#pragma once
#include"../UIBase.h"

class HPBar :public UIBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	HPBar() {};

	//デストラクタ
	~HPBar()override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新
	void Update()override;

	//描画前の処理
	void PreDraw()override;

	//2Dの描画
	void DrawSprite()override;

	//---- 設定 ----

	//パーセント(0.0f～1.0f)

	//遅れて減る演出あり
	// _percent ... HPの割合(0〜1)
	void SetHPBarTexPercent(float _percent);

	//遅れて減る演出なし
	// _percent ... HPの割合(0〜1)
	void SetHPBarTexPercentNODelayOffset(float _percent);

	//表示するかをセット
	// _flg ... 表示するか
	void SetDrawFlg(bool _flg) { m_drawFlg = _flg; }

	//大きさの倍率をセット
	// _siz ... 倍率
	void SetSiz(float _siz) { m_siz = _siz; }

	//表示するレベルをセット
	// _LV ... レベル
	void SetLV(int _LV);

	//---- その他の処理 ----

	//画面上の座標をセット(バーの左端の位置も計算する)
	// _pos ... 画面上の座標(バーの中心)
	void Set2DPos(const Math::Vector2& _pos) {
		m_2DPos = _pos;
		m_HPBarBackTex2DPos = { m_2DPos.x - m_HPBarBackTexLength.x / 2.0f * m_siz ,m_2DPos.y };
		m_HPBarTex2DPos = { m_2DPos.x - m_HPBarTexLength.x / 2.0f * m_siz ,m_2DPos.y };
		m_HPBarDelayTex2DPos = { m_2DPos.x - m_HPBarTexLength.x / 2.0f * m_siz ,m_2DPos.y };
		m_HPBarIconFrameTex2DPos = { m_HPBarBackTex2DPos.x - m_HPBarIconFrameTexLength.x / 2.0f * m_siz ,m_2DPos.y };
	}

	//消すときの処理(今は何もしない)
	void Delete();

private:

	//---- メンバ変数 ----

	//表示するか
	bool m_drawFlg = false;

	//サイズ
	float m_siz = 1.0f;

	//HPバー背景
	std::shared_ptr<KdTexture>m_spHPBarBackTex = nullptr;

	//背景の描画位置
	Math::Vector2 m_HPBarBackTex2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_HPBarBackTexLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle m_HPBarBackTexRect;

	//HPバー本体
	std::shared_ptr<KdTexture>m_spHPBarTex = nullptr;

	//本体の描画位置
	Math::Vector2 m_HPBarTex2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_HPBarTexLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle  m_HPBarTexRect;

	//色
	Math::Color  	m_HPBarTexColor = { 0.35f, 0.85f, 0.30f, 1.0f };

	//パーセント
	float m_HPBarTexPercent = 1;

	//進捗度
	float m_HPBarTexProgress = 0;

	//進捗を終わらせる秒数
	float m_HPBarDelayTexPercentDownSpeed = 1;

	//遅れて減るバーの描画位置
	Math::Vector2 m_HPBarDelayTex2DPos = Math::Vector2::Zero;

	//色
	Math::Color  m_HPBarDelayTexColor = { 0.85f, 0.2f, 0.15f, 1.0f };

	//画像の切り取り範囲
	Math::Rectangle  m_HPBarDelayTexRect;

	//パーセント
	float m_HPBarDelayTexPercent = 1;

	//アイコン用フレーム
	std::shared_ptr<KdTexture>m_spHPBarIconFrameTex = nullptr;

	//アイコン用フレームの描画位置
	Math::Vector2 m_HPBarIconFrameTex2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_HPBarIconFrameTexLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle m_HPBarIconFrameTexRect;

	//LV
	int m_LV = 1;

	//---- その他の処理 ----

	//遅れて減るバーの描画前の処理
	void HPBarDelayPreDraw();
};
