#pragma once
#include "ImageGO2D.h"

//a pause logo that appears when the game is paused

class PauseLogo : public ImageGO2D
{
public:

	virtual void Tick(GameState* _GS) override;
	virtual void Draw(DrawData2D* _DD2D) override;

protected:

	bool m_paused = false;
};

