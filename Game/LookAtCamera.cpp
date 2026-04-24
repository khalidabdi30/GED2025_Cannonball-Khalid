#include "pch.h"
#include "LookAtCamera.h"
#include "Game.h"
#include "stringHelp.h"
#include "DrawData.h"
#include "ViewportCam.h"

LookAtCamera::LookAtCamera()
{
    m_type = "LOOKAT";
    m_canBeMainCamera = false; //Can't be true or else it'll break

    m_X = 0.0f;
    m_Y = 0.0f;
    m_width = 1.0f;
    m_height = 1.0f;

    m_GO = nullptr;
}

LookAtCamera::~LookAtCamera()
{
}

void LookAtCamera::PreRender(DrawData* _DD)
{
    SetVP(_DD->m_width, _DD->m_height);

    float vpW = (float)_DD->m_width * m_width;
    float vpH = (float)_DD->m_height * m_height;
    m_aspectRatio = (vpH > 0.0f) ? (vpW / vpH) : 1.0f;

    _DD->m_pd3dImmediateContext->RSSetViewports(1, &m_viewport);
}

void LookAtCamera::Tick(GameState* _GS)
{
    if (!m_GO) return;

    // Get the character's position
    m_target = m_GO->GetPos();

    // Rotate the camera offset by the character's rotation matrix
    // This ensures the camera is always behind the character
    Matrix rotation = m_GO->GetRotMat();
    Vector3 rotatedOffset = Vector3::Transform(m_dPos, rotation);

    // Update camera position
    m_pos = m_target + rotatedOffset;

    // Call base camera update
    Camera::Tick(_GS);
}

void LookAtCamera::Load(ifstream& _file)
{
    StringHelp::String(_file, "NAME", m_name);
    StringHelp::Float3(_file, "DPOS", m_dPos.x, m_dPos.y, m_dPos.z); // e.g., (0, 5, -10)
    StringHelp::String(_file, "GAMEOBJECT", m_GO_Name);
    StringHelp::Float3(_file, "UP", m_up.x, m_up.y, m_up.z);
    StringHelp::Float(_file, "FOV", m_fieldOfView);
    m_fieldOfView = XM_PI * m_fieldOfView / 180.0f;
    StringHelp::Float(_file, "NEAR", m_nearPlaneDistance);
    StringHelp::Float(_file, "FAR", m_farPlaneDistance);
    StringHelp::Float(_file, "X", m_X);
    StringHelp::Float(_file, "Y", m_Y);
    StringHelp::Float(_file, "WIDTH", m_width);
    StringHelp::Float(_file, "HEIGHT", m_height);
}

void LookAtCamera::Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game)
{
    // Find the GameObject to follow
    m_GO = _game->GetGOByName(m_GO_Name);

    // gets width and height
    SetVP(_game->GetWidth(), _game->GetHeight());
    m_aspectRatio = (float)_game->GetWidth() / (float)_game->GetHeight();

    Camera::Init(_GD, _EF, _game);
}

void LookAtCamera::SetVP(int _w, int _h)
{
    m_viewport = { (float)_w * m_X, (float)_h * m_Y, (float)_w * m_width, (float)_h * m_height, 0.f, 1.f };
}