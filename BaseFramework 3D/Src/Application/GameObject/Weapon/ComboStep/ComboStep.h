#pragma once


// コンボ
struct ComboStep
{
	// アニメーション名
	std::string m_animeName;

	//何コンボ目に出すものなのか(0は1段目)
	int m_comboIndex = 0;

	////////////////////////////////
	// 進行度はすべて、このアニメーションでの 0〜1

	// 再生速度の倍率
	float m_speed = 1.0f;

	// 次の段の入力を受け付け始める進行度（最後の段は 9.9 のまま）
	float m_acceptStart = 9.9f;

	// 予約があれば、ここで次の段へ移る進行度
	float m_cancelPoint = 9.9f;

	// この進行度以降は回避で攻撃を切れる
	float m_evasionCancel = 0.6f;

	// 当たり判定を出す範囲
	float m_hitStart = 0.3f;
	//当たり判定を出し終わる進行度
	float m_hitEnd = 0.6f;

	// 軌跡を出す範囲
	float m_trailStart = 0.3f;
	//軌跡を出し終わる進行度
	float m_trailEnd = 0.6f;

	////////////////////////////////
	// 威力・手触り

	// ダメージ倍率
	float m_damageRate = 1.0f;

	// ノックバック倍率
	float m_knockbackRate = 1.0f;

	// ヒットストップの時間（秒）
	float m_hitStopTime = 0.03f;

	// 段の開始時に前へ踏み込む距離（m）
	float m_stepForward = 0.0f;

	// この段に進むときに使うスタミナ
	float m_staminaCost = 0.0f;
};