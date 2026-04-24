#pragma once
#include "gameobject.h"
#include <GeometricPrimitive.h>

//demo geometric primitive types
//good base shapes for debugging, prototyping, or simple games
//more info on DirectXTK GeometricPrimitive class:
//https://github.com/microsoft/DirectXTK/wiki/GeometricPrimitive

enum GeoPrimType
{
	GPT_UNKNOWN,
	GPT_BOX,
	GPT_CONE,
	GPT_CUBE,
	GPT_CYLINDER,
	GPT_DODECAHEDRON,
	GPT_GEOSPHERE,
	GPT_ICOSAHEDRON,
	GPT_OCTAHEDRON,
	GPT_SPHERE,
	GPT_TEAPOT,
	GPT_TETRAHEDRON,
	GPT_TORUS
};

class GeoPrim : public GameObject
{
public:
	GeoPrim();
	virtual ~GeoPrim();
	virtual void Draw(DrawData* _DD) override;
	virtual void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game) override;
	virtual void Load(ifstream& _file) override;

protected:

	std::unique_ptr<GeometricPrimitive> m_shape;
	string m_primType;

	GeoPrimType m_geoPrimType = GPT_UNKNOWN;

	Color m_colour;

	//geometric parameters
	Vector3 m_vSize = Vector3::One;
	float m_diameter = 1.0f;
	float m_height = 1.0f;
	int m_tessellation = 16;
	float m_fSize = 1.0f;
	float m_thickness = 0.2f;
};

