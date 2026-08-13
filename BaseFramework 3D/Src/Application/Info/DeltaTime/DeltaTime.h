#pragma once

//ゲーム内で使うデルタタイム
class DeltaTime
{
public:
	//実デルタタイムを使う
	void Update(float realDelta)
	{
		// フレーム落ち対策
		realDelta = std::min(realDelta, m_maxDelta);

		m_realDeltaTaime = realDelta;

		m_slowTimer -= DeltaTime::Instance().GetRealDeltaTime();
		if (m_slowTimer <= 0.0f)
		{
			SetTimeScale(1.0f);
		}

		// ヒットストップ中なら時間を止める
		if (m_hitStopTimer > 0.0f)
		{
			m_hitStopTimer -= m_realDeltaTaime;
			m_gameDeltaTime = 0.0f;
		}
		else
		{
			m_gameDeltaTime = m_realDeltaTaime * m_timeScale;
		}
	}

	float GetGameDeltaTime() const { return m_gameDeltaTime; }
	float GetRealDeltaTime() const { return m_realDeltaTaime; }

	void HitStop(float duration)
	{
		m_hitStopTimer = duration;
	}

	void SetTimeScale(float scale)
	{
		m_timeScale = scale;
	}

	void SetSlowTimer(float slowTimer)
	{
		m_slowTimer = slowTimer;
	}

private:
	DWORD m_prevTime = timeGetTime();
	float m_gameDeltaTime = 0.0f;
	float m_timeScale = 1.0f;
	float m_hitStopTimer = 0.0f;
	float m_maxDelta = 0.033f; // 最大33ms

	float m_realDeltaTaime = 0;



	float m_slowTimer = 0;
	//=====================================================
	// シングルトンパターン
	//=====================================================

private:
	// 
	DeltaTime() {}

public:
	static DeltaTime& Instance() {
		static DeltaTime Instance;
		return Instance;
	}
};