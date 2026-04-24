#pragma once
#include "CMOGO.h"
#include <vector>
#include "PowerUpTypes.h"

class GamePadPhysCMOGO;

//an in-world object that players can interact with that grants power-ups and respawns after delay.
//uses bullet trigger collision physics CF_NO_CONTACT_RESPONSE.
//Flow: Player Collides -> Box Rolls a PU Type -> PUFactory creates PU -> PU Handler Stores it 
// -> Box Deactivates -> Box Respawns After Delay. 
class PowerUpBox : public CMOGO
{
public:
	PowerUpBox();

	virtual ~PowerUpBox();

	virtual void Init(ID3D11Device* device, IEffectFactory* fxFactory, Game* game) override;

	virtual void Tick(GameState* gs) override;

	virtual void OnCollision(GameObject* other) override;

	void SetRespawnDelay(float seconds) { respawnDelay = seconds; }

	void SetAvalPowerUps(const std::vector<PowerUpType>& types) { avaliableTypes = types; }

private:
	PowerUpType RollPowerUp() const;

private:

	bool isUsed = false;

	float respawnDelay = 5.0f;
	float respawnTimer = 0.0f;

	//Handles which PU types this box can roll.
	std::vector<PowerUpType> avaliableTypes;
};
