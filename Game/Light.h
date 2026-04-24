#pragma once
#include "gameobject.h"

//=================================================================
//a simple light defined by a colour and and ambient colour and 
//used as a point light with no drop off in the standard shader 
//used to draw VBGOs
//=================================================================

struct DrawData;

class Light : public GameObject
{
public:
	Light();
	~Light();

	virtual void Draw(DrawData* _DD) override { _DD; }; //draws nothing

	virtual void Tick(GameState* _GD) override;

	virtual void Load(ifstream& _file) override;

	virtual void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game) override {};

	//getters
	Color GetColour() { return m_colour; }
	Color GetAmbCol() { return m_ambientColour; }

	//setters
	void SetColor(Color _colour) { m_colour = _colour; }
	void SetAmbCol(Color _colour) { m_ambientColour = _colour; }

protected:

	Color m_colour;
	Color m_ambientColour;
};