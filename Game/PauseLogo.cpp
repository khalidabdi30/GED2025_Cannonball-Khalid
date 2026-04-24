#include "pch.h"
#include "PauseLogo.h"
#include "GameState.h"

void PauseLogo::Tick(GameState* _GS)
{
	m_paused = (_GS->m_FSM == FSM_PAUSE);
}

void PauseLogo::Draw(DrawData2D* _DD2D)
{
	if (m_paused)
	{
		ImageGO2D::Draw(_DD2D);
	}
}
