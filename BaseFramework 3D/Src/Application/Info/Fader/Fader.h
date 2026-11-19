#pragma once

//フェードの状態
enum class FadeState
{
	Hidden,     // 非表示（描画しない）
	FadingIn,   // 出現中
	Visible,    // 表示中
	FadingOut,  // 消失中
};

//============================================================
// 出現・消失の進み具合を管理するクラス
//	持ち主のクラスにメンバとして持たせて使う
//	GetDissolve()   ... ディゾルブにそのまま渡せる値(0 = 表示, 1 = 消失)
//	GetVisibility() ... アルファにそのまま渡せる値(1 = 表示, 0 = 消失)
//============================================================
class Fader
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	// _duration  ... フェードにかける時間(秒)
	// _initState ... 最初の状態
	Fader(float _duration = 0.5f, FadeState _initState = FadeState::Hidden)
		: m_state(_initState)
		, m_duration(_duration)
	{
	}

	//---- 基本の処理 ----

	//フェードを進める(毎フレーム呼ぶ)
	// _deltaTime ... 経過時間(秒)
	void Update(float _deltaTime)
	{
		if (!IsFading()) { return; }

		m_elapsed += _deltaTime;

		float rate = 1.0f;
		if (m_duration > 0.0f)
		{
			rate = std::clamp(m_elapsed / m_duration, 0.0f, 1.0f);
		}

		// 加速(最初ゆっくり → だんだん速く)
		m_progress = rate * rate;

		if (rate >= 1.0f)
		{
			if (m_state == FadeState::FadingIn)
			{
				m_state = FadeState::Visible;
			}
			else
			{
				m_state = FadeState::Hidden;
			}
		}
	}

	//出現を始める(表示中・出現中なら何もしない)
	void FadeIn()
	{
		if (m_state == FadeState::Visible || m_state == FadeState::FadingIn) { return; }
		Start(FadeState::FadingIn);
	}

	//消失を始める(非表示・消失中なら何もしない)
	void FadeOut()
	{
		if (m_state == FadeState::Hidden || m_state == FadeState::FadingOut) { return; }
		Start(FadeState::FadingOut);
	}

	//フェードなしで表示にする
	void SetVisibleImmediate()
	{
		m_state = FadeState::Visible;
		m_elapsed = 0.0f;
		m_progress = 0.0f;
	}

	//フェードなしで非表示にする
	void SetHiddenImmediate()
	{
		m_state = FadeState::Hidden;
		m_elapsed = 0.0f;
		m_progress = 0.0f;
	}

	//---- 取得 ----

	//ディゾルブの値を返す
	//戻り値 ... 0 = 完全に表示, 1 = 完全に消失
	float GetDissolve() const
	{
		switch (m_state)
		{
		case FadeState::FadingIn:  return 1.0f - m_progress;
		case FadeState::FadingOut: return m_progress;
		case FadeState::Hidden:    return 1.0f;
		default:                   return 0.0f;	// Visible
		}
	}

	//見えている割合を返す(アルファ用)
	//戻り値 ... 1 = 完全に表示, 0 = 完全に消失
	float GetVisibility() const { return 1.0f - GetDissolve(); }

	//今の状態を返す
	//戻り値 ... 状態
	FadeState GetState() const { return m_state; }

	//非表示か
	//戻り値 ... 非表示ならtrue
	bool IsHidden() const { return m_state == FadeState::Hidden; }

	//表示中か
	//戻り値 ... 完全に表示されていたらtrue
	bool IsVisible() const { return m_state == FadeState::Visible; }

	//フェード中か
	//戻り値 ... 出現中・消失中ならtrue
	bool IsFading() const { return m_state == FadeState::FadingIn || m_state == FadeState::FadingOut; }

	//---- 設定 ----

	//フェードにかける時間をセット
	// _duration ... 時間(秒)
	void SetDuration(float _duration) { m_duration = _duration; }

private:

	//---- 内部の処理 ----

	//フェードを始める
	//逆向きのフェード中なら、今の見た目を保ったまま向きだけ変える
	// _next ... FadingIn か FadingOut
	void Start(FadeState _next)
	{
		if (IsFading())
		{
			// FadingIn: d = 1 - r²  /  FadingOut: d = r²
			float d = GetDissolve();
			float r = 0.0f;
			if (_next == FadeState::FadingOut)
			{
				r = std::sqrt(d);
			}
			else
			{
				r = std::sqrt(1.0f - d);
			}

			m_elapsed = r * m_duration;
			m_progress = r * r;
		}
		else
		{
			m_elapsed = 0.0f;
			m_progress = 0.0f;
		}

		m_state = _next;
	}

	//---- メンバ変数 ----

	//今の状態
	FadeState m_state = FadeState::Hidden;

	//フェードを始めてからの時間(秒)
	float m_elapsed = 0.0f;

	//フェードにかける時間(秒)
	float m_duration = 0.5f;

	//進み具合(イージング後 0 ～ 1)
	float m_progress = 0.0f;
};
