#pragma once
#include "gameobject.h"

//Sets a bottom to the world via a large box collider which zaps objects that go out of bounds
//sets them either to a fixed location or to a height

class WorldLimit : public GameObject
{
public:

	WorldLimit();
	virtual ~WorldLimit();
	virtual void Load(ifstream& _file) override;
	virtual void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game) override;
	virtual void Draw(DrawData* _DD) override {} //no need to draw this object

	virtual void OnCollision(GameObject* _other) override;

protected:

	Vector3 m_zapp = Vector3::Zero; //location to zap objects to if they go out of bounds

	string m_limitType = "";//either "LOCATION" or "HEIGHT"
};

