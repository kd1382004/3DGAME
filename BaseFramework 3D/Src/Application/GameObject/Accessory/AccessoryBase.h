#pragma once

//アクセサリーの基底クラス
class AccessoryBase:public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	AccessoryBase() {};

	//デストラクタ
	~AccessoryBase()override {};

	//---- 基本の処理 ----

	//初期化
	virtual void Init() override;

	//更新
	virtual void Update() override;

	//更新後の処理(親の行列に合わせて位置を決める)
	virtual void PostUpdate() override;

	//影の描画
	virtual void GenerateDepthMapFromLight() override;

	//ライトありの描画
	virtual void DrawLit() override;

	//光る部分の描画
	virtual void DrawBright() override;

	//---- 設定 ----

	//親(付けている部位)の行列をセット
	// _mat ... 親の行列
	void SetParentMatrix(Math::Matrix _mat) { m_accessoryParentMat = _mat; }

	//親の回転をセット
	// _rot ... 親の回転(度)
	void SetParentRotation(Math::Vector3 _rot) { m_accessoryParentRot = _rot; }

	//持ち主(プレイヤー)の行列をセット
	// _mat ... 持ち主の行列
	void SetOwnerMatrix(const Math::Matrix& _mat) { m_ownerMat = _mat; }

protected:

	//---- メンバ変数 ----

	//ローカル
	Math::Matrix m_localMat = Math::Matrix::Identity;

	//親からずらす位置
	Math::Vector3 m_localPos = Math::Vector3::Zero;

	//親
	Math::Matrix m_accessoryParentMat = Math::Matrix::Identity;

	//親の回転(度)
	Math::Vector3 m_accessoryParentRot = Math::Vector3::Zero;

	//持ち主(プレイヤー)の行列
	Math::Matrix m_ownerMat = Math::Matrix::Identity;
};
