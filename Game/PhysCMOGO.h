#pragma once

//=================================================================
//A Class for loading and displaying CMO as GameObjects
//=================================================================

//you can test ypu have built your models correctly using this
//https://github.com/walbourn/directxtkmodelviewer
//TODO: lots of repeated code from other classes needs to be cleaned up later
//ideally we would have each GameObject have a Model component that handles all model loading and drawing

#include "PhysicsGO.h"
#include "Model.h"
#include <string>

using namespace std;
using namespace DirectX;

struct GameState;

class PhysCMOGO : public PhysicsGO
{
public:
	PhysCMOGO();
	virtual ~PhysCMOGO();

	virtual void Tick(GameState* _GD) override;
	virtual void Draw(DrawData* _DD) override;

	virtual void Load(ifstream& _file) override;
	virtual void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game) override;

protected:
	string m_file;

	unique_ptr<Model>  m_model;

	//needs a slightly different raster state that the VBGOs so create one and let them all use it
	static ID3D11RasterizerState* s_pRasterState;
	static int m_count;
};