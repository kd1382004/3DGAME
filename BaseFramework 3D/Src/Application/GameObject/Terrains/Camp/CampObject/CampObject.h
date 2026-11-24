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

	//---- 取得 ----

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

private:

	//---- メンバ変数 ----

	//置物のデータ(モデルのパス・位置・回転・大きさなど)
	CampObjectData m_data;

	//モデル
	std::shared_ptr<KdModelWork> m_spModel;

	//アニメーションを動かすもの
	std::shared_ptr<KdAnimator> m_spAnimator;
};