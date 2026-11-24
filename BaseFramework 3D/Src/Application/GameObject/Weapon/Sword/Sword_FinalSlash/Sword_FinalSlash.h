#pragma once

class CharacterBase;
class CameraBase;

//////////////////////////////////////////////////////////////
// 終ノ一閃（残し斬り）
// 最後の段の斬撃の跡がその場に残り、少し遅れて弾けて敵を斬る
//////////////////////////////////////////////////////////////
class Sword_FinalSlash :public KdGameObject
{
public:
	//コンストラクタ
	Sword_FinalSlash() {}
	//デストラクタ
	~Sword_FinalSlash()override
	{
		// 溜めの音が鳴りっぱなしにならないように止める
		if (m_spChargeSE) { m_spChargeSE->Stop(); }
	}

	//初期化
	void Init()override;
	//更新
	void Update()override;
	//エフェクトの描画
	void DrawEffect()override;
	//光る部分の描画
	void DrawBright()override;

	// 出すときに Sword から呼ぶ
	// _obb    : 斬撃の跡の判定（ワールド)
	// _damage : 弾けたときのダメージ
	// _dir    : ノックバックの向き
	// _knockback ... ノックバックの強さ
	// _defenseIgnore ... 防御無視の割合
	// _chargeTime ... 溜めた時間
	void SetSlashStatus(const DirectX::BoundingOrientedBox& _obb, float _damage,
		float _knockback, const Math::Vector3& _dir, float _defenseIgnore, float _chargeTime);

	// 弾けたときに揺らすカメラ
	// _spCamera ... カメラ
	void SetCamera(const std::shared_ptr<CameraBase>& _spCamera) { m_wpCamera = _spCamera; }

	// 当たり判定をする敵のリスト（Sword が持っているものを渡す）
	// _list ... 当たり判定をするキャラのリスト
	void SetAttackHitCharacterList(const std::list<std::weak_ptr<CharacterBase>>& _list)
	{
		m_attackHitCharacterList = _list;
	}

private:
	// 弾けたときに敵へダメージを入れる（1回だけ）
	void Burst();

	//////////////////////////////////////////////////////////////
	// 段階と時間
	//////////////////////////////////////////////////////////////

	// 今どの段階か
	enum class State
	{
		Charge,		// 溜め：線が残っている
		Burst,		// 弾け：このフレームで判定
		Fade,		// 消える
	};
	//今の段階
	State m_state = State::Charge;

	// 弾けたときに揺らすカメラ
	std::weak_ptr<CameraBase> m_wpCamera;

	// 効果音
	std::shared_ptr<KdSoundInstance> m_spChargeSE;	// 溜めている間のバチバチ（弾けたら止める）

	// 今の段階に入ってからの経過時間
	float m_timer = 0.0f;

	// 溜めの時間（Lv が上がると短くなる。Sword から受け取る）
	float m_chargeTime = 0.3f;

	// 溜めの進み具合（0〜1）。Update で計算して DrawEffect で使う
	float m_chargeRate = 0.0f;

	// 弾けてから消えるまでの時間
	const float m_fadeTime = 0.4f;

	// 消える進み具合（0〜1）。Update で計算して DrawEffect で使う
	float m_fadeRate = 0.0f;

	// Fade に入ってから「弾けた瞬間」として一番派手に見せる時間（秒）
	const float m_burstFlashTime = 0.05f;

	//////////////////////////////////////////////////////////////
	// 攻撃（判定とダメージ）
	//////////////////////////////////////////////////////////////

	// 斬撃の跡の判定（ワールド）
	DirectX::BoundingOrientedBox m_obb;

	// 弾けたときのダメージ
	float m_damage = 0.0f;

	// ノックバックの強さ
	float m_knockback = 0.0f;

	// ノックバックの向き（剣を振った向き）
	Math::Vector3 m_dir = Math::Vector3::Forward;

	// 防御無視の割合（0〜1）
	float m_defenseIgnore = 0.0f;

	// 当たり判定をする敵のリスト（Sword から受け取る）
	std::list<std::weak_ptr<CharacterBase>> m_attackHitCharacterList;

	//////////////////////////////////////////////////////////////
	// 演出：板と画像
	//////////////////////////////////////////////////////////////

	// 板ポリゴン（1枚を使い回して、描くたびに画像を差し替える）
	std::shared_ptr<KdSquarePolygon> m_spPoly = nullptr;

	// 斬撃の線（白。描くときに赤を掛ける、加算）
	std::shared_ptr<KdTexture> m_spLineTex = nullptr;

	// 線のまわりのぼんやりした光（白。描くときに赤を掛ける、加算）
	std::shared_ptr<KdTexture> m_spGlowTex = nullptr;

	// 弾けたときに後ろを暗くする黒いもや（Alpha）
	std::shared_ptr<KdTexture> m_spDarkHaloTex = nullptr;

	// 黒い稲妻（形違いを3枚。切り替えてバチバチさせる、Alpha）
	static constexpr int kBoltTexNum = 3;
	//黒い稲妻の画像
	std::shared_ptr<KdTexture> m_spBoltTex[kBoltTexNum];

	// 衝撃波の黒い輪（Alpha）
	std::shared_ptr<KdTexture> m_spRingTex = nullptr;

	// 衝撃波の輪のぼんやりした光（白。赤を掛けて加算）
	std::shared_ptr<KdTexture> m_spRingGlowTex = nullptr;

	//////////////////////////////////////////////////////////////
	// 演出：色
	//////////////////////////////////////////////////////////////

	// 溜めの間の線の色（暗い赤）
	const Math::Color m_chargeColor = { 0.6f, 0.05f, 0.03f, 1.0f };

	// 弾けた瞬間の線と光の色（明るい赤）
	const Math::Color m_burstColor = { 1.0f, 0.15f, 0.08f, 1.0f };

	// 衝撃波の輪の光の色（明るい赤）
	const Math::Color m_ringGlowColor = { 1.0f, 0.12f, 0.06f, 1.0f };

	//////////////////////////////////////////////////////////////
	// 演出：大きさ
	//////////////////////////////////////////////////////////////

	// 弾けた瞬間に線を太くする倍率（溜めの間は 1.0）
	const float m_burstWidthRate = 1.6f;

	// 消える間に線を長さ方向へ伸ばす倍率（最後にこの倍率になる）
	const float m_fadeLengthRate = 1.2f;

	// 黒いもやの大きさの倍率（箱の大きさに対して）
	const float m_darkHaloScale = 2.0f;

	// 黒いもやの横幅を、縦より広げる倍率（丸く広げるため）
	const float m_darkHaloWidthRate = 3.0f;

	// 弾けたときの稲妻の長さの倍率（刃より外まで飛び出させる）
	const float m_burstBoltLengthRate = 1.8f;

	// 弾けたときの稲妻の横幅の倍率
	const float m_burstBoltWidthRate = 4.0f;

	//////////////////////////////////////////////////////////////
	// 演出：稲妻（放射状に何本も）
	//////////////////////////////////////////////////////////////

	// 同時に出す稲妻の本数
	static constexpr int kBoltNum = 5;

	// 稲妻ごとの、放射状に広げる基本の角度（度。板の面の中で回す）
	// 0 が刃の向き。左右に広げて「斬撃から弾け飛ぶ」形にする
	const float m_boltBaseDeg[kBoltNum] = { -40.0f, -15.0f, 0.0f, 15.0f, 40.0f };

	// 稲妻ごとの、今表示している絵の番号（0 〜 kBoltTexNum-1）
	int m_boltIndices[kBoltNum] = {};

	// 稲妻ごとの、基本の角度からのずれ（度。切り替えるたびにランダム）
	float m_boltRollDegs[kBoltNum] = {};

	// 稲妻ごとの、左右反転するか（切り替えるたびにランダム）
	bool m_boltFlipFlgs[kBoltNum] = {};

	// 溜めのチラ見せで出す稲妻の本数（予告なので少なめ）
	const int m_boltPeekNum = 1;

	//---------- 切り替え（バチバチ） ----------
	// 次の稲妻に切り替えるまでの経過時間
	float m_boltFlickerTimer = 0.0f;

	// 稲妻を切り替える間隔（秒）。消える段階では長くして、ゆっくりにする
	float m_boltFlickerInterval = 0.04f;

	// 稲妻を切り替える間隔の、弾けた直後の値（秒）… 速くバチバチ
	const float m_boltFlickerIntervalStart = 0.04f;

	// 稲妻を切り替える間隔の、消える直前の値（秒）… ゆっくり
	const float m_boltFlickerIntervalEnd = 0.12f;

	//---------- 溜めのチラ見せ ----------
	// 溜めの間に、稲妻をチラッと見せているか（予告）
	// 溜めの後半だけ、稲妻を切り替えるたびにランダムで決める
	bool m_boltPeekFlg = false;

	// 稲妻のチラ見せを始める、溜めの進み具合（0〜1）
	const float m_boltPeekStartRate = 0.7f;

	//---------- 1本だけのときの値（配列に置き換えたら消す） ----------
	// 今表示している稲妻の番号（0 〜 kBoltTexNum-1）
	int m_boltIndex = 0;

	// 稲妻を左右反転するか（切り替えるたびにランダムで決める）
	bool m_boltFlipFlg = false;

	// 稲妻を少し傾ける角度（度。切り替えるたびにランダムで決める）
	float m_boltRollDeg = 0.0f;

	//////////////////////////////////////////////////////////////
	// 演出：衝撃波リング
	//////////////////////////////////////////////////////////////

	// 輪が広がりきるまでの時間（秒）。Fade に入ってからの時間で数える
	const float m_ringTime = 0.25f;

	// 輪の最初の大きさ（m）
	const float m_ringStartSize = 0.3f;

	// 輪の最後の大きさ（m）
	const float m_ringEndSize = 4.0f;

	//稲妻の向きと形をランダムに決め直す
	void RandomizeBolts();



	//---- 型 ----

	//描くのに使う値(段階ごとに CalcDrawParam で入れる)
	struct DrawParam
	{
		//線の横幅の倍率
		float m_lineWidthRate = 1.0f;

		//線の長さの倍率
		float m_lineLengthRate = 1.0f;

		//線の色と透明度
		Math::Color m_lineColor = { 1, 1, 1, 0 };

		//光の横幅の倍率
		float m_glowWidthRate = 1.0f;

		//光の色と透明度
		Math::Color m_glowColor = { 1, 1, 1, 0 };

		//稲妻を描くか
		bool m_drawBolt = false;

		//稲妻の色と透明度
		Math::Color m_boltColor = { 1, 1, 1, 0 };

		//描く稲妻の本数
		int m_boltNum = 0;

		//稲妻の横幅の倍率
		float m_boltWidthRate = 3.0f;

		//稲妻の長さの倍率
		float m_boltLengthRate = 1.0f;

		//黒いもやを描くか
		bool m_drawHalo = false;

		//黒いもやの大きさの倍率(縦・横とも)
		float m_haloScale = 1.0f;

		//黒いもやの色と透明度
		Math::Color m_haloColor = { 1, 1, 1, 0 };

		//衝撃波リングを描くか(Fade の最初の m_ringTime 秒だけ)
		bool m_drawRing = false;

		//衝撃波リングの大きさ(m。直径)
		float m_ringSize = 0.0f;

		//衝撃波リングの行列
		Math::Matrix m_ringMat = Math::Matrix::Identity;

		//衝撃波リングの光の色と透明度(加算)
		Math::Color m_ringGlowColor = { 1, 1, 1, 0 };

		//衝撃波リングの黒い輪の色と透明度(Alpha)
		Math::Color m_ringColor = { 1, 1, 1, 0 };
	};

	//---- その他の処理 ----

	//今の段階の描く値を計算する
	//戻り値 ... 描く値
	DrawParam CalcDrawParam() const;

	//板を置く行列を2枚分(十字)作る
	// _widthRate  ... 横幅の倍率
	// _lengthRate ... 長さの倍率
	// _rollDeg    ... 板を傾ける角度
	// _flip       ... 左右反転するか
	//戻り値 ... [0] = 1枚目、[1] = 2枚目
	std::array<Math::Matrix, 2> MakeCrossMats(float _widthRate, float _lengthRate = 1.0f, float _rollDeg = 0.0f, bool _flip = false) const;

	//ブルームの強さ(ブルームに描く色の倍率)
	float m_bloomRate = 1.0f;
};