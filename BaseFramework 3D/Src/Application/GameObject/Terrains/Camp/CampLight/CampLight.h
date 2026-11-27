#pragma once
#include"../CampLayoutData/CampLayoutData.h"

//準備シーン(キャンプ)に置くポイントライト
class CampLight : public KdGameObject
{
public:

	//---- 基本の処理 ----

	//更新後の処理(ポイントライトを登録する)
	void PostUpdate() override
	{
		// 色 × 明るさ を渡す(松明は {24,10,2} のように1を超える値を使っている)
		KdShaderManager::Instance().WorkAmbientController().AddPointLight(
			m_data.m_color * m_data.m_intensity,
			m_data.m_radius,
			m_data.m_pos);
	}

	//---- 設定 ----

	//ライトのデータをセット
	// _data ... ライトのデータ
	void SetData(const CampLightData& _data) { m_data = _data; }

	//シーンから消す(エディタでライトを消したときに使う。次の更新で一覧から外れる)
	void Remove() { m_isExpired = true; }

	//---- 取得 ----

	//ライトのデータを取得(エディタから値を変えるときに使う)
	//戻り値 ... ライトのデータ(書き換え可)
	CampLightData& WorkData() { return m_data; }

private:

	//---- メンバ変数 ----

	//ライトのデータ(色・明るさ・範囲・位置)
	CampLightData m_data;
};