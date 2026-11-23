#pragma once
#include"../UIBase.h"

class PlayerBase;
class PotionUseController;
class PotionTexInfo;
class PlayerInventoryUI;

//画面下に出す、ポーションを選んで使うバー(モンハンのアイテムバー風)
class ItemBar : public UIBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	ItemBar() {};

	//デストラクタ
	~ItemBar()override {};

	//---- 基本の処理 ----

	//初期化
	void Init() override;

	//更新(一覧を作る・入力・使用・演出)
	void Update() override;

	//2Dの描画
	void DrawSprite() override;

	//---- 設定 ----

	//プレイヤーをセット
	// _spPlayer ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayer) { m_wpPlayer = _spPlayer; }

	//ポーションを使う指示を出すクラスをセット
	// _spController ... ポーションを使う指示を出すクラス
	void SetPotionUseController(std::shared_ptr<PotionUseController> _spController) { m_wpPotionUseController = _spController; }

	//ポーションの画像一覧をセット
	// _spTexInfo ... ポーションの画像一覧
	void SetPotionTexInfo(std::shared_ptr<PotionTexInfo> _spTexInfo) { m_wpPotionTexInfo = _spTexInfo; }

	//インベントリのUIをセット(開いている間はバーを止めるため)
	// _spUI ... インベントリのUI
	void SetPlayerInventoryUI(std::shared_ptr<PlayerInventoryUI> _spUI) { m_wpPlayerInventoryUI = _spUI; }

private:

	//---- 定数 ----

	//左右に出す数
	static constexpr int kSideNum = 2;

	//使うキー
	static constexpr int kUseKey = 'F';

	//使ったときにはねる時間(秒)
	static constexpr float kPopTime = 0.25f;

	//---- メンバ変数 ----

	//プレイヤー
	std::weak_ptr<PlayerBase>          m_wpPlayer;

	//ポーションを使う指示を出すクラス
	std::weak_ptr<PotionUseController> m_wpPotionUseController;

	//ポーションの画像一覧
	std::weak_ptr<PotionTexInfo>       m_wpPotionTexInfo;

	//インベントリのUI
	std::weak_ptr<PlayerInventoryUI>   m_wpPlayerInventoryUI;

	//バーに並べるもの(個数1以上のポーションID)
	std::vector<int> m_itemIDList;

	//選択中のポーションID(一覧が変わっても同じものを選び続けるためIDで持つ)
	int m_selectID = -1;

	//切り替え直後のずれ(±1 → 0 に戻る)
	float m_slideOffset = 0.0f;

	//使ったときのはねる演出の残り時間
	float m_popTimer = 0.0f;

	//アイコン同士の間隔
	const float m_slotSpacing = 36.0f;

	//中央のアイコンの大きさ
	const float m_selectSize = 32.0f;

	//左右のアイコンの大きさ
	const float m_slotSize = 20.0f;

	//バー全体の帯の画像
	std::shared_ptr<KdTexture> m_spBackTex;

	//左右の小さい枠の画像
	std::shared_ptr<KdTexture> m_spSlotTex;

	//中央の選択枠の画像
	std::shared_ptr<KdTexture> m_spSelectTex;

	//---- その他の処理 ----

	//バーに並べるものの一覧を作る(毎フレーム)
	void MakeItemList();

	//ホイールで選ぶ
	void SelectUpdate();

	//キーで使う
	void UseUpdate();

	//演出の更新
	void EffectUpdate();

	//入力を受け付けない状態か(インベントリを開いている・倒れている)
	//戻り値 ... 受け付けないならtrue
	bool IsInputBlocked();

	//選択中のものが一覧の何番目か
	//戻り値 ... 何番目か(見つからなければ-1)
	int  GetSelectIndex();

	//アイコン1つ分を描く
	// _ID ... ポーションの番号
	// _x ... 画面上の横位置
	// _size ... アイコンの大きさ
	// _alpha ... 不透明度
	// _select ... 中央(選択中)か。中央は小さい枠を描かない
	void DrawSlot(int _ID, float _x, float _size, float _alpha, bool _select);

	//画像を読み込む(ファイルがなければnullptr)
	// _path ... 画像のパス
	//戻り値 ... 読み込んだ画像
	std::shared_ptr<KdTexture> LoadTex(const std::string& _path);
};
