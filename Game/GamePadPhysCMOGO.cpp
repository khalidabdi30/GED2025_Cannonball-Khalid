#include "pch.h"
#include "GamePadPhysCMOGO.h"
#include "stringHelp.h"
#include "PowerUpHandler.h"
#include "gameobject.h"
#include "Game.h"
#include "GameState.h"

namespace
{
	const Vector3 kNpcWaypoints[] =
	{
		Vector3(12.0f, 0.0f, -45.0f),
		Vector3(18.0f, 0.0f, -20.0f),
		Vector3(18.0f, 0.0f, 25.0f),
		Vector3(-33.0f, 0.0f, 25.0f),
		Vector3(-36.0f, 0.0f, -20.0f),
		Vector3(-36.0f, 0.0f, -60.0f),
		Vector3(-10.0f, 0.0f, -73.0f),
		Vector3(18.0f, 0.0f, -60.0f)
	};

	constexpr size_t kWaypointCount = sizeof(kNpcWaypoints) / sizeof(kNpcWaypoints[0]);
}

GamePadPhysCMOGO::GamePadPhysCMOGO()
{
    m_type = "GAMEPAD_PHYS_CMOGO";
    m_powerUpHandler = new PowerUpHandler();
}

GamePadPhysCMOGO::~GamePadPhysCMOGO()
{
    delete m_powerUpHandler;
    m_powerUpHandler = nullptr;
}

void GamePadPhysCMOGO::Init(ID3D11Device* device, IEffectFactory* fxFactory, Game* game)
{
    PhysCMOGO::Init(device, fxFactory, game);

    if (!m_rigidBody)
    {
        return;
    }

    // Keep the racers upright so wall collisions feel like kart impacts instead of tumbles.
    m_rigidBody->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
    m_rigidBody->setFriction(1.2f);
    m_rigidBody->setRollingFriction(0.2f);
    m_rigidBody->setSpinningFriction(0.3f);
    m_rigidBody->setDamping(0.08f, 0.88f);
    m_rigidBody->setSleepingThresholds(btScalar(0.0f), btScalar(0.0f));
    m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
    m_rigidBody->setCcdMotionThreshold(btScalar(0.35f));
    m_rigidBody->setCcdSweptSphereRadius(btScalar(0.45f));
}

void GamePadPhysCMOGO::TickNpc(GameState* gs)
{
    if (!gs || !m_rigidBody)
    {
        return;
    }

    const Vector3 currentPos = m_pos;
    Vector3 target = kNpcWaypoints[m_aiWaypointIndex % kWaypointCount];
    Vector3 toTarget = target - currentPos;
    toTarget.y = 0.0f;
    float targetDistanceSq = toTarget.LengthSquared();

    const float reachDistanceSq = 9.0f;
    if (targetDistanceSq <= reachDistanceSq)
    {
        m_aiWaypointIndex = (m_aiWaypointIndex + 1) % kWaypointCount;
        target = kNpcWaypoints[m_aiWaypointIndex % kWaypointCount];
        toTarget = target - currentPos;
        toTarget.y = 0.0f;
        targetDistanceSq = toTarget.LengthSquared();
    }

    if (targetDistanceSq < 0.0001f)
    {
        return;
    }

    if (m_aiPowerUpUseTimer > 0.0f)
    {
        m_aiPowerUpUseTimer -= gs->m_dt;
    }

    toTarget.Normalize();

    Vector3 forward = Vector3::Transform(Vector3::UnitZ, m_rotMat);
    forward.y = 0.0f;
    if (forward.LengthSquared() < 0.0001f)
    {
        forward = Vector3::UnitZ;
    }
    else
    {
        forward.Normalize();
    }

    float speedMulti = m_powerUpHandler ? m_powerUpHandler->GetSpeedMulti() : 1.0f;
    float controlFlip = m_powerUpHandler ? m_powerUpHandler->GetControlFlip() : 1.0f;
    float turnMulti = m_powerUpHandler ? m_powerUpHandler->GetTurnMulti() : 1.0f;

    if (m_powerUpHandler && m_powerUpHandler->HasPowerUp() && m_aiPowerUpUseTimer <= 0.0f)
    {
        m_powerUpHandler->UsePowerUp(this);
        m_aiPowerUpUseTimer = 2.0f;
    }

    const btVector3 linearVelocity = m_rigidBody->getLinearVelocity();
    const float planarSpeed = Vector3(linearVelocity.getX(), 0.0f, linearVelocity.getZ()).Length();
    if (planarSpeed < 1.25f && targetDistanceSq > 36.0f)
    {
        m_stuckTimer += gs->m_dt;
    }
    else
    {
        m_stuckTimer = 0.0f;
    }

    if (m_stuckTimer >= 0.9f)
    {
        m_wallRecoverTimer = 0.6f;
        m_wallRecoverDir = -forward;
        m_wallRecoverDir.y = 0.0f;
        if (m_wallRecoverDir.LengthSquared() < 0.001f)
        {
            m_wallRecoverDir = Vector3::Backward;
        }
        else
        {
            m_wallRecoverDir.Normalize();
        }
        m_stuckTimer = 0.0f;
    }

    Vector3 driveDir = toTarget;
    const float facingDot = forward.Dot(toTarget);
    if (m_wallRecoverTimer > 0.0f)
    {
        m_wallRecoverTimer -= gs->m_dt;
        driveDir = m_wallRecoverDir;
    }

    float throttleScale = 1.0f;
    if (m_wallRecoverTimer <= 0.0f && facingDot < -0.2f)
    {
        throttleScale = 0.4f;
    }

    Vector3 move = driveDir * (m_baseMoveForce * 1.8f * speedMulti * throttleScale);
    m_rigidBody->applyCentralForce(btVector3(move.x, move.y, move.z));

    const float turnError = forward.Cross(driveDir).y;
    const float turnForce = (m_wallRecoverTimer > 0.0f ? 80.0f : 60.0f);
    m_rigidBody->applyTorque(btVector3(0.0f, turnError * turnForce * turnMulti * controlFlip, 0.0f));

    btVector3 v = m_rigidBody->getLinearVelocity();
    const btScalar maxSpeed = btScalar(34.0f * speedMulti);
    if (v.length() > maxSpeed)
    {
        m_rigidBody->setLinearVelocity(v.normalized() * maxSpeed);
    }

    m_rigidBody->setDamping(0.08f, 0.88f);
    m_rigidBody->activate(true);
}

void GamePadPhysCMOGO::Tick(GameState* _GS)
{
    if (!_GS) return;
    if (!m_isActive) return;
    if (_GS->m_FSM == FSM_PAUSE) return;

    if (m_powerUpHandler)
        m_powerUpHandler->Tick(_GS->m_dt);

    if (!m_hasPhysics || !m_rigidBody)
    {
        PhysCMOGO::Tick(_GS);
        return;
    }

    m_rigidBody->activate(true);

    const bool connected = (m_device >= 0 && m_device < 4 && _GS->m_GPS[m_device].connected);
    if (connected)
    {
        auto state = _GS->m_GPS[m_device];

        // Use powerup on A
        static bool prevA[4] = { false,false,false,false };
        bool aNow = state.buttons.a;
        if (aNow && !prevA[m_device] && m_powerUpHandler)
            m_powerUpHandler->UsePowerUp(this);
        prevA[m_device] = aNow;

        float speedMulti = m_powerUpHandler ? m_powerUpHandler->GetSpeedMulti() : 1.0f;
        float controlFlip = m_powerUpHandler ? m_powerUpHandler->GetControlFlip() : 1.0f;
        float turnMulti = m_powerUpHandler ? m_powerUpHandler->GetTurnMulti() : 1.0f;

        Vector3 forward = Vector3::Transform(Vector3::UnitZ, m_rotMat);
        forward.y = 0.0f;
        if (forward.LengthSquared() > 0.0001f)
            forward.Normalize();
        else
            forward = Vector3::UnitZ;

        float throttle = -state.thumbSticks.leftY * controlFlip;      // forward
        Vector3 move = forward * (throttle * m_baseMoveForce * speedMulti);
        m_rigidBody->applyCentralForce(btVector3(move.x, move.y, move.z));

        float steer = state.thumbSticks.rightX;
        const float deadzone = 0.15f;
        if (fabsf(steer) < deadzone) steer = 0.0f;

        m_rigidBody->applyTorque(btVector3(0.0f, steer * -20.0f * turnMulti * controlFlip, 0.0f));

        m_rigidBody->setDamping(0.2f, 0.9f);
    }
    else
    {
        TickNpc(_GS);
    }

    GameObject::Tick(_GS);
}


void GamePadPhysCMOGO::Load(ifstream& _file)
{
    PhysCMOGO::Load(_file);
    StringHelp::Int(_file, "DEVICE", m_device);
    m_aiWaypointIndex = static_cast<size_t>((m_device >= 0) ? m_device : 0);
}

void GamePadPhysCMOGO::OnCollision(GameObject* other)
{
    if (!other) return;
    if (!m_rigidBody)
    {
        return;
    }

    const std::string otherName = other->GetName();
    if (otherName.rfind("TRACK_COL_", 0) == 0 || other->GetType() == "WORLD_LIMIT")
    {
        Vector3 pushDir = m_pos - other->GetPos();
        pushDir.y = 0.0f;
        if (pushDir.LengthSquared() < 0.001f)
        {
            pushDir = -Vector3::Transform(Vector3::UnitZ, m_rotMat);
            pushDir.y = 0.0f;
        }

        if (pushDir.LengthSquared() > 0.001f)
        {
            pushDir.Normalize();
            m_wallRecoverDir = pushDir;
        }
        m_wallRecoverTimer = 0.55f;

        btVector3 slowed = m_rigidBody->getLinearVelocity() * btScalar(0.45f);
        m_rigidBody->setLinearVelocity(slowed);
        m_rigidBody->applyCentralImpulse(btVector3(pushDir.x * 3.5f, 0.0f, pushDir.z * 3.5f));
        m_rigidBody->activate(true);
    }
    else if (other->GetType() == "GAMEPAD_PHYS_CMOGO")
    {
        Vector3 separate = m_pos - other->GetPos();
        separate.y = 0.0f;
        if (separate.LengthSquared() > 0.001f)
        {
            separate.Normalize();
            m_rigidBody->applyCentralImpulse(btVector3(separate.x * 1.2f, 0.0f, separate.z * 1.2f));
            m_rigidBody->activate(true);
        }
    }
}
