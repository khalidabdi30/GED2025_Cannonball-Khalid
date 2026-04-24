#include <iostream>
#include <algorithm>
#include "pch.h"
#include "GameState.h"
#include "Game.h"
#include "PowerUpHandler.h"
#include "PlayerObject.h"

//TEMP PLAYER OBJECT USED FOR TESTING POWER UPS! PLEASE SEE COMMENTS FOR WHAT NEEDS TO BE INCLUDED
// TO ADD POWER-UPS TO ACTUAL PLAYERS.
PlayerObject::PlayerObject()
{
	m_type = "PLAYER";
}

PlayerObject::~PlayerObject()
{
	//NEEDED IN THE DECONSTRUCTOR
	delete m_powerUpHandler;
	m_powerUpHandler = nullptr;
}

void PlayerObject::Init(ID3D11Device* device, IEffectFactory* fxFactory, Game* game)
{
	CMOGO::Init(device, fxFactory, game);
	//NOT NEEDED FOR POWERUPS
	if (m_hasPhysics)
	{
		m_rigidBody->setAngularFactor(btVector3(0, 0, 0)); //stop constant rotation

		//set up constraints so player doesnt roll over constantly
		btTransform localA;
		localA.setIdentity(); localA.getOrigin() = btVector3(0, 0, 0);
		btGeneric6DofSpring2Constraint* constraint = new btGeneric6DofSpring2Constraint(*m_rigidBody, localA);
		constraint->setLinearLowerLimit(btVector3(-10, 0, -10));
		constraint->setLinearUpperLimit(btVector3(10, 10, 10));
		game->GetDynamicsWorld()->addConstraint(constraint, true);
	}

	//NEEDED - Create PU Handler
	m_powerUpHandler = new PowerUpHandler();
	
}

void PlayerObject::Tick(GameState* gs)
{
	m_rigidBody->activate(true);
	if (!gs) return;

	//NEEDED FOR POWERUPS
	if (m_powerUpHandler)
	{
		m_powerUpHandler->Tick(gs->m_dt);
	}

	//NEEDED FOR POWERUPS - ACTIVATION KEY NEEDS TO SWITCH TO CONTROLLER INPUT
	if (gs->m_KBS_tracker.pressed.E)//SWITCH INPUT HERE
	{
		if (m_powerUpHandler)
		{
			m_powerUpHandler->UsePowerUp(this);
		}
	}

	if (!m_isActive) return;
	if (!m_hasPhysics || !m_rigidBody) return;

	//movement input
	Vector3 push = Vector3::Zero;
	if (gs->m_KBS.Up)
	{
		push += Vector3::Left;
	}
	if (gs->m_KBS.Down)
	{
		push += Vector3::Right;
	}
	if (gs->m_KBS.Left)
	{
		push += Vector3::Backward;
	}
	if (gs->m_KBS.Right)
	{
		push += Vector3::Forward;
	}

	Vector3 turn = Vector3::Zero;
	if (gs->m_KBS.A) turn += Vector3::Up;
	if (gs->m_KBS.D) turn += Vector3::Down;

	float speedMulti = 1.0f;
	float controlFlip = 1.0f;
	float turnMulti = 1.0f;
	if (m_powerUpHandler)
	{
		speedMulti = m_powerUpHandler->GetSpeedMulti();
		controlFlip = m_powerUpHandler->GetControlFlip();
		turnMulti = m_powerUpHandler->GetTurnMulti();
	}

	const float accelerationScale = m_acceleration * speedMulti;
	const float turnSpeedScale = m_turnSpeed * turnMulti;
	const float maxSpeed = m_maxSpeed * speedMulti;

	push *= controlFlip;
	push *= accelerationScale;
	turn *= turnSpeedScale * controlFlip;

	push = Vector3::Transform(push, m_rotMat);

	//apply forces
	m_rigidBody->applyCentralForce(btVector3(push.x, push.y, push.z));
	m_rigidBody->applyTorque(btVector3(turn.x, turn.y, turn.z));

	//clamp max speed
	btVector3 v = m_rigidBody->getLinearVelocity();
	btScalar speed = v.length();
	if (speed > maxSpeed)
	{
		btVector3 clamped = v.normalized() * maxSpeed;
		m_rigidBody->setLinearVelocity(clamped);
	}

	CMOGO::Tick(gs);
}

void PlayerObject::OnCollision(GameObject* other)
{

}
