#pragma once

//デバッグ用の状態
class DebugInfo
{
public:

	//---- 取得 ----

	//SceneManagerのImGui編集モード中か
	//戻り値 ... 編集モード中ならtrue
	bool GetSceneManagerImGUIFlg() { return m_sceneManagerImGUIFlg; }

	//当たり判定(m_pDebugWire)を描画するか
	//戻り値 ... 描画するならtrue
	bool GetDrawDebugWireFlg() { return m_drawDebugWireFlg; }

	//---- 設定 ----

	//SceneManagerのImGui編集モードにするかをセット
	// _flg ... 編集モードにするならtrue
	void SetSceneManagerImGUIFlg(bool _flg) { m_sceneManagerImGUIFlg = _flg; }

	//当たり判定(m_pDebugWire)を描画するかをセット
	// _flg ... 描画するならtrue
	void SetDrawDebugWireFlg(bool _flg) { m_drawDebugWireFlg = _flg; }

	//ImGui のチェックボックス用(bool を直接書き換える)
	bool* WorkDrawDebugWireFlg() { return &m_drawDebugWireFlg; }

	//---- シングルトン ----

	//インスタンスを取得する(シングルトン)
	//戻り値 ... DebugInfoのインスタンス
	static DebugInfo& Instance() {
		static DebugInfo instance;
		return instance;
	}

private:

	//---- メンバ変数 ----

	//SceneManagerのImGui編集モード中か(trueの間はゲームの更新を止める)
	bool m_sceneManagerImGUIFlg = false;

	//当たり判定(m_pDebugWire)を描画するか(シーンが変わっても残る)
	bool m_drawDebugWireFlg = false;

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ(シングルトンなので外から作れない)
	DebugInfo() {};

	//デストラクタ
	~DebugInfo() {};
};
