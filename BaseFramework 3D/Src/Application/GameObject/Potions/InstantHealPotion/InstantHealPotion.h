#pragma once
class PlayerBase;

class InstantHealPotion
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	InstantHealPotion() {};

	//デストラクタ
	~InstantHealPotion() {};

	//---- 設定 ----

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

	//---- その他の処理 ----

	//HP即回復
	void UseInstantHeal();

private:

	//---- メンバ変数 ----

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;
};
