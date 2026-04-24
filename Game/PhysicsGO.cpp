#include "pch.h"
#include "PhysicsGO.h"
#include "GameState.h"
#include "stringHelp.h"

PhysicsGO::PhysicsGO()
{
	m_type = "PHYSICSGO";
}

PhysicsGO::~PhysicsGO()
{
}

void PhysicsGO::Tick(GameState* _GS)
{
	if (!m_isActive) return;

	Vector3 acc = m_force / m_mass;

	Vector3 newPos = m_pos + m_vel * _GS->m_dt;
	Vector3 newVel = m_vel + acc * _GS->m_dt;

	m_pos = newPos;
	m_vel = newVel;
	m_force = Vector3::Zero; //reset forces to allow addition of them during the next tick

	GameObject::Tick(_GS);
}

void PhysicsGO::Load(ifstream& _file)
{
	GameObject::Load(_file);
	if (!m_hasPhysics)
		StringHelp::Float(_file, "MASS", m_mass);
}
