#pragma once

#include"../CampLayoutData/CampLayoutData.h"

//準備シーン(キャンプ)に置く置物
class CampObject : public KdGameObject
{
public:

	//---- 基本の処理 ----

	//初期化
	void Init() override;

	//更新(アニメーションを進める)
	void Update() override;

	//ライトありの描画
	void DrawLit() override;

	//影の描画
	void GenerateDepthMapFromLight() override;

	//---- 設定 ----

	// データを入れて、モデルと行列を作り直す
	// _data ... 置物のデータ
	void SetData(const CampObjectData& _data);

	//描くかをセット(武器立ての武器など、条件で隠したい物に使う)
	// _flg ... 描くならtrue
	void SetDrawFlg(bool _flg) { m_drawFlg = _flg; }

	//シーンから消す(エディタで置物を消したときに使う。次の更新で一覧から外れる)
	void Remove() { m_isExpired = true; }

	//エディタで選んでいる印(色を変えて描く)をセット
	// _flg ... 選んでいるならtrue
	void SetHighlight(bool _flg) { m_highlightFlg = _flg; }

	//---- 取得 ----

	//描いているか(武器立ての隠れている武器はfalse)
	//戻り値 ... 描いているならtrue
	bool GetDrawFlg() const { return m_drawFlg; }

	// エディタから値を変えるときに使う
	//戻り値 ... 置物のデータ(書き換え可)
	CampObjectData& WorkData() { return m_data; }

	//---- その他の処理 ----

	//今のデータから行列を作り直す
	void ApplyData() { m_mWorld = m_data.MakeWorld(); }

	//アニメーションを再生する
	// _name ... アニメーションの名前
	// _loop ... ループさせるならtrue
	void PlayAnime(const std::string& _name, bool _loop);


	void PlayAnimeOnce(const std::string& _name, const std::string& _next)
	{
		PlayAnime(_name, false);
		m_nextAnime = _next;
	}

private:

	//---- メンバ変数 ----

	//置物のデータ(モデルのパス・位置・回転・大きさなど)
	CampObjectData m_data;

	//モデル
	std::shared_ptr<KdModelWork> m_spModel;

	//アニメーションを動かすもの
	std::shared_ptr<KdAnimator> m_spAnimator;

	std::string m_nextAnime;	//今のアニメが終わったら流すアニメ(空なら何もしない)

	//描くか(falseなら影も描かない)
	bool m_drawFlg = true;

	//エディタで選んでいるか(選んでいる間は色を変えて描く)
	bool m_highlightFlg = false;
};