#include "pch.h"
#include "RTImage.h"
#include "RenderTarget.h"
#include "Game.h"
#include "DrawData2D.h"

RTImage::RTImage()
{
	m_type = "RENDERTARGET";
}

RTImage::~RTImage()
{
}

void RTImage::Init(ID3D11Device* _GD, Game* _game)
{
	m_rt = (RenderTarget*)_game->GetCamByName(m_name);
}

void RTImage::Draw(DrawData2D* _DD2D)
{
	if (!m_rt)
	{
		return;
	}

	//nullptr can be changed to a RECT* to define what area of this image to grab
	//you can also add an extra value at the end to define layer depth
	_DD2D->m_Sprites->Draw(m_rt->GetShaderResourceView(), m_pos, nullptr, m_colour, m_rotation, m_origin, m_scale, SpriteEffects_None);
}
