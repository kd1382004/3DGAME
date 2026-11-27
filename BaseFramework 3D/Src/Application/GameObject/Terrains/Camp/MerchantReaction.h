#pragma once

//商人の反応(ショップで何が起きたか)
//ショップの画面(ShopDisplay)から準備シーン → キャンプの管理(CampManager)へ渡す
//CampManager を知らなくても使えるように、これだけ別のヘッダーにしている
enum class MerchantReaction
{
	Buy,		//プレイヤーが買った(品物を差し出してお辞儀)
	Sell,		//プレイヤーが売った(受け取って品定めして支払い)
	NoMoney,	//お金が足りなくて買えなかった(首を振って手を左右に振る)
};
