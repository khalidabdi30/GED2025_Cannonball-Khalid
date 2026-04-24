#pragma once

//=================================================================
//Data to be passed by game to all 2D Game Objects via Draw and related functions
//=================================================================
#include "SpriteBatch.h"
#include "SpriteFont.h"

using namespace DirectX;
using namespace std;

struct DrawData2D
{
	//spritebatch stuff
	unique_ptr<SpriteBatch>	m_Sprites;
	unique_ptr<SpriteFont> m_Font;
};