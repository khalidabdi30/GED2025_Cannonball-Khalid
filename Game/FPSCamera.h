#pragma once
#include "camera.h"

//a first-person style camera

class FPSCamera : public Camera
{
public:
	FPSCamera();
	~FPSCamera();

	virtual void Tick(GameState* _GS) override;

	virtual void Load(ifstream& _file) override;

protected:

	float m_theta, m_phi; // spherical coordinates theta (rotation around the x axis) and phi (rotation around the y axis).  <theta, phi> are stored in degrees.  Zero degree rotation on <theta, phi> places the camera on the +z axis.  A positive phi represents counter-clockwise rotation around the y axis in a right-hand coordinate system.  A positive theta represents a counter-clockwise rotation around the x axis in a right-handed coordinate system

	float m_speed; // movement speed in units per second
	float m_spin; // rotation speed in radians per second

};

