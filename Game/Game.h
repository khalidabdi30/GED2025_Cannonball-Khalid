//
// Game.h
//

#pragma once
#include "pch.h"

#include "bullet/btBulletDynamicsCommon.h"

#include "StepTimer.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "GamePad.h"
#include "../DirectXTK-main/Inc/Effects.h" //this clashes with an SDK file so must explitily state it
#include "CommonStates.h"
#include <list>
#include <vector>
#include <string>
#include "Audio.h"

using std::list;
using std::vector;
using std::string;

// Forward declarations
struct GameState;
struct DrawData2D;
class GameObject2D;
class Camera;
class GameObject;
struct DrawData;
class Light;
class Sound;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:

	Game() noexcept;
	~Game();

	Game(Game&&) = default;
	Game& operator= (Game&&) = default;

	Game(Game const&) = delete;
	Game& operator= (Game const&) = delete;

	// Initialization and management
	bool Initialize(HWND _window, int _width, int _height);

	// Basic game loop
	void Tick();

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowSizeChanged(int _width, int _height);

	// Properties
	void GetDefaultSize(int& _width, int& _height) const noexcept;

	//get things in the scene by their names
	GameObject* GetGOByName(string _name);
	GameObject2D* GetGO2DByName(string _name);
	Camera* GetCamByName(string _name);
	Light* GetLightByName(string _name);
	Sound* GetSoundByName(string _name);
	const list<GameObject*>& GetGameObjects() const { return m_GameObjects; }

	//add things to the scene at runtime as opposed to loading from the manifest file
	void AddGameObject(GameObject* _GO) { m_GameObjects.push_back(_GO); }
	void AddGameObject2D(GameObject2D* _GO2D) { m_GameObject2Ds.push_back(_GO2D); }

	int GetWidth() { return m_outputWidth; }
	int GetHeight() { return m_outputHeight; }

private:

	void Update(DX::StepTimer const& timer);
	void Render();

	void Clear();
	void Present();

	void CreateDevice();
	void CreateResources();

	void OnDeviceLost();

	// Device resources.
	HWND                                            m_window;
	int                                             m_outputWidth;
	int                                             m_outputHeight;

	D3D_FEATURE_LEVEL                               m_featureLevel;
	Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;

	Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

	// Rendering loop timer.
	DX::StepTimer                                   m_timer;

	Camera* m_cam = NULL; //principle camera
	int m_useCameraIndex = 0;

	//required for the CMO model rendering system
	DirectX::CommonStates* m_states;
	DirectX::IEffectFactory* m_fxFactory = NULL;

	//Data to be shared to all 3D Game Objects as they are drawn
	DrawData* m_DD = NULL;

	//Scarle Added stuff
	GameState* m_GS = NULL;			//State to be shared to all Game Objects as they are ticked
	void Load(); //Load Game Objects from manifest file
	bool InitGO();//Initialize Game Objects
	bool m_manifestLoaded = false;

	list<GameObject*> m_GameObjects; //data structure to hold pointers to the 3D Game Objects
	list<GameObject2D*> m_GameObject2Ds;//ditto 2D objects
	vector<Camera*> m_Cameras;//ditto 3D cameras
	int m_numCameras = 0;
	vector<Light*> m_lights;//ditto 3D lights for VB system
	int m_numLights = 0;

	//basic keyboard and mouse input system
	void ReadInput(); //Get current Mouse and Keyboard states
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;

	//ditto for Game Pad devices
	std::unique_ptr<DirectX::GamePad> m_gamepad;
	//you can find more info about this system here: https://github.com/microsoft/DirectXTK/wiki/GamePad

	//2D Render System
	DrawData2D* m_DD2D;

	//Audio Engine
	//This uses a simple system, but a better pipeline can be used using Wave Banks
	//See here: https://github.com/Microsoft/DirectXTK/wiki/Creating-and-playing-sounds Using wave banks Section
	std::unique_ptr<DirectX::AudioEngine> m_audEngine;
	list<Sound*>m_Sounds;
	int m_numSounds = 0;

	void NextMainCamera(); //Switch to the next camera in the list that can be the main camera

	//Bullet Physics
	void InitPhysics();
	void StepPhysics(float _dt);
	void ExitPhysics();

	void Collisions(); //Check for collisions between rigid bodies in the physics world and send messages to the Game Objects

	/// collision configuration contains default setup for memory , collision setup . Advanced
	//users can create their own configuration .
	btDefaultCollisionConfiguration* m_collisionConfiguration = nullptr;

	/// use the default collision dispatcher . For parallel processing you can use a different
	//	dispatcher(see Extras / BulletMultiThreaded)
	btCollisionDispatcher* m_dispatcher = nullptr;

	/// btDbvtBroadphase is a good general purpose broad phase . You can also try out
	//	btAxis3Sweep .
	btBroadphaseInterface* m_overlappingPairCache = nullptr;

	/// the default constraint solver . For parallel processing you can use a different solver
	//(see Extras / BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* m_solver = nullptr;

	btDiscreteDynamicsWorld* m_dynamicsWorld = nullptr;

	///keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

public:
	//getters for Bullet Physics members
	btDiscreteDynamicsWorld* GetDynamicsWorld() { return m_dynamicsWorld; }
	btAlignedObjectArray<btCollisionShape*>* GetCollisionShapes() { return &m_collisionShapes; }

};
