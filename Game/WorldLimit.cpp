#include "pch.h"
#include "WorldLimit.h"
#include "stringHelp.h"
#include "Game.h"

WorldLimit::WorldLimit()
{
	m_type = "WORLD_LIMIT";
}

WorldLimit::~WorldLimit()
{
}

void WorldLimit::Load(ifstream& _file)
{
	StringHelp::String(_file, "NAME", m_name);
	StringHelp::Float3(_file, "POS", m_pos.x, m_pos.y, m_pos.z);
	StringHelp::Float3(_file, "SCALE", m_scale.x, m_scale.y, m_scale.z);
	StringHelp::String(_file, "LIMIT_TYPE", m_limitType);

	if (m_limitType == "LOCATION")
	{
		StringHelp::Float3(_file, "LOCATION", m_zapp.x, m_zapp.y, m_zapp.z);
	}
	else if (m_limitType == "HEIGHT")
	{
		StringHelp::Float(_file, "HIEGHT", m_zapp.y);
	}
	else
	{
		cout << "UNKNOWN WORLD LIMIT TYPE " << m_limitType << endl;
		assert(false);
	}

	m_hasPhysics = true; //this is a physics object but it is static
	m_mass = 0.0f; //static object has no mass
}

void WorldLimit::Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game)
{
	//bullet physics initialization
	m_colShape = new btBoxShape(btVector3(btScalar(m_scale.x), btScalar(m_scale.y), btScalar(m_scale.z)));
	_game->GetCollisionShapes()->push_back(m_colShape);

	btTransform transform;
	transform.setIdentity();

	transform.setOrigin(btVector3(btScalar(m_pos.x), btScalar(m_pos.y), btScalar(m_pos.z)));

	btScalar mass(m_mass);

	btVector3 localInertia(0, 0, 0);

	//using motion state is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	m_MotionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_MotionState, m_colShape, localInertia);
	m_rigidBody = new btRigidBody(rbInfo);
	m_rigidBody->setUserPointer(this);

	//add the body to the dynamics world
	_game->GetDynamicsWorld()->addRigidBody(m_rigidBody);
}

void WorldLimit::OnCollision(GameObject* _other)
{
	if (m_limitType == "LOCATION")
	{
		_other->SetPos(m_zapp); //set the position of the object to the zap location
	}
	else if (m_limitType == "HEIGHT")
	{
		_other->SetPos(Vector3(_other->GetPos().x, m_zapp.y, _other->GetPos().z)); //set the y position of the object to the zap height	
	}

	cout << "WorldLimit: " << m_name << " collided with " << _other->GetName() << endl;
}
