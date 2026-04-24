//base camera class
#include "pch.h"
#include "camera.h"
#include "stringHelp.h"
#include "Game.h"
#include "DrawData.h"

Camera::Camera()
{
	m_type = "CAMERA";
	m_canBeMainCamera = true;
}

Camera::~Camera()
{
}

void Camera::PreRender(DrawData* _DD)
{
	m_viewport = { 0.0f, 0.0f, (float)_DD->m_width, (float)_DD->m_height, 0.f, 1.f };
	m_aspectRatio = (float)_DD->m_width / (float)_DD->m_height;
	_DD->m_pd3dImmediateContext->RSSetViewports(1, &m_viewport);
}

void Camera::Tick(GameState* _GS)
{
	//calculate the projection and view matrices to all this camera to be used
	m_projMat = Matrix::CreatePerspectiveFieldOfView(m_fieldOfView, m_aspectRatio, m_nearPlaneDistance, m_farPlaneDistance);
	m_viewMat = Matrix::CreateLookAt(m_pos, m_target, m_up);
	GameObject::Tick(_GS);
}

void Camera::Draw(DrawData* _DD)
{
	_DD;
	//standard camera doesn't draw ANYTHING
}

void Camera::Load(ifstream& _file)
{
	StringHelp::String(_file, "NAME", m_name);
	StringHelp::Float3(_file, "POS", m_pos.x, m_pos.y, m_pos.z);
	StringHelp::Float3(_file, "LOOKAT", m_target.x, m_target.y, m_target.z);
	StringHelp::Float3(_file, "UP", m_up.x, m_up.y, m_up.z);
	StringHelp::Float(_file, "FOV", m_fieldOfView);
	m_fieldOfView = XM_PI * m_fieldOfView / 180.0f;
	StringHelp::Float(_file, "NEAR", m_nearPlaneDistance);
	StringHelp::Float(_file, "FAR", m_farPlaneDistance);
}

void Camera::Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game)
{
	m_viewport = { 0.0f, 0.0f, (float)_game->GetWidth(), (float)_game->GetHeight(), 0.f, 1.f };
	m_aspectRatio = (float)_game->GetWidth() / (float)_game->GetHeight();
}