#pragma once
#include "camera.h"

//a camera that renders to a specified viewport not to the full back buffer

class ViewportCam : public Camera
{
public:
	ViewportCam();
	~ViewportCam() {};

	virtual void PreRender(DrawData* _DD) override;

	virtual void Tick(GameState* _GS) override;

	virtual void Load(ifstream& _file) override;
	virtual void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game) override;

protected:

	float m_X, m_Y;//locations of the top left corner of the viewport as fractions of the screen
	float m_width, m_height;//size of the viewport as fractions of the screen

	void SetVP(int _w, int _h);

};

