#pragma once

//ゲーム内で使うデルタタイム
class DeltaTime
{
public:
	//実デルタタイムを使う
	// _realDelta ... 前のフレームからの実際の経過時間(秒)
	void Update(float _realDelta)
	{
		// フレーム落ち対策
		_realDelta = std::min(_realDelta, m_maxDelta);

		m_realDeltaTime = _realDelta;

		m_slowTimer -= DeltaTime::Instance().GetRealDeltaTime();
		if (m_slowTimer <= 0.0f)
		{
			SetTimeScale(1.0f);
		}

		// ヒットストップ中なら時間を止める
		if (m_hitStopTimer > 0.0f)
		{
			m_hitStopTimer -= m_realDeltaTime;
			m_gameDeltaTime = 0.0f;
		}
		else
		{
			m_gameDeltaTime = m_realDeltaTime * m_timeScale;
		}
	}

	//ゲーム内のデルタタイム(ヒットストップ・スロー込み)を取得
	//戻り値 ... ゲーム内のデルタタイム(秒)
	float GetGameDeltaTime() const { return m_gameDeltaTime; }
	//実際のデルタタイム(ヒットストップ・スローの影響なし)を取得
	//戻り値 ... 実際のデルタタイム(秒)
	float GetRealDeltaTime() const { return m_realDeltaTime; }

	//ヒットストップする(その間ゲーム内の時間が止まる)
	// _duration ... 止める時間(秒)
	void HitStop(float _duration)
	{
		// 今のヒットストップより短いなら上書きしない（とどめの長い止まりを、次の弱い当たりで縮めない）
		m_hitStopTimer = std::max(m_hitStopTimer, _duration);
	}

	//時間の進む速さをセット
	// _scale ... 速さの倍率(1で通常、0.5で半分の速さ)
	void SetTimeScale(float _scale)
	{
		m_timeScale = _scale;
	}

	//スロー(時間の速さの変更)を続ける時間をセット。0になったら速さが1に戻る
	// _slowTimer ... 続ける時間(秒)
	void SetSlowTimer(float _slowTimer)
	{
		m_slowTimer = _slowTimer;
	}

private:
	//前のフレームの時刻(ミリ秒)
	DWORD m_prevTime = timeGetTime();
	//ゲーム内のデルタタイム(秒)
	float m_gameDeltaTime = 0.0f;
	//時間の進む速さの倍率
	float m_timeScale = 1.0f;
	//ヒットストップの残り時間(秒)
	float m_hitStopTimer = 0.0f;
	float m_maxDelta = 0.033f; // 最大33ms

	//実際のデルタタイム(秒)
	float m_realDeltaTime = 0;



	//スローの残り時間(秒)
	float m_slowTimer = 0;
	//=====================================================
	// シングルトンパターン
	//=====================================================

private:
	// 
	DeltaTime() {}

public:
	//インスタンスを取得する(シングルトン)
	//戻り値 ... DeltaTimeのインスタンス
	static DeltaTime& Instance() {
		static DeltaTime instance;
		return instance;
	}
};