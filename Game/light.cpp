#include "pch.h"
#include "light.h"
#include "GameState.h"
#include "stringHelp.h"

Light::Light()
{
	m_type = "LIGHT";
}

Light::~Light()
{
}

void Light::Tick(GameState* _GS)
{
	//not really needed but spins the light around to show off the lambert default VBGO shader
	static float time = 0.0f;
	time += _GS->m_dt;
	m_pos.x = 100.0f * cosf(time);
	m_pos.z = 100.0f * sinf(time);
	if (time > XM_2PI) time -= XM_2PI;
}

void Light::Load(ifstream& _file)
{
	StringHelp::String(_file, "NAME", m_name);
	StringHelp::Float3(_file, "POS", m_pos.x, m_pos.y, m_pos.z);
	StringHelp::Float4(_file, "COL", m_colour.x, m_colour.y, m_colour.z, m_colour.w);
	StringHelp::Float4(_file, "AMBCOL", m_ambientColour.x, m_ambientColour.y, m_ambientColour.z, m_ambientColour.w);
}
