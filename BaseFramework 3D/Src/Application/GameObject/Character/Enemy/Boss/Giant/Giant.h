#pragma once
#include"../../EnemyBase.h"

class Giant:public EnemyBase
{
public:
	Giant() {};
	~Giant() {};

	void Init()override;
	void PreUpdate()override;
	void Update()override;
	void PostUpdate()override;

private:

};
