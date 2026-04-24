#pragma once
#include "gameobject.h"

class Game;

//=================================================================
//Base Camera Class
//=================================================================

class Camera : public GameObject
{
public:
	Camera();
	~Camera();

	virtual void PreRender(DrawData* _DD);//set up this camera for rendering

	virtual void Tick(GameState* _GS) override; //Update per frame

	virtual void Draw(DrawData* _DD) override; //Cameras don't draw anything as a rule but debugging views might

	virtual void Load(ifstream& _file) override; //Load object data from file
	virtual void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game) override; //Initialize any resources needed

	//Getters
	Matrix GetProj() { return m_projMat; }
	Matrix GetView() { return m_viewMat; }

	bool CanBeMainCamera() { return m_canBeMainCamera; } //some cameras are not eligible to be the main camera i.e. RenderTargets


protected:

	//Principle transforms/matrices for this camera
	Matrix m_projMat;
	Matrix m_viewMat;

	//parameters for setting up a camera
	float m_fieldOfView;
	float m_aspectRatio;
	float m_nearPlaneDistance;
	float m_farPlaneDistance;
	Vector3 m_target;
	Vector3 m_up;

	//Viewport
	D3D11_VIEWPORT m_viewport;

	bool m_canBeMainCamera;

};