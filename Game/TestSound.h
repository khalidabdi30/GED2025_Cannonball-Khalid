#pragma once
#include "Sound.h"

//An example sound which goes off when you press a key.

class TestSound :
	public Sound
{
public:
	TestSound();
	~TestSound();


	virtual void Tick(GameState* _GS) override;
};

