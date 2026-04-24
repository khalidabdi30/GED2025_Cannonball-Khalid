#include "pch.h"
#include "GameState.h"
#include "Sound.h"
#include "Game.h"
#include <iostream>
#include "BulletCMOGO.h"


BulletCMOGO::BulletCMOGO()
{
	m_type = "BULLET_CMOGO";
}

BulletCMOGO::~BulletCMOGO()
{
}

void BulletCMOGO::Tick(GameState* _GS)
{
	if (_GS->m_KBS_tracker.IsKeyPressed(Keyboard::Keys::Q))
	{
		//pressing Q toggles the active state of the object
		ToggleActive();
	}

	if (_GS->m_KBS_tracker.IsKeyPressed(Keyboard::Keys::X))
	{
		//pressing X to wake up the object
		m_rigidBody->activate(true);
		cout << "Waking up " << m_name << " " << m_rigidBody->getActivationState() << endl;
	}

	if (!m_isActive) return;

	if (!m_CollisionLastFrame && m_CollisionThisFrame)
	{
		if (m_sound)
		{
			m_sound->Play();
		}
	}
	m_CollisionLastFrame = m_CollisionThisFrame;
	m_CollisionThisFrame = false;

	Vector3 push = Vector3::Zero;

	//most of these are all flipped thanks to the model being backwards
	//if (_GS->m_KBS.Up)
	//{
	//	push += Vector3::Backward;
	//}
	//if (_GS->m_KBS.Down)
	//{
	//	push += Vector3::Forward;
	//}
	//if (_GS->m_KBS.Left)
	//{
	//	push += Vector3::Right;
	//}
	//if (_GS->m_KBS.Right)
	//{
	//	push += Vector3::Left;
	//}
	if (_GS->m_KBS.Enter)
	{
		push += 5.0f * Vector3::Up;
	}
	if (_GS->m_KBS.RightShift)
	{
		push += 5.0f * Vector3::Down;
	}

	push *= 50.0;

	Vector3 torque = Vector3::Zero;

	if (_GS->m_KBS.OemComma)
	{
		torque += Vector3::Up;
	}

	if (_GS->m_KBS.OemPeriod)
	{
		torque += Vector3::Down;
	}

	torque *= 20.0f;

	CMOGO::Tick(_GS);

	//rotate push based on the orientation of the object
	push = Vector3::Transform(push, m_rotMat);

	m_rigidBody->applyCentralForce(btVector3(push.x, push.y, push.z));
	m_rigidBody->applyTorque(btVector3(torque.x, torque.y, torque.z));
}

void BulletCMOGO::Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game)
{
	m_sound = _game->GetSoundByName("BULLET_COLLISION");
	if (m_sound)
	{
		m_sound->SetVolume(0.1f);
	}

	CMOGO::Init(_GD, _EF, _game);

	if (m_hasPhysics)
	{
		//if the linear and angular velocity fall below these values the body can go to sleep
		m_rigidBody->setSleepingThresholds(btScalar(0.01f), btScalar(0.01f));

		m_rigidBody->setDamping(0.1f, 0.1f); //linear and angular damping

		m_rigidBody->setAngularFactor(btVector3(0, 1, 0)); //allow rotation only around the y axes

		//set up constraints
		//see Bullet docs for more information on constraints
		btTransform localA;
		localA.setIdentity(); localA.getOrigin() = btVector3(0, 0, 0);
		btGeneric6DofSpring2Constraint* constraint = new btGeneric6DofSpring2Constraint(*m_rigidBody, localA);
		constraint->setLinearLowerLimit(btVector3(-10, 0, -10));
		constraint->setLinearUpperLimit(btVector3(10, 10, 10));
		_game->GetDynamicsWorld()->addConstraint(constraint, true);
	}
}

void BulletCMOGO::OnCollision(GameObject* _other)
{
	//collision are checked before the Tick function is called
	//this way the object itself reacts to the collision whilst its doing its Tick
	m_CollisionThisFrame = true;
}
