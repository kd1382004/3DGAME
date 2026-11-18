#pragma once

#include"../MapObjBase.h"

class Torch :public MapObjBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	Torch() {};

	//デストラクタ
	~Torch()override { Release(); };

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新後の処理
	void PostUpdate()override;

	//描画前の処理(視錐台カリングなど)
	void PreDraw();

	//ライトありの描画
	void DrawLit();

	//影の描画
	void GenerateDepthMapFromLight()override;

	//---- 設定 ----

	//座標をセット
	// _pos ... 座標
	void SetPos(const Math::Vector3& _pos)override;

	//回転をセット
	// _rMat ... 回転行列
	void SetRotation(Math::Matrix _rMat);

	//カメラをセット
	// _spCamera ... カメラ
	void SetCamera(std::shared_ptr<CameraBase>_spCamera) { m_wpCamera = _spCamera; }

	//チャンク内にいるかをセット(エフェクトを出すか切り替える)
	// _flg ... チャンク内にいるか
	void SetInChunk(bool _flg)override { SetEffectUpdate(_flg); };

private:

	//---- メンバ変数 ----

	//カメラ
	std::weak_ptr<CameraBase>m_wpCamera;

	//座標
	Math::Vector3 m_pos;

	//松明のモデル
	std::shared_ptr<KdModelWork>m_spTorchModel;

	//視錐台用のBoxInfo
	KdCollider::BoxInfo m_frustumBox;

	//カメラに映っているか
	bool m_isInView = false;

	//エフェクト
	std::weak_ptr<KdEffekseerObject> m_wpAuraEffect;

	//エフェクトを出す位置(ローカル)
	Math::Vector3 m_effectLocalPos;

	//光の色
	Math::Vector3 m_effectLocalColor;

	//光の半径
	float m_radius;

	//揺れの時間
	float m_flickerTime = 0.0f;
	float m_flickerAmount = 0.1f; // 揺れ幅（半径に対する割合）
	float m_flickerSpeed =0.3;   // 揺れの速さの倍率

	//音
	std::shared_ptr<KdSoundInstance3D> m_spFireSE;

	//---- 基本の処理 ----

	//エフェクトを消す
	void Release();

	//---- 設定 ----

	//炎のエフェクトを出すか切り替える
	// _flg ... 出すか
	void SetEffectUpdate(bool _flg);
};
