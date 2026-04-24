#pragma once
#include "VBGO.h"

//=================================================================
//create a VBGO using data from a file 
//format is a sort-of a simplified version of the obj format
//=================================================================

class FileVBGO : public VBGO
{
public:
	FileVBGO();
	virtual ~FileVBGO();

	void Load(ifstream& _file) override;
	virtual void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game) override;

private:

	//data supplied by file tex coordinates or colours
	bool m_texCoords;
	bool m_colour;

	string m_file; //file to load data from placed in the Assets/Models folder
};