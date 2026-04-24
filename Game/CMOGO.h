#pragma once

//=================================================================
//A Class for loading and displaying CMO as GameObjects
//=================================================================

//you can test you have built your models correctly using this
//https://github.com/walbourn/directxtkmodelviewer

//CMOs are created using the Assets Models project
//makes sure you rebuild that when ever you add or change a model

#include "gameobject.h"
#include "Model.h"
#include <string>

using namespace std;
using namespace DirectX;

struct GameState;

class CMOGO : public GameObject
{
public:
	CMOGO();
	virtual ~CMOGO();

	virtual void Tick(GameState* _GD) override;
	virtual void Draw(DrawData* _DD) override;

	virtual void Load(ifstream& _file) override;
	virtual void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game) override;

	virtual void OnCollision(GameObject* _other) override;

protected:
	string m_file;

	unique_ptr<Model>  m_model;

	//needs a slightly different raster state that the VBGOs so create one and let them all use it
	static ID3D11RasterizerState* s_pRasterState;
	static int m_count;
};