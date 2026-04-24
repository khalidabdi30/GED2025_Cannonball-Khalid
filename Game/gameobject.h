#pragma once

//=================================================================
//Base Game Object Class
//=================================================================

#include "bullet/btBulletDynamicsCommon.h"

#include "CommonStates.h"
#include "SimpleMath.h"
#include "../DirectXTK-main/Inc/Effects.h" //this clashes with an SDK file so must explitily state it
#include <string>
#include <fstream>

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

class Camera;
struct ID3D11DeviceContext;
struct GameState;
struct DrawData;
class Game;
class PowerUpHandler;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void Tick(GameState* _GS); //Update per frame
	virtual void Draw(DrawData* _DD) = 0; //Render the object

	virtual void Load(ifstream& _file); //Load object data from file
	virtual void Init(ID3D11Device* _GD, IEffectFactory* _EF, Game* _game); //Initialize any resources needed

	PowerUpHandler* powerUpHandler = nullptr;

	//getters
	Vector3		GetPos() { return m_pos; }
	Vector3		GetScale() { return m_scale; }

	float		GetPitch() { return m_pitch; }
	float		GetYaw() { return m_yaw; }
	float		GetRoll() { return m_roll; }

	string		GetType() { return m_type; }
	string		GetName() { return m_name; }
	Game* GetGame() const { return m_game; }

	const Matrix& GetRotMat() const { return m_rotMat; }
	btRigidBody* GetRigidBody() const { return m_rigidBody; }

	//setters
	void		SetPos(Vector3 _pos);

	void		SetScale(float _scale) { m_scale = _scale * Vector3::One; }
	void		SetScale(float _x, float _y, float _z) { m_scale = Vector3(_x, _y, _z); }
	void		SetScale(Vector3 _scale) { m_scale = _scale; }

	void		SetPitch(float _pitch) { m_pitch = _pitch; }
	void		SetYaw(float _yaw) { m_yaw = _yaw; }
	void		SetRoll(float _roll) { m_roll = _roll; }
	void		SetPitchYawRoll(float _pitch, float _yaw, float _roll) { m_pitch = _pitch; m_yaw = _yaw; m_roll = _roll; }

	virtual void OnCollision(GameObject* _other) {} //called when the RigidBodies of this and another GameObject collide

	void 	Activate();
	void Deactivate();
	void	SetActive(bool _active);
	void 	ToggleActive();

protected:

	Game* m_game = nullptr;

	string m_type, m_name;

	bool m_isActive = true;

	//World transform/matrix of this GO and it components
	Matrix m_worldMat;
	Matrix m_rotMat;
	Matrix m_fudge;

	Vector3 m_pos;
	float m_pitch, m_yaw, m_roll;
	Quaternion m_orientation;
	Vector3 m_scale;

	//bullet stuff
	bool m_hasPhysics = false;
	string m_collisionShapeType = "";
	Vector3 m_collisionShapeSize = Vector3::One;
	float m_mass = 0.0f;
	float m_restitution = 0.0f;

	btCollisionShape* m_colShape = nullptr;
	btRigidBody* m_rigidBody = nullptr;
	btDefaultMotionState* m_MotionState = nullptr;
};
