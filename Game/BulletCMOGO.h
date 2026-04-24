#pragma once
#include "CMOGO.h"

//an object that demos Bullet physics

class Sound;

class BulletCMOGO : public CMOGO
{
public:
	BulletCMOGO();
	virtual ~BulletCMOGO();
	virtual void Tick(GameState* _GS) override;

	virtual void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game) override;

	virtual void OnCollision(GameObject* _other) override;

protected:

	Sound* m_sound = nullptr; //sound to play on collision
	bool m_CollisionThisFrame = false; //did we collide this frame?
	bool m_CollisionLastFrame = false; //did we collide last frame?
};

