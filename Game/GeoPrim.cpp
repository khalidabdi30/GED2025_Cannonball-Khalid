#include "pch.h"
#include "GeoPrim.h"
#include "stringHelp.h"
#include "DrawData.h"
#include "Camera.h"

GeoPrim::GeoPrim()
{
	m_type = "GEOPRIM";
}

GeoPrim::~GeoPrim()
{
}

void GeoPrim::Draw(DrawData* _DD)
{
	if (!m_isActive) return;

	m_shape->Draw(m_worldMat, _DD->m_cam->GetView(), _DD->m_cam->GetProj(), m_colour);
}

void GeoPrim::Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game)
{
	GameObject::Init(_GD, _EF, _game);

	ID3D11DeviceContext* deviceContext;
	_GD->GetImmediateContext(&deviceContext);

	switch (m_geoPrimType)
	{
	case GPT_BOX:
		const XMFLOAT3 size = XMFLOAT3(m_vSize.x, m_vSize.y, m_vSize.z);
		m_shape = GeometricPrimitive::CreateBox(deviceContext, size);
		break;
	case GPT_CONE:
		m_shape = GeometricPrimitive::CreateCone(deviceContext, m_diameter, m_height, m_tessellation);
		break;
	case GPT_CUBE:
		m_shape = GeometricPrimitive::CreateCube(deviceContext, m_fSize);
		break;
	case GPT_CYLINDER:
		m_shape = GeometricPrimitive::CreateCylinder(deviceContext, m_height, m_diameter, m_tessellation);
		break;
	case GPT_DODECAHEDRON:
		m_shape = GeometricPrimitive::CreateDodecahedron(deviceContext, m_fSize);
		break;
	case GPT_GEOSPHERE:
		m_shape = GeometricPrimitive::CreateGeoSphere(deviceContext, m_diameter, m_tessellation);
		break;
	case GPT_ICOSAHEDRON:
		m_shape = GeometricPrimitive::CreateIcosahedron(deviceContext, m_fSize);
		break;
	case GPT_OCTAHEDRON:
		m_shape = GeometricPrimitive::CreateOctahedron(deviceContext, m_fSize);
		break;
	case GPT_SPHERE:
		m_shape = GeometricPrimitive::CreateSphere(deviceContext, m_diameter, m_tessellation);
		break;
	case GPT_TEAPOT:
		m_shape = GeometricPrimitive::CreateTeapot(deviceContext, m_fSize, m_tessellation);
		break;
	case GPT_TETRAHEDRON:
		m_shape = GeometricPrimitive::CreateTetrahedron(deviceContext, m_fSize);
		break;
	case GPT_TORUS:
		m_shape = GeometricPrimitive::CreateTorus(deviceContext, m_diameter, m_thickness, m_tessellation);
		break;
	default:
		assert(0);
		printf("UNKNOWN GEOMETRIC PRIMITIVE TYPE DURING INIT: %s \n", m_primType.c_str());
		break;
	}
}

void GeoPrim::Load(ifstream& _file)
{
	GameObject::Load(_file);
	StringHelp::String(_file, "PRIMTYPE", m_primType);

	if (m_primType == "BOX")
	{
		m_geoPrimType = GPT_BOX;
		StringHelp::Float3(_file, "SIZE", m_vSize.x, m_vSize.y, m_vSize.z);
	}
	else if (m_primType == "CONE")
	{
		m_geoPrimType = GPT_CONE;
		StringHelp::Float(_file, "DIAMETER", m_diameter);
		StringHelp::Float(_file, "HEIGHT", m_height);
		StringHelp::Int(_file, "TESSELLATION", m_tessellation);
	}
	else if (m_primType == "CUBE")
	{
		m_geoPrimType = GPT_CUBE;
		StringHelp::Float(_file, "SIZE", m_fSize);
	}
	else if (m_primType == "CYLINDER")
	{
		m_geoPrimType = GPT_CYLINDER;
		StringHelp::Float(_file, "HEIGHT", m_height);
		StringHelp::Float(_file, "DIAMETER", m_diameter);
		StringHelp::Int(_file, "TESSELLATION", m_tessellation);
	}
	else if (m_primType == "DODECAHEDRON")
	{
		m_geoPrimType = GPT_DODECAHEDRON;
		StringHelp::Float(_file, "SIZE", m_fSize);
	}
	else if (m_primType == "GEOSPHERE")
	{
		m_geoPrimType = GPT_GEOSPHERE;
		StringHelp::Float(_file, "DIAMETER", m_diameter);
		StringHelp::Int(_file, "TESSELLATION", m_tessellation);

	}
	else if (m_primType == "ICOSAHEDRON")
	{
		m_geoPrimType = GPT_ICOSAHEDRON;
		StringHelp::Float(_file, "SIZE", m_fSize);
	}
	else if (m_primType == "OCTAHEDRON")
	{
		m_geoPrimType = GPT_OCTAHEDRON;
		StringHelp::Float(_file, "SIZE", m_fSize);
	}
	else if (m_primType == "SPHERE")
	{
		m_geoPrimType = GPT_SPHERE;
		StringHelp::Float(_file, "DIAMETER", m_diameter);
		StringHelp::Int(_file, "TESSELLATION", m_tessellation);
	}
	else if (m_primType == "TEAPOT")
	{
		m_geoPrimType = GPT_TEAPOT;
		StringHelp::Float(_file, "SIZE", m_fSize);
		StringHelp::Int(_file, "TESSELLATION", m_tessellation);
	}
	else if (m_primType == "TETRAHEDRON")
	{
		m_geoPrimType = GPT_TETRAHEDRON;
		StringHelp::Float(_file, "SIZE", m_fSize);
	}
	else if (m_primType == "TORUS")
	{
		m_geoPrimType = GPT_TORUS;
		StringHelp::Float(_file, "DIAMETER", m_diameter);
		StringHelp::Float(_file, "THICKNESS", m_thickness);
		StringHelp::Int(_file, "TESSELLATION", m_tessellation);
	}
	else
	{
		printf("UNKNOWN GEOMETRIC PRIMITIVE TYPE: %s \n", m_primType.c_str());
		assert(0);
	}

	StringHelp::Float4(_file, "COLOUR", m_colour.x, m_colour.y, m_colour.z, m_colour.w);

}
