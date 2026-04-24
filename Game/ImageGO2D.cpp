#include "pch.h"
#include "ImageGO2D.h"
#include "DDSTextureLoader.h"
#include "DrawData2D.h"
#include "GameState.h"
#include "helper.h"
#include "stringHelp.h"

ImageGO2D::ImageGO2D() :m_pTextureRV(nullptr)
{
	m_type = "IMAGE";
}

ImageGO2D::~ImageGO2D()
{
	if (m_pTextureRV)
	{
		m_pTextureRV->Release();
		m_pTextureRV = nullptr;
	}
}

void ImageGO2D::Tick(GameState* _GS)
{
	GameObject2D::Tick(_GS);
}



void ImageGO2D::Draw(DrawData2D* _DD)
{
	//nullptr can be changed to a RECT* to define what area of this image to grab
	// i.e. for sprite sheets or similar
	//you can also add an extra value at the end to define layer depth
	_DD->m_Sprites->Draw(m_pTextureRV, m_pos, nullptr, m_colour, m_rotation, m_origin, m_scale, SpriteEffects_None);

	//see here for more details:
	//https://github.com/microsoft/DirectXTK/wiki/SpriteBatch
	//https://github.com/microsoft/DirectXTK/wiki/Sprites-and-textures
	//https://github.com/microsoft/DirectXTK/wiki/More-tricks-with-sprites
}

void ImageGO2D::Load(ifstream& _file)
{
	StringHelp::String(_file, "NAME", m_name);
	StringHelp::String(_file, "FILE", m_file);
	StringHelp::Float2(_file, "POS", m_pos.x, m_pos.y);
	StringHelp::Float2(_file, "SCALE", m_scale.x, m_scale.y);
	StringHelp::Float(_file, "ROT", m_rotation);
}

void ImageGO2D::Init(ID3D11Device* _GD, Game* _game)
{
	string fullfilename = "../Assets/" + m_file + ".dds";

	HRESULT hr = CreateDDSTextureFromFile(_GD, Helper::charToWChar(fullfilename.c_str()),
		nullptr, &m_pTextureRV);

	//this nasty thing is required to find out the size of this image!
	ID3D11Resource* pResource;
	D3D11_TEXTURE2D_DESC Desc;
	m_pTextureRV->GetResource(&pResource);
	((ID3D11Texture2D*)pResource)->GetDesc(&Desc);

	m_origin = 0.5f * Vector2((float)Desc.Width, (float)Desc.Height);//around which rotation and scaling is done
}
