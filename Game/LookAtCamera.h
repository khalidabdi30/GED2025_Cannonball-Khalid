#pragma once
#include "camera.h"

// a camera that looks at a specified Game Object

class LookAtCamera : public Camera
{
public:
	LookAtCamera();
	~LookAtCamera();

	virtual void Tick(GameState* _GS) override;
	virtual void PreRender(DrawData* _DD) override;

	virtual void Load(ifstream& _file) override;
	virtual void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game) override;

protected:

	Vector3 m_dPos; // offset from the target object's position

	string m_GO_Name; // name of the Game Object to look at

	GameObject* m_GO; // pointer to the Game Object to look at pulled in at Init time using m_GO_Name

	float m_X, m_Y;//locations of the top left corner of the viewport as fractions of the screen
	float m_width, m_height;//size of the viewport as fractions of the screen

	void SetVP(int _w, int _h);

};

