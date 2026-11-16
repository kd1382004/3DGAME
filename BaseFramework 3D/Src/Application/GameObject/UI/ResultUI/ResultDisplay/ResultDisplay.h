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

class ResultDisplay :public UIBase
{
public:
	ResultDisplay() {};
	~ResultDisplay() {};

	void Init()override;

	void Update()override;

	void PreDraw()override;
	void DrawSprite()override;


	void SetPath(std::string path);

	void SetDisplayFloorInfo(std::vector<DisplayFloorInfo>_displayFloorInfo);


	void SetPlayerInventoryList(std::vector<std::vector<Inventory>> _playerInventoryList);

	void SetPlayerStatusGameResultFilePath(std::string _path);

	void SetSpWeaponBase(std::shared_ptr<WeaponBase> spWeaponBase);


	void SetPlayerDead(bool _flg);
private:
	std::shared_ptr<KdTexture>m_ResultBackTex;
	std::shared_ptr<KdTexture>m_ResultBack2Tex;
	std::shared_ptr<ResultDisplay_Score>m_ResultDisplay_Score;
	std::shared_ptr<ResultDisplay_PlayerStatus>m_ResultDisplay_PlayerStatus;
	std::shared_ptr<ResultDisplay_WeponInventory>m_ResultDisplay_WeponInventory;

	std::shared_ptr<ResultDisplay_PotionInventory>m_ResultDisplay_PotionInventory;

	std::vector<std::shared_ptr<ResultDisplayBase>>m_ResultDisplayList;
	int m_CurrentDisplayIndex = 0;



	std::shared_ptr<KdTexture>m_ArrowTex;
	Math::Vector2 m_ArrowLPos;
	Math::Vector2 m_ArrowRPos;
	Math::Vector2 m_ArrowSiz;

	bool m_ArrowRHiFlg = false;
	bool m_ArrowLHiFlg = false;

	float m_AroowHitScale = 1.1;

	float m_AroowAlpha = 1;
	float m_AddAroowAlpha = 1;

	void ArrowUpdate();
	void ArrowDrawSprite();


	std::vector<std::vector<Inventory>> m_playerInventoryList;

	bool m_playerDead = false;





	std::shared_ptr<KdTexture>m_lostTex;
	Math::Vector2 m_lostTexPos;
	Math::Vector2 m_lostTexSiz;
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
