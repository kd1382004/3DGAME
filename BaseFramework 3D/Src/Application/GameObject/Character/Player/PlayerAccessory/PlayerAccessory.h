#pragma once

class Flashlight;
class PlayerBase;

class PlayerAccessory
{
public:
	PlayerAccessory() {};
	~PlayerAccessory() {};

	void Init();
	void Update(PlayerBase*_player);


	void SetFlashlight(std::shared_ptr<Flashlight> _spFlashlight) { m_spFlashlight = _spFlashlight; }

private:

	std::weak_ptr<Flashlight>m_spFlashlight;

};
