#include "pch.h"
#include "TestSound.h"
#include "GameState.h"


TestSound::TestSound()
{
	m_type = "TEST";
}

TestSound::~TestSound()
{
}

void TestSound::Tick(GameState* _GS)
{
	if (_GS->m_KBS_tracker.pressed.Q)
	{
		Play();
	}
}
