#pragma once
#include "PowerUp.h"

//A simple power-up that boosts players base speed.
//FORWARD DECLARE ACTUAL PLAYER CLASS
class GamePadPhysCMOGO;
class PowerUpHandler;

class SpeedBoostPowerUp: public PowerUp
{
public:
	SpeedBoostPowerUp(float boostMulti, float duration);

	virtual void Activate(GameObject* user) override;

private:
	//base Values do not modify
	float m_boostMulti = 1.0f;
	float m_duration = 0.0f;
};