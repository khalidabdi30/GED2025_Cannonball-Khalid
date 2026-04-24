#pragma once

//=================================================================
//the base Game Object 2D
//=================================================================
#include <d3d11_1.h>
#include "SimpleMath.h"
#include <string>
#include <fstream>

using namespace DirectX;
using namespace SimpleMath;
using namespace std;

struct GameState;
struct DrawData2D;
class Game;

class GameObject2D
{
public:
	GameObject2D();
	virtual ~GameObject2D() {};

	virtual void Tick(GameState* _GS); //Update per frame
	virtual void Draw(DrawData2D* _DD2D) = 0; //Render the object
	virtual void Load(ifstream& _file) = 0;	//Load object data from file
	virtual void Init(ID3D11Device* _GD, Game* _game) = 0; //Initialize any resources needed

	//setters
	void SetPos(Vector2 _pos) { m_pos = _pos; }
	void SetRot(float _rot) { m_rotation = _rot; }
	void SetColour(Color _colour) { m_colour = _colour; }
	void SetScale(Vector2 _scale) { m_scale = _scale; }
	void SetScale(float _scale) { m_scale = _scale * Vector2::One; }
	void SetOrigin(Vector2 _origin) { m_origin = _origin; }
	void SetName(string _name) { m_name = _name; }

	//mutators
	string GetName() { return m_name; }
	string GetType() { return m_type; }

protected:
	string m_name;

	string m_type;
	string m_file;

	Vector2 m_pos;
	float m_rotation;
	Vector2 m_scale;
	Color m_colour;
	Vector2 m_origin;
};