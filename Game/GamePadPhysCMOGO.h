#pragma once
#include "PhysCMOGO.h"

//a Game Object that is a physics object controlled by a gamepad
//and has a model loaded from a .cmo file
class PowerUpHandler;

class GamePadPhysCMOGO : public PhysCMOGO
{
public:
	GamePadPhysCMOGO();
	~GamePadPhysCMOGO();
	virtual void Init(ID3D11Device* device, IEffectFactory* fxFactory, Game* game) override;
	virtual void Tick(GameState* _GS) override;
	virtual void Load(ifstream& _file) override;
	virtual void OnCollision(GameObject* other) override;

	PowerUpHandler* GetPowerUpHandler() const
	{
		return m_powerUpHandler;
	}

protected:

	int m_device = -1;//which gamepad to use

	float m_baseMoveForce = 25.0f;     // replaces the magic number you had as invertForward
	float m_yawSpeed = 0.02f;

private:
	PowerUpHandler* m_powerUpHandler = nullptr;
	size_t m_aiWaypointIndex = 0;
	float m_stuckTimer = 0.0f;
	float m_wallRecoverTimer = 0.0f;
	float m_aiPowerUpUseTimer = 0.0f;
	Vector3 m_wallRecoverDir = Vector3::Backward;

	void TickNpc(GameState* gs);
};
