#pragma once

class DebugInfo
{
public:



	//SceneManagerのImGui編集モードにするかをセット
	// _flg ... 編集モードにするならtrue
	void SetSceneManagerImGUIFlg(bool _flg) { m_sceneManagerImGUIFlg = _flg; }
	//SceneManagerのImGui編集モード中か
	//戻り値 ... 編集モード中ならtrue
	bool GetSceneManagerImGUIFlg() { return m_sceneManagerImGUIFlg; }

private:
	//SceneManagerのImGui編集モード中か(trueの間はゲームの更新を止める)
	bool m_sceneManagerImGUIFlg = false;
private:
	//コンストラクタ(シングルトンなので外から作れない)
	DebugInfo() {};
	//デストラクタ
	~DebugInfo() {};

public:
	//インスタンスを取得する(シングルトン)
	//戻り値 ... DebugInfoのインスタンス
	static DebugInfo& Instance() {
		static DebugInfo instance;
		return instance;
	}
};
