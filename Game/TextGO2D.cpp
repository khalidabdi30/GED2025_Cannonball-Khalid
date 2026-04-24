#include "pch.h"
#include "TextGO2D.h"
#include "DrawData2D.h"
#include "helper.h"
#include "stringHelp.h"

TextGO2D::TextGO2D()
{
	m_type = "TEXT";
}


void TextGO2D::Tick(GameState* _GS)
{
	GameObject2D::Tick(_GS);
}


void TextGO2D::Draw(DrawData2D* _DD)
{
	_DD->m_Font->DrawString(_DD->m_Sprites.get(), Helper::charToWChar(m_text.c_str()), m_pos, m_colour, m_rotation, m_origin, m_scale);
}

void TextGO2D::Init(ID3D11Device* _GD, Game* _game)
{

}

void TextGO2D::Load(ifstream& _file)
{
	StringHelp::String(_file, "NAME", m_name);
	string dummy;
	_file >> dummy; //ignore the label
	getline(_file, m_text); //get the rest of the line
	m_text = m_text.substr(1); //ignore the leading space
	cout << "TEXT : " << m_text << endl;;
	StringHelp::Float2(_file, "POS", m_pos.x, m_pos.y);
	StringHelp::Float2(_file, "SCALE", m_scale.x, m_scale.y);
	StringHelp::Float(_file, "ROT", m_rotation);
	StringHelp::Float3(_file, "COL", m_colour.x, m_colour.y, m_colour.z);
}