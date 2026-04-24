#pragma once
#include "GameObject2D.h"

//a line of text as a 2D Game Object

class TextGO2D :public GameObject2D
{
public:
	TextGO2D();

	virtual void Tick(GameState* _GS) override;
	virtual void Draw(DrawData2D* _DD2D) override;

	virtual void Load(ifstream& _file) override;
	virtual void Init(ID3D11Device* _GD, Game* _game) override;

protected:
	string m_text;
};