#include "pch.h"
#include "FPSCamera.h"
#include "GameState.h"
#include "stringHelp.h"

FPSCamera::FPSCamera()
{
	m_type = "FPSCamera";
	m_canBeMainCamera = true;

	m_theta = 0.0f;
	m_phi = 0.0f;
}

FPSCamera::~FPSCamera()
{
}

void FPSCamera::Tick(GameState* _GS)
{
	//don't tick me if we are paused
	if (_GS->m_FSM == FSM_PAUSE) return;

	Vector3 move = Vector3::Zero;//this is how I'll move this frame

	//mouse for rotation
	//m_theta += _GS->m_MS.x * m_spin * _GS->m_dt;
	//m_phi += _GS->m_MS.y * m_spin * _GS->m_dt;

	m_theta -= _GS->m_MS.x * m_spin * _GS->m_dt; //mouse looking left and right	
	m_phi -= _GS->m_MS.y * m_spin * _GS->m_dt; //mouse looking up and down

	//limit rotation
	if (m_phi < (-XM_PIDIV2 + 0.001f))
	{
		m_phi = (-XM_PIDIV2 + 0.001f);
	}

	if (m_phi > (XM_PIDIV2 - 0.001f))
	{
		m_phi = (XM_PIDIV2 - 0.001f);
	}

	//move position
	if (_GS->m_KBS.A)
	{
		move -= Vector3::Right;
	}
	if (_GS->m_KBS.D)
	{
		move += Vector3::Right;
	}
	if (_GS->m_KBS.W)
	{
		move += Vector3::Forward;
	}
	if (_GS->m_KBS.S)
	{
		move -= Vector3::Forward;
	}
	if (_GS->m_KBS.R)
	{
		move += Vector3::Up;
	}
	if (_GS->m_KBS.F)
	{
		move -= Vector3::Up;
	}

	//rotate move vector by current orientation
	move = Vector3::Transform(move, Matrix::CreateRotationY(m_theta));
	m_pos += move * m_speed * _GS->m_dt;

	m_target = Vector3::Transform(Vector3::Forward, Matrix::CreateFromYawPitchRoll(m_theta, m_phi, 0.0f));// Matrix::CreateRotationX(m_phi)* Matrix::CreateRotationY(m_theta) );
	m_target = m_pos + m_target;

	Camera::Tick(_GS);
}

void FPSCamera::Load(ifstream& _file)
{
	Camera::Load(_file);
	StringHelp::Float(_file, "ROT_SPEED", m_spin);
	StringHelp::Float(_file, "SPEED", m_speed);
	m_theta = XM_PI * m_target.x / 180.0f;
	m_phi = XM_PI * m_target.y / 180.0f;
}
