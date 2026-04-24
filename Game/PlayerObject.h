#pragma once
#include "gameobject.h"
#include "CMOGO.h"

class PowerUpHandler;
struct GameState;

class PlayerObject : public CMOGO
{
public:
	PlayerObject();

	virtual ~PlayerObject();

	virtual void Init(ID3D11Device* device, IEffectFactory* fxFactory, Game* game) override;

	virtual void Tick(GameState* gs) override;

	virtual void OnCollision(GameObject* other) override;

	//NEEDED FOR POWERUPS
	PowerUpHandler* GetPowerUpHandler() const { return m_powerUpHandler; }

private:

	//Player Movement
	float m_acceleration = 18.0f;
	float m_maxSpeed = 30.0f;
	float m_turnSpeed = 2.5f;
	float m_currentSpeed = 1.0f;

	//NEEDED FOR POWERUPS
	PowerUpHandler* m_powerUpHandler = nullptr;
};
