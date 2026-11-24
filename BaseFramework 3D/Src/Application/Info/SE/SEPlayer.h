#pragma once
//---- 便利機能 ----
#include"../VOL/VOLInfo.h"		//音量設定

//効果音を鳴らすための小さな便利関数
//(Play → SetVolume の2行を毎回書かなくていいようにする)
namespace SEPlayer
{
	//2D(画面全体)で鳴らす
	// _path ... wav ファイルのパス
	// _rate ... 音量の倍率(設定の SE 音量 × この値)
	// _loop ... ループするなら true
	inline std::shared_ptr<KdSoundInstance> Play2D(std::string_view _path, float _rate = 1.0f, bool _loop = false)
	{
		std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play(_path, _loop);
		if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * _rate); }
		return spSE;
	}

	//3D(その場所から聞こえる)で鳴らす
	// _path ... wav ファイルのパス
	// _pos  ... 鳴らす位置
	// _rate ... 音量の倍率(設定の SE 音量 × この値)
	// _loop ... ループするなら true
	inline std::shared_ptr<KdSoundInstance3D> Play3D(std::string_view _path, const Math::Vector3& _pos, float _rate = 1.0f, bool _loop = false)
	{
		std::shared_ptr<KdSoundInstance3D> spSE = KdAudioManager::Instance().Play3D(_path, _pos, _loop);
		if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * _rate); }
		return spSE;
	}
}
