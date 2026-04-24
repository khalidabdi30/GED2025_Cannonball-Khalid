//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <time.h>
#include <fstream>
#include <iostream>

//Scarle Headers
#include "GameState.h"
#include "FSMState.h"
#include "DrawData2D.h"
#include "GO2DFactory.h"
#include "GameObject2D.h"
#include "stringHelp.h"
#include "DrawData.h"
#include "GOFactory.h"
#include "gameobject.h"
#include "CameraFactory.h"
#include "camera.h"
#include "VBGO.h"
#include "LightFactory.h"
#include "Light.h"
#include "SoundFactory.h"
#include "Sound.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;
using std::ifstream;
using std::cout;
using std::endl;

namespace
{
	std::ifstream OpenManifestStream()
	{
		const char* candidates[] =
		{
			"manifest.txt",
			"../App/manifest.txt",
			"../GED2025/App/manifest.txt",
			"../../GED2025/App/manifest.txt",
			"GED2025/App/manifest.txt"
		};

		for (const char* candidate : candidates)
		{
			std::ifstream file(candidate);
			if (file.is_open())
			{
				return file;
			}
		}

		return std::ifstream();
	}
}

Game::Game() noexcept :
	m_window(nullptr),
	m_outputWidth(800),
	m_outputHeight(600),
	m_featureLevel(D3D_FEATURE_LEVEL_11_1)
{
	InitPhysics();
}

Game::~Game()
{
	if (m_audEngine)
	{
		m_audEngine->Suspend();
	}

	ExitPhysics();
}

// Initialize the Direct3D resources required to run.
bool Game::Initialize(HWND _window, int _width, int _height)
{
	m_window = _window;
	m_outputWidth = std::max(_width, 1);
	m_outputHeight = std::max(_height, 1);

	CreateDevice();

	CreateResources();

	// for 60 FPS fixed timestep update logic
	// Remove this for the default variable timestep mode.	
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);

	// Create other render resources here
	m_states = new CommonStates(m_d3dDevice.Get());

	//set up DirectXTK Effects system
	m_fxFactory = new EffectFactory(m_d3dDevice.Get());
	//Tell the fxFactory to look to the correct build directory to pull stuff in from
	((EffectFactory*)m_fxFactory)->SetDirectory(L"..\\Assets");

	//create DrawData struct and populate its pointers
	m_DD = new DrawData;
	m_DD->m_pd3dImmediateContext = nullptr;
	m_DD->m_states = m_states;
	m_DD->m_cam = m_cam;
	m_DD->m_width = m_outputWidth;
	m_DD->m_height = m_outputHeight;

	//Create DirectXTK spritebatch stuff
	m_DD2D = new DrawData2D();
	m_DD2D->m_Sprites.reset(new SpriteBatch(m_d3dContext.Get()));
	m_DD2D->m_Font.reset(new SpriteFont(m_d3dDevice.Get(), L"../Assets/italic.spritefont"));

	//seed the random number generator
	srand((UINT)time(NULL));

	//set up keyboard and mouse system
	//documentation here: https://github.com/microsoft/DirectXTK/wiki/Mouse-and-keyboard-input
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(_window);
	m_mouse->SetMode(Mouse::MODE_RELATIVE);
	//Hide the mouse pointer
	ShowCursor(false);

	//set up GamePad system
	m_gamepad = std::make_unique<GamePad>();

	//create GameData struct and populate its pointers
	m_GS = new GameState;
	m_GS->m_FSM = FSM_PLAY;

	//set up VBGO render system
	VBGO::VBGOInit(m_d3dDevice.Get());

	//set up Audio system
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags |= AudioEngine_Debug;
#endif
	m_audEngine = std::make_unique<AudioEngine>(eflags);

	//use manifest file to load and then Initialize Game Objects
	Load();
	if (!m_manifestLoaded)
	{
		return false;
	}
	return InitGO();
}

// Executes the basic game loop.
void Game::Tick()
{
	m_timer.Tick([&]()
		{
			Update(m_timer);
		});

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	//how long since the last frame?
	float elapsedTime = float(timer.GetElapsedSeconds());
	m_GS->m_dt = elapsedTime;

	StepPhysics(elapsedTime);
	Collisions();

	//this will update the audio engine but give us chance to do something else if that isn't working
	if (!m_audEngine->Update())
	{
		if (m_audEngine->IsCriticalError())
		{
			// We lost the audio device!
		}
	}
	else
	{
		//update sounds playing
		for (list<Sound*>::iterator it = m_Sounds.begin(); it != m_Sounds.end(); it++)
		{
			(*it)->Tick(m_GS);
		}
	}

	ReadInput();//read keyboard, mouse and gamepad states

	// pressing P flip between the two states
	// TODO: will need something much more complicated for a full FSM
	if (m_GS->m_KBS_tracker.pressed.P)
	{
		if (m_GS->m_FSM == FSM_PAUSE)
		{
			m_GS->m_FSM = FSM_PLAY;
		}
		else if (m_GS->m_FSM == FSM_PLAY)
		{
			m_GS->m_FSM = FSM_PAUSE;
		}
	}

	//upon space bar switch camera
	//see docs here for what's going on: https://github.com/Microsoft/DirectXTK/wiki/Keyboard
	if (m_GS->m_KBS_tracker.pressed.Space)
	{
		NextMainCamera();
	}

	//update current camera
	m_cam->Tick(m_GS);

	for (vector<Camera*>::iterator it = m_Cameras.begin(); it != m_Cameras.end(); ++it)
	{
		//if it can't be main camera, update it here
		//as probably needed
		if (!(*it)->CanBeMainCamera())
		{
			(*it)->Tick(m_GS);
		}
	}

	//update all objects
	for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Tick(m_GS);
	}

	for (list<GameObject2D*>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->Tick(m_GS);
	}

	for (vector<Light*>::iterator it = m_lights.begin(); it != m_lights.end(); it++)
	{
		(*it)->Tick(m_GS);
	}
}

// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	//set immediate context of the graphics device
	m_DD->m_pd3dImmediateContext = m_d3dContext.Get();

	//first draw all the render targets
	//TODO: a lot of repeated code here that could be cleaned up
	//also would be a good start to a render pass system
	for (vector<Camera*>::iterator it = m_Cameras.begin(); it != m_Cameras.end(); ++it)
	{
		if ((*it)->GetType() == "RENDERTARGET")
		{
			//clears out textures so don't try to draw to this one
			//whilst still "connected" to read from
			ID3D11ShaderResourceView* srvs[] = { NULL };
			m_d3dContext->PSSetShaderResources(0, 1, srvs);

			//set camera in DrawData to this render target camera
			Camera* currCam = m_DD->m_cam;
			m_DD->m_cam = (*it);

			//update the constant buffer for the rendering of VBGOs
			VBGO::UpdateConstantBuffer(m_DD);

			//prerender for this RenderTarget cameras also includes setting its render target and depth stencil view
			(*it)->PreRender(m_DD);

			//Draw 3D Game Objects
			for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
			{
				(*it)->Draw(m_DD);
			}

			// Draw sprite batch stuff 
			m_DD2D->m_Sprites->Begin(DirectX::SpriteSortMode_Deferred, m_states->NonPremultiplied());
			for (list<GameObject2D*>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
			{
				//don't draw any 2D objects that are also RENDERTARGET types
				//in a render target pass
				if ((*it)->GetType() != "RENDERTARGET")
				{
					(*it)->Draw(m_DD2D);
				}
			}
			m_DD2D->m_Sprites->End();

			//drawing text screws up the Depth Stencil State, this puts it back again!
			m_d3dContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);

			//restore main camera to DrawData
			m_DD->m_cam = currCam;
		}
	}

	//main clear puts main back buffer and depth stencil view back as targets
	Clear();

	std::vector<Camera*> splitCams;
	for (auto it = m_Cameras.begin(); it != m_Cameras.end(); ++it)
	{
		Camera* c = *it;
		if (c && c->GetType() == "LOOKAT")
		{
			if (c->GetName().find("LOOKCAM") == 0)
				splitCams.push_back(c);
		}
	}

	if (splitCams.empty())
	{
		VBGO::UpdateConstantBuffer(m_DD);
		m_DD->m_cam->PreRender(m_DD);

		for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); ++it)
		{
			(*it)->Draw(m_DD);
		}
	}
	else
	{
		Camera* savedMain = m_DD->m_cam;

		for (size_t i = 0; i < splitCams.size(); ++i)
		{
			m_DD->m_cam = splitCams[i];

			VBGO::UpdateConstantBuffer(m_DD);
			m_DD->m_cam->PreRender(m_DD);

			for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); ++it)
			{
				(*it)->Draw(m_DD);
			}
		}

		m_DD->m_cam = savedMain;
	}

	// Draw sprite batch stuff 
	m_DD2D->m_Sprites->Begin(DirectX::SpriteSortMode_Deferred, m_states->NonPremultiplied());
	for (list<GameObject2D*>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->Draw(m_DD2D);
	}
	m_DD2D->m_Sprites->End();

	//drawing text screws up the Depth Stencil State, this puts it back again!
	m_d3dContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);

	Present();
}

void Game::Collisions()
{
	int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();

	if (numManifolds > 0)
	{
		for (int i = 0; i < numManifolds; i++)
		{
			btPersistentManifold* contactManifold = m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
			btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
			btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());
			int numContacts = contactManifold->getNumContacts();
			if (numContacts > 0)
			{
				//get GameObject pointers from the collision objects
				GameObject* GOA = (GameObject*)obA->getUserPointer();
				GameObject* GOB = (GameObject*)obB->getUserPointer();
				if (GOA && GOB)
				{
					GOA->OnCollision(GOB);
					GOB->OnCollision(GOA);
				}
			}
		}
	}
}

// Helper method to clear the back buffers.
void Game::Clear()
{
	// Clear the views.
	m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
	m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	HRESULT hr = m_swapChain->Present(1, 0);

	// If the device was reset we must completely reinitialize the renderer.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		OnDeviceLost();
	}
	else
	{
		DX::ThrowIfFailed(hr);
	}
}

// Message handlers
void Game::OnActivated()
{
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).
	m_audEngine->Suspend();
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();
	m_audEngine->Resume();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_window)
		return;

	m_outputWidth = std::max(width, 1);
	m_outputHeight = std::max(height, 1);

	if (m_DD)
	{
		m_DD->m_height = m_outputHeight;
		m_DD->m_width = m_outputWidth;
	}

	CreateResources();

	// TODO: Game window is being resized.

}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = 800;
	height = 600;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
	UINT creationFlags = 0;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	static const D3D_FEATURE_LEVEL featureLevels[] =
	{
		// TODO: Modify for supported Direct3D feature levels
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	// Create the DX11 API device object, and get a corresponding context.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	DX::ThrowIfFailed(D3D11CreateDevice(
		nullptr,                            // specify nullptr to use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		featureLevels,
		static_cast<UINT>(std::size(featureLevels)),
		D3D11_SDK_VERSION,
		device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
		&m_featureLevel,                    // returns feature level of device created
		context.ReleaseAndGetAddressOf()    // returns the device immediate context
	));

#ifndef NDEBUG
	ComPtr<ID3D11Debug> d3dDebug;
	if (SUCCEEDED(device.As(&d3dDebug)))
	{
		ComPtr<ID3D11InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				// TODO: Add more message IDs here as needed.
			};
			D3D11_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}
	}
#endif

	DX::ThrowIfFailed(device.As(&m_d3dDevice));
	DX::ThrowIfFailed(context.As(&m_d3dContext));

	// TODO: Initialize device dependent objects here (independent of window size).
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
	// Clear the previous window size specific context.
	m_d3dContext->OMSetRenderTargets(0, nullptr, nullptr);
	m_renderTargetView.Reset();
	m_depthStencilView.Reset();
	m_d3dContext->Flush();

	const UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
	const UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
	const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	constexpr UINT backBufferCount = 2;

	// If the swap chain already exists, resize it, otherwise create one.
	if (m_swapChain)
	{
		HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// If the device was removed for any reason, a new device and swap chain will need to be created.
			OnDeviceLost();

			// Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method
			// and correctly set up the new device.
			return;
		}
		else
		{
			DX::ThrowIfFailed(hr);
		}
	}
	else
	{
		// First, retrieve the underlying DXGI Device from the D3D Device.
		ComPtr<IDXGIDevice1> dxgiDevice;
		DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

		// Identify the physical adapter (GPU or card) this device is running on.
		ComPtr<IDXGIAdapter> dxgiAdapter;
		DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

		// And obtain the factory object that created it.
		ComPtr<IDXGIFactory2> dxgiFactory;
		DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

		// Create a descriptor for the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = backBufferWidth;
		swapChainDesc.Height = backBufferHeight;
		swapChainDesc.Format = backBufferFormat;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = backBufferCount;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed = TRUE;

		// Create a SwapChain from a Win32 window.
		DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
			m_d3dDevice.Get(),
			m_window,
			&swapChainDesc,
			&fsSwapChainDesc,
			nullptr,
			m_swapChain.ReleaseAndGetAddressOf()
		));

		// This template does not support exclusive full screen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
		DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
	}

	// Obtain the back buffer for this window which will be the final 3D render target.
	ComPtr<ID3D11Texture2D> backBuffer;
	DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

	// Create a view interface on the render target to use on bind.
	DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

	// Allocate a 2-D surface as the depth/stencil buffer and
	// create a DepthStencil view on this surface to use on bind.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

	ComPtr<ID3D11Texture2D> depthStencil;
	DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

	DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), nullptr, m_depthStencilView.ReleaseAndGetAddressOf()));

	// TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.

	m_depthStencilView.Reset();
	m_renderTargetView.Reset();
	m_swapChain.Reset();
	m_d3dContext.Reset();
	m_d3dDevice.Reset();

	CreateDevice();

	CreateResources();
}

void Game::Load()
{
	m_manifestLoaded = false;
	ifstream manifest = OpenManifestStream();
	if (!manifest.is_open())
	{
		std::cerr << "Failed to open manifest.txt from known locations." << std::endl;
		return;
	}
	m_manifestLoaded = true;

	string dummy;

	//Load Game Objects 2D
	int numGO2D;
	StringHelp::Int(manifest, "GAMEOBJECTS2D", numGO2D);
	for (int i = 0; i < numGO2D; i++)
	{

		//skip {
		manifest.ignore(256, '\n');
		cout << "{\n";

		string type;
		StringHelp::String(manifest, "TYPE", type);
		GameObject2D* newGO2D = GO2DFactory::makeNewGO2D(type);
		newGO2D->Load(manifest);

		m_GameObject2Ds.push_back(newGO2D);

		//skip }
		manifest.ignore(256, '\n');
		cout << "}\n";
	}


	cout << endl << endl;

	//Load Cameras
	StringHelp::Int(manifest, "CAMERAS", m_numCameras);
	for (int i = 0; i < m_numCameras; i++)
	{

		//skip {
		manifest.ignore(256, '\n');
		cout << "{\n";

		string type;
		StringHelp::String(manifest, "TYPE", type);
		Camera* newCam = CameraFactory::makeNewCam(type);
		newCam->Load(manifest);

		m_Cameras.push_back(newCam);

		//skip }
		manifest.ignore(256, '\n');
		cout << "}\n";
	}


	cout << endl << endl;

	//Load Lights
	StringHelp::Int(manifest, "LIGHTS", m_numLights);
	for (int i = 0; i < m_numLights; i++)
	{

		//skip {
		manifest.ignore(256, '\n');
		cout << "{\n";

		string type;
		StringHelp::String(manifest, "TYPE", type);
		Light* newLight = LightFactory::makeNewLight(type);
		newLight->Load(manifest);

		m_lights.push_back(newLight);

		//skip }
		manifest.ignore(256, '\n');
		cout << "}\n";
	}

	m_DD->m_light = m_lights.empty() ? nullptr : m_lights[0]; //currently only use one light so just use the first one

	cout << endl << endl;

	//Load 3D Game Objects
	int numGO;
	StringHelp::Int(manifest, "GAMEOBJECTS", numGO);
	for (int i = 0; i < numGO; i++)
	{

		//skip {
		manifest.ignore(256, '\n');
		cout << "{\n";

		string type;
		StringHelp::String(manifest, "TYPE", type);
		GameObject* newGO = GOFactory::makeNewGO(type);
		newGO->Load(manifest);

		m_GameObjects.push_back(newGO);

		//skip }
		manifest.ignore(256, '\n');
		cout << "}\n";
	}

	cout << endl << endl;

	//Load Sounds
	StringHelp::Int(manifest, "SOUNDS", m_numSounds);
	for (int i = 0; i < m_numSounds; i++)
	{

		//skip {
		manifest.ignore(256, '\n');
		cout << "{\n";

		string type;
		StringHelp::String(manifest, "TYPE", type);
		Sound* newSound = SoundFactory::makeNewSound(type);
		newSound->Load(manifest);

		m_Sounds.push_back(newSound);

		//skip }
		manifest.ignore(256, '\n');
		cout << "}\n";
	}

	manifest.close();
}

bool Game::InitGO()
{
	if (m_Cameras.empty())
	{
		std::cerr << "No cameras were loaded from the manifest. Startup aborted." << std::endl;
		return false;
	}

	int count = 0;
	for (vector<Camera*>::iterator it = m_Cameras.begin(); it != m_Cameras.end(); ++it)
	{
		(*it)->Init(m_d3dDevice.Get(), m_fxFactory, this);

		//if a camera is called MAIN
		//this will be the starting camera used
		if ((*it)->GetName() == "MAIN")
		{
			m_cam = (*it);
			m_useCameraIndex = count;
			break;
		}
		count++;
	}

	//if no MAIN camera just use the first one that can be the main camera
	if (!m_cam)
	{
		m_cam = (*m_Cameras.begin());
		m_useCameraIndex = 0;
	}
	if (!m_cam)
	{
		std::cerr << "No valid main camera was created. Startup aborted." << std::endl;
		return false;
	}
	if (!m_cam->CanBeMainCamera())
	{
		NextMainCamera();
	}
	if (!m_cam)
	{
		std::cerr << "Unable to select a usable main camera. Startup aborted." << std::endl;
		return false;
	}

	m_DD->m_cam = m_cam;

	//initialize all game objects
	//this is where we can set up links between objects if needed
	//for example a camera following a player object
	//by giving the name of the camera to the player object in the manifest file
	//and then here getting the pointer to that camera and giving it to the player object
	for (list<GameObject2D*>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->Init(m_d3dDevice.Get(), this);
	}

	for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Init(m_d3dDevice.Get(), m_fxFactory, this);
	}

	for (list<Sound*>::iterator it = m_Sounds.begin(); it != m_Sounds.end(); it++)
	{
		(*it)->Init(m_audEngine.get());
	}

	for (vector<Light*>::iterator it = m_lights.begin(); it != m_lights.end(); it++)
	{
		(*it)->Init(m_d3dDevice.Get(), m_fxFactory, this);
	}

	return true;
}

GameObject* Game::GetGOByName(string _name)
{
	GameObject* found = nullptr;
	for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		if (_name == (*it)->GetName())
		{
			found = (*it);
			return found;
		}
	}
	printf("UNKNOWN GAMEOBJECT NAME : %s\n", _name.c_str());
	return found;
}

GameObject2D* Game::GetGO2DByName(string _name)
{
	GameObject2D* found = nullptr;
	for (list<GameObject2D*>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		if (_name == (*it)->GetName())
		{
			found = (*it);
			return found;
		}
	}
	printf("UNKNOWN GAMEOBJECT NAME : %s\n", _name.c_str());
	return found;
}

Camera* Game::GetCamByName(string _name)
{
	Camera* found = nullptr;
	for (vector<Camera*>::iterator it = m_Cameras.begin(); it != m_Cameras.end(); it++)
	{
		if (_name == (*it)->GetName())
		{
			found = (*it);
			return found;
		}
	}
	printf("UNKNOWN CAMERA NAME : %s\n", _name.c_str());
	return found;
}

Light* Game::GetLightByName(string _name)
{
	Light* found = nullptr;
	for (vector<Light*>::iterator it = m_lights.begin(); it != m_lights.end(); it++)
	{
		if (_name == (*it)->GetName())
		{
			found = (*it);
			return found;
		}
	}
	printf("UNKNOWN LIGHT NAME : %s\n", _name.c_str());
	return found;
}

Sound* Game::GetSoundByName(string _name)
{
	Sound* found = nullptr;
	for (list<Sound*>::iterator it = m_Sounds.begin(); it != m_Sounds.end(); it++)
	{
		if (_name == (*it)->GetName())
		{
			found = (*it);
			return found;
		}
	}
	printf("UNKNOWN SOUND NAME : %s\n", _name.c_str());
	return found;
}

void Game::ReadInput()
{
	m_GS->m_KBS = m_keyboard->GetState();
	m_GS->m_KBS_tracker.Update(m_GS->m_KBS);
	//quit game on hitting escape
	if (m_GS->m_KBS.Escape)
	{
		ExitGame();
	}

	m_GS->m_MS = m_mouse->GetState();

	//lock the cursor to the centre of the window
	RECT window;
	GetWindowRect(m_window, &window);
	SetCursorPos((window.left + window.right) >> 1, (window.bottom + window.top) >> 1);

	for (int i = 0; i < 4; i++)
	{
		m_GS->m_GPS[i] = m_gamepad->GetState(i);
	}
}

void Game::NextMainCamera()
{
	if (m_numCameras <= 0 || m_Cameras.empty())
	{
		m_cam = nullptr;
		return;
	}

	do
	{
		m_useCameraIndex = (++m_useCameraIndex) % m_numCameras;
		m_cam = m_Cameras[m_useCameraIndex];
		m_DD->m_cam = m_cam;
	} while (!m_cam->CanBeMainCamera());

	cout << "changing camera " << m_useCameraIndex << endl;
}

void Game::InitPhysics()
{
	/// collision configuration contains default setup for memory , collision setup . Advanced
	//users can create their own configuration .
	m_collisionConfiguration = new btDefaultCollisionConfiguration();

	/// use the default collision dispatcher . For parallel processing you can use a different
	//	dispatcher(see Extras / BulletMultiThreaded)
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	/// btDbvtBroadphase is a good general purpose broad phase . You can also try out
	//	btAxis3Sweep .
	m_overlappingPairCache = new btDbvtBroadphase();

	/// the default constraint solver . For parallel processing you can use a different solver
   //(see Extras / BulletMultiThreaded)
	m_solver = new btSequentialImpulseConstraintSolver;

	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);

	m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

void Game::StepPhysics(float _dt)
{
	m_dynamicsWorld->stepSimulation(_dt, 10);
}

void Game::ExitPhysics()
{
	//remove the constraints from the dynamics world and delete them
	for (int i = m_dynamicsWorld->getNumConstraints() - 1; i >= 0; i--)
	{
		btTypedConstraint* constraint = m_dynamicsWorld->getConstraint(i);
		m_dynamicsWorld->removeConstraint(constraint);
		delete constraint;
	}

	// remove the rigid bodies from the dynamics world and delete them
	for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	// delete collision shapes
	for (int j = 0; j < m_collisionShapes.size(); j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		m_collisionShapes[j] = 0;
		delete shape;
	}

	// delete dynamics world
	delete m_dynamicsWorld;
	m_dynamicsWorld = nullptr;

	// delete solver
	delete m_solver;
	m_solver = nullptr;

	// delete broadphase
	delete m_overlappingPairCache;
	m_overlappingPairCache = nullptr;

	// delete dispatcher
	delete m_dispatcher;
	m_dispatcher = nullptr;

	// delete collision configuration
	delete m_collisionConfiguration;
	m_collisionConfiguration = nullptr;

	// next line is optional : it will be cleared by the destructor when the array goes out of	scope
	m_collisionShapes.clear();
}
