#pragma once
//=================================================================
//create a snail shell from a base line shape read in from a txt file
// in the Assets/Models folder
//=================================================================

#include "VBGO.h"
#include <string>

struct GameData;

class VBSnail : public VBGO
{
public:
	VBSnail() {};
	virtual ~VBSnail() {};

	virtual void Tick(GameState* _GS) override;//Update per frame

	//initialize the Vertex and Index buffers for the cube
	void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game) override;

	void Load(ifstream& _file) override;//Load object data from file

protected:
	string m_filename;
	int m_sections;
	float m_shellScale;
	float m_shellRot;
	float m_step;
	Color m_col1;
	Color m_col2;
};