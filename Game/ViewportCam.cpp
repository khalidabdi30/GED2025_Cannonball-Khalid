#include "pch.h"
#include "ViewportCam.h"
#include "stringHelp.h"
#include "Game.h"
#include "DrawData.h"

ViewportCam::ViewportCam()
{
	m_type = "VIEWPORT";
	m_canBeMainCamera = false; //Can't be true or else it'll break
}

void ViewportCam::PreRender(DrawData* _DD)
{
	SetVP(_DD->m_width, _DD->m_height);
	m_aspectRatio = (float)_DD->m_width / (float)_DD->m_height;
	_DD->m_pd3dImmediateContext->RSSetViewports(1, &m_viewport);
}

void ViewportCam::Tick(GameState* _GS)
{
	Camera::Tick(_GS);
}

void ViewportCam::Load(ifstream& _file)
{
	Camera::Load(_file);
	StringHelp::Float(_file, "X", m_X);
	StringHelp::Float(_file, "Y", m_Y);
	StringHelp::Float(_file, "WIDTH", m_width);
	StringHelp::Float(_file, "HEIGHT", m_height);
}

void ViewportCam::Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game)
{
	SetVP(_game->GetWidth(), _game->GetHeight());
	m_aspectRatio = (float)_game->GetWidth() / (float)_game->GetHeight();
}

void ViewportCam::SetVP(int _w, int _h)
{
	m_viewport = { (float)_w * m_X, (float)_h * m_Y, (float)_w * m_width, (float)_h * m_height, 0.f, 1.f };
}
