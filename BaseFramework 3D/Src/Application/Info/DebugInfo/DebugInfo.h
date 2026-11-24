#pragma once

//デバッグ用の状態
class DebugInfo
{
public:

	//---- 取得 ----

	//SceneManagerのImGui編集モード中か
	//戻り値 ... 編集モード中ならtrue
	bool GetSceneManagerImGUIFlg() { return m_sceneManagerImGUIFlg; }

	//---- 設定 ----

	//SceneManagerのImGui編集モードにするかをセット
	// _flg ... 編集モードにするならtrue
	void SetSceneManagerImGUIFlg(bool _flg) { m_sceneManagerImGUIFlg = _flg; }

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

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ(シングルトンなので外から作れない)
	DebugInfo() {};

	//デストラクタ
	~DebugInfo() {};
};
