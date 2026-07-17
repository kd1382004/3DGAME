#pragma once

class CharacterBase : public KdGameObject
{
public:
	CharacterBase() {};
	~CharacterBase()	override { Release(); };

	void Init()			override;
	void Update()		override;
	void PostUpdate()	override;

	void GenerateDepthMapFromLight()override;
	void DrawLit()		override;

	void ImGUI()override;

private:
	// 衝突判定とそれに伴う座標の更新
	void CollisionUpdate();

	// 解放処理
	void Release();



protected:
	std::shared_ptr<KdModelWork> m_spCharaModel = nullptr;


	
	
	float m_Gravity = 0;

	float m_gravityPower = 0.01f;

	///////////////////////////////////////////
	//キャラステータス

	//キャラステータスが入ってるファイル名
	std::string m_charaStatusFilePath;

	struct StatusHP
	{
		//基礎体力
		float baseHP = 0;

		//最大体力
		float maxHP = 0;

		//現在体力
		float nowHP = 0;
	};

	struct StatusAttck
	{
		//基礎攻撃力
		float baseAttckPowe = 0;

		//現在攻撃力
		float nowAttck = 0;
	};

	struct StatusDefense
	{
		//基礎防御力
		float baseDefensePowe = 0;

		//現在防御力
		float nowDefense = 0;
	};

	struct StatusMoveSpeed
	{
		//基礎速度
		float baseSpeed = 0;

		//歩き速度(基礎速度に足す値)
		float walkMovePowe = 0;

		//ダッシュ速度(基礎速度に足す値)
		float runMovePowe = 0;

		//現在の速度
		float nowSpeed = 0;
	};

	struct Status
	{
		//体力
		StatusHP HP;

		// 攻撃力
		StatusAttck attck;

		//防御力
		StatusDefense defense;

		//スピード
		StatusMoveSpeed moveSpeed;
	};

	//ステータス
	Status m_status;

	//_filePath ... ファイルパスを書く
	void LoadCharaStatus(std::string _filePath);
	void SaveCharaStatus(std::string _filePath);
	///////////////////////////////////////////

	///////////////////////////////////////////
	
	//座標
	Math::Vector3 m_pos;

	//サイズ
	float m_siz;

	//移動方向
	Math::Vector3 m_moveVec;

	///////////////////////////////////////////
	
	///////////////////////////////////////////
	//キャラの回転
	//角度
	float m_angle;

	void AngeleUpdate();

	///////////////////////////////////////////

	///////////////////////////////////////////
	//ステータスエディターに表示する
	virtual void StatusEditor();

	bool m_statusEditorFlf = false;

	std::string m_statusEditorName = "キャラクター";

	///////////////////////////////////////////
};
