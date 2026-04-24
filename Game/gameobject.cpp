#include "pch.h"
#include "gameobject.h"
#include "GameState.h"
#include "stringHelp.h"
#include "Game.h"

GameObject::GameObject()
{
	//set the GameObject to the origin with no rotation and unit scaling 
	m_pos = Vector3::Zero;
	m_pitch = 0.0f;
	m_yaw = 0.0f;
	m_roll = 0.0f;
	m_scale = Vector3::One;

	m_worldMat = Matrix::Identity;
	m_fudge = Matrix::Identity;
	m_rotMat = Matrix::Identity;
	m_orientation = Quaternion::Identity;
}

GameObject::~GameObject()
{

}

void GameObject::Tick(GameState* _GS)
{
	if (!m_isActive) return;

	if (m_hasPhysics)
	{
		//update the position and rotation from bullet physics
		btTransform trans;
		m_rigidBody->getMotionState()->getWorldTransform(trans);

		m_pos = Vector3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());

		btQuaternion orientation = trans.getRotation();
		m_orientation = Quaternion(orientation.getX(), orientation.getY(), orientation.getZ(), orientation.getW());
	}

	//build up the world matrix depending on the new position of the GameObject
	//the assumption is that this class will be inherited by the class that ACTUALLY changes this
	Matrix  scaleMat = Matrix::CreateScale(m_scale);
	if (m_hasPhysics)
	{
		m_rotMat = Matrix::CreateFromQuaternion(m_orientation);
	}
	else
	{
		m_rotMat = Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, m_roll); //possible not the best way of doing this!
	}
	Matrix  transMat = Matrix::CreateTranslation(m_pos);

	//m_fudge is for any extra adjustments needed to the world matrix due to bad artists or other issues
	m_worldMat = m_fudge * scaleMat * m_rotMat * transMat;

	//we are rebuilding this instead of just using matrix from Bullet because of potential issues with the
	//way that vectors/matrices maybe represented in Bullet vs DirectX (row vs column major for example)
}

void GameObject::Load(ifstream& _file)
{
	StringHelp::String(_file, "NAME", m_name);
	StringHelp::Float3(_file, "POS", m_pos.x, m_pos.y, m_pos.z);
	StringHelp::Float3(_file, "ROT", m_yaw, m_pitch, m_roll);
	m_yaw = XM_PI * m_yaw / 180.0f;
	m_pitch = XM_PI * m_pitch / 180.0f;
	m_roll = XM_PI * m_roll / 180.0f;
	StringHelp::Float3(_file, "SCALE", m_scale.x, m_scale.y, m_scale.z);
	string physicsQuestion;
	StringHelp::String(_file, "HAS_PHYSICS", physicsQuestion);
	if (physicsQuestion == "TRUE")
	{
		m_hasPhysics = true;
		StringHelp::String(_file, "PHYSICS_SHAPE", m_collisionShapeType);
		if (m_collisionShapeType == "BOX")
		{
			StringHelp::Float3(_file, "COLLISION_SHAPE_SIZE", m_collisionShapeSize.x, m_collisionShapeSize.y, m_collisionShapeSize.z);
		}
		else if (m_collisionShapeType == "SPHERE")
		{
			StringHelp::Float(_file, "COLLISION_SHAPE_RADIUS", m_collisionShapeSize.x);
		}
		else
		{
			cout << "UNKNOWN COLLISION SHAPE TYPE " << m_collisionShapeType << endl;
			assert(false);
		}

		StringHelp::Float(_file, "MASS", m_mass);
		StringHelp::Float(_file, "RESTITUTION", m_restitution);
	}
	else
	{
		m_hasPhysics = false;
	}
}

void GameObject::Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game)
{
	m_game = _game;

	if (m_hasPhysics)
	{
		//bullet physics initialization
		if (m_collisionShapeType == "BOX")
		{
			m_colShape = new btBoxShape(btVector3(btScalar(m_collisionShapeSize.x), btScalar(m_collisionShapeSize.y), btScalar(m_collisionShapeSize.z)));
		}
		else if (m_collisionShapeType == "SPHERE")
		{
			m_colShape = new btSphereShape(btScalar(m_collisionShapeSize.x));
		}

		_game->GetCollisionShapes()->push_back(m_colShape);

		btTransform transform;
		transform.setIdentity();

		transform.setOrigin(btVector3(btScalar(m_pos.x), btScalar(m_pos.y), btScalar(m_pos.z)));
		const Quaternion startOrientation = Quaternion::CreateFromYawPitchRoll(m_yaw, m_pitch, m_roll);
		transform.setRotation(btQuaternion(startOrientation.x, startOrientation.y, startOrientation.z, startOrientation.w));

		btScalar mass(m_mass);

		//rigid body is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			m_colShape->calculateLocalInertia(mass, localInertia);

		//using motion state is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		m_MotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_MotionState, m_colShape, localInertia);
		m_rigidBody = new btRigidBody(rbInfo);
		m_rigidBody->setUserPointer(this);

		m_rigidBody->setRestitution(m_restitution);

		//add the body to the dynamics world
		_game->GetDynamicsWorld()->addRigidBody(m_rigidBody);
	}
}

void GameObject::SetPos(Vector3 _pos)
{
	m_pos = _pos;
	if (m_hasPhysics)
	{
		//update the position in bullet physics
		m_rigidBody->getWorldTransform().setOrigin(btVector3(btScalar(_pos.x), btScalar(_pos.y), btScalar(_pos.z)));
	}
}

void GameObject::Activate()
{
	if (!m_isActive)
	{
		m_isActive = true;
		if (m_hasPhysics)
		{
			m_rigidBody->setActivationState(true);
		}
	}
}

void GameObject::Deactivate()
{
	if (m_isActive)
	{
		m_isActive = false;
		if (m_hasPhysics)
		{
			m_rigidBody->setActivationState(false);
		}
	}
}

void GameObject::SetActive(bool _active)
{
	if (m_isActive != _active)
	{
		m_isActive = _active;
		if (m_hasPhysics)
		{
			if (m_isActive)
			{
				m_rigidBody->setActivationState(true);
			}
			else
			{
				m_rigidBody->setActivationState(false);
			}
		}
	}
}

void GameObject::ToggleActive()
{
	m_isActive = !m_isActive;
	if (m_hasPhysics)
	{
		if (m_isActive)
		{
			m_rigidBody->setActivationState(true);
		}
		else
		{
			m_rigidBody->setActivationState(false);
		}
	}
}
