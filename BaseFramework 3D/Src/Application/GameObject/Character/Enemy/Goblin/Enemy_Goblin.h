#pragma once

#include"../EnemyAmbush/EnemyAmbush.h"

class Goblin :public EnemyAmbush
{
public:
	Goblin() {};
	~Goblin()override {};


	void Init()override;
	void Update()override;

private:

};
