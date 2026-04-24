#pragma once
#include "gameobject.h"

//adds basic physics to a Game Object
//essentially a Euler solver for Newtonian physics
//with each frame adding forces or acceleration to the object

class PhysicsGO : public GameObject
{
public:
	PhysicsGO();
	virtual ~PhysicsGO();

	virtual void Tick(GameState* _GS) override;

	virtual void Load(ifstream& _file) override;

	void AddForce(Vector3 _force) { m_force += _force; } //add a force to this object for this frame
	void AddAcc(Vector3 _acc) { m_force += m_mass * _acc; } //add an acceleration to this object for this frame
	void AddDrag(float _factor) { m_force -= _factor * m_mass * m_vel; } //add drag force proportional to velocity

protected:

	float m_mass;

	Vector3 m_vel;
	Vector3 m_force;
};

