#pragma once
#include"../../UIBase.h"

#include"../../../../Scene/SceneManager.h"
class ResultDisplay_Score;
class ResultDisplayBase;
class ResultDisplay_PotionInventory;
class ResultDisplay_PlayerStatus;
class ResultDisplay_WeponInventory;
class WeaponBase;

struct Inventory;

//リザルト画面
class ResultDisplay :public UIBase
{
public:
	//コンストラクタ
	ResultDisplay() {};
	//デストラクタ
	~ResultDisplay() {};

	//初期化
	void Init()override;

	//更新
	void Update()override;

	//描画前の処理
	void PreDraw()override;
	//2Dの描画
	void DrawSprite()override;


	//パスをセット(今は何もしない)
	// _path ... パス
	void SetPath(std::string _path);

	//各階の結果をセット
	// _displayFloorInfo ... 全体の結果
	void SetDisplayFloorInfo(const DisplayFloorInfo& _displayFloorInfo);


	//持ち物の一覧をセット
	// _playerInventoryList ... 持ち物の一覧([0]ポーション/[1]武器強化素材)
	void SetPlayerInventoryList(std::vector<std::vector<Inventory>> _playerInventoryList);

	//ゲーム終了時のステータスのファイルのパスをセット
	// _path ... ファイルのパス
	void SetPlayerStatusGameResultFilePath(std::string _path);

	//ゲーム終了時の武器をセット
	// _spWeaponBase ... 武器
	void SetSpWeaponBase(std::shared_ptr<WeaponBase> _spWeaponBase);


	//プレイヤーが死んだかをセット
	// _flg ... 死んだか
	void SetPlayerDead(bool _flg);
private:
	//背景の画像
	std::shared_ptr<KdTexture>m_spResultBackTex;
	//背景2の画像
	std::shared_ptr<KdTexture>m_spResultBack2Tex;
	//スコアのページ
	std::shared_ptr<ResultDisplay_Score>m_spResultDisplay_Score;
	//ステータスのページ
	std::shared_ptr<ResultDisplay_PlayerStatus>m_spResultDisplay_PlayerStatus;
	//武器強化のページ
	std::shared_ptr<ResultDisplay_WeponInventory>m_spResultDisplay_WeaponInventory;

	//ポーションのページ
	std::shared_ptr<ResultDisplay_PotionInventory>m_spResultDisplay_PotionInventory;

	//ページの一覧
	std::vector<std::shared_ptr<ResultDisplayBase>>m_resultDisplayList;
	//今のページの番号
	int m_currentDisplayIndex = 0;



	//矢印の画像
	std::shared_ptr<KdTexture>m_spArrowTex;
	//左の矢印の位置
	Math::Vector2 m_arrowLPos;
	//右の矢印の位置
	Math::Vector2 m_arrowRPos;
	//矢印の大きさ
	Math::Vector2 m_arrowSize;

	//右の矢印にマウスが乗っているか
	bool m_arrowRHitFlg = false;
	//左の矢印にマウスが乗っているか
	bool m_arrowLHitFlg = false;

	//マウスが乗っているときの矢印の倍率
	float m_arrowHitScale = 1.1;

	//矢印のアルファ
	float m_arrowAlpha = 1;
	//1秒あたりの矢印のアルファの増減
	float m_addArrowAlpha = 1;

	//矢印の更新(クリックでページを変える)
	void ArrowUpdate();
	//矢印の描画
	void ArrowDrawSprite();


	//持ち物の一覧
	std::vector<std::vector<Inventory>> m_playerInventoryList;

	//プレイヤーが死んだか
	bool m_playerDead = false;





	//「LOST」のハンコの画像
	std::shared_ptr<KdTexture>m_spLostTex;
	//ハンコの位置
	Math::Vector2 m_lostTexPos;
	//ハンコの大きさ
	Math::Vector2 m_lostTexSize;
	//---------- Lost（失敗のハンコ）の演出 ----------
	// ハンコを表示してからの経過時間（秒）
	// 0.4秒までは何も出さず、そこから押すアニメーションに使う
	float m_lostTimer = 0;

	// もうハンコを押し終わったか
	// 一度 true になったら、ページを移っても押した後の状態で描く（毎回押し直さない）
	bool m_lostStampedFlg = false;

	// 押した瞬間の画面の揺れの残り時間（秒）
	// 0より大きい間だけ揺らし、毎フレーム減らしていく
	float m_lostShakeTimer = 0;



	//Lost（失敗のハンコ）の更新（タイマーを進める）
	void LostUpdate();

	//Lost（失敗のハンコ）の描画（大きさ・透明度・光・揺れを計算して描く）
	void LostDraw();


	//---------- Lost（失敗のハンコ）の演出の時間 

	const float kLostDelayTime = 0.4f;	// ページを開いてからハンコが出始めるまで
	const float kLostFallTime = 0.15f;	// 大きいところから押されるまで
	const float kLostBounceTime = 0.15f;	// 押した後に少し跳ね返る時間
	const float kLostShakeTime = 0.25f;	// 押した瞬間の揺れの長さ

	const float kLostStartScale = 2.5f;	// 押す前の大きさ（倍）
	const float kLostBounceScale = 0.08f;	// 跳ね返りの大きさ（1 + この値 まで膨らむ）
	const float kLostShakePower = 6.0f;	// 揺れの強さ（ピクセル）

	// ロストのハンコの透明度（少し透けさせて、インクが染みた感じにする）
	static constexpr float kLostStampAlpha = 0.85f;

	// ロストのハンコの基本の傾き（度）。マイナスで左上が上がる向き
	static constexpr float kLostStampBaseAngle = -15.0f;

	// 押すたびに付けるブレの幅（度）。基本の傾き ± この値の範囲で決まる
	static constexpr float kLostStampAngleRange = 10.0f;

	// 今回押したハンコの傾き（度）。押すときに一度だけ決める
	float m_lostStampAngle = kLostStampBaseAngle;
};
