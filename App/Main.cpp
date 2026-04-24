//
// Main.cpp
// Entry point for the actual application
// This creates the window, initializes the Game class and runs the message pump linking to Windows
//

#include "pch.h"
#include "../Game/Game.h"
#include "bulletLibs.h"
#include <string>
#include <vector>

using namespace DirectX;

#ifdef __clang__
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

#pragma warning(disable : 4061)

namespace
{
	std::unique_ptr<Game> g_game;

	bool PathExists(const std::wstring& path)
	{
		const DWORD attributes = GetFileAttributesW(path.c_str());
		return attributes != INVALID_FILE_ATTRIBUTES;
	}

	bool TrySetStartupWorkingDirectory()
	{
		wchar_t modulePath[MAX_PATH] = {};
		if (!GetModuleFileNameW(nullptr, modulePath, MAX_PATH))
		{
			return false;
		}

		std::wstring exeDir(modulePath);
		const size_t lastSlash = exeDir.find_last_of(L"\\/");
		if (lastSlash == std::wstring::npos)
		{
			return false;
		}

		exeDir.resize(lastSlash);

		const std::vector<std::wstring> candidates =
		{
			exeDir + L"\\App",
			exeDir,
			exeDir + L"\\..\\App",
			exeDir + L"\\..\\..\\App",
			exeDir + L"\\..\\..\\..\\App"
		};

		for (const std::wstring& candidate : candidates)
		{
			const std::wstring manifestPath = candidate + L"\\manifest.txt";
			const std::wstring assetsPath = candidate + L"\\..\\Assets";
			if (PathExists(manifestPath) && PathExists(assetsPath) && SetCurrentDirectoryW(candidate.c_str()))
			{
				return true;
			}
		}

		return false;
	}
}

LPCWSTR g_szAppName = L"Game";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ExitGame() noexcept;

// Indicates to hybrid graphics systems to prefer the discrete part by default
extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPWSTR _lpCmdLine, _In_ int _nCmdShow)
{
	_nCmdShow;
#ifdef _DEBUG
	//add a Debug Console to which cout is directed whilst in DEBUG
	if (AllocConsole()) {
		FILE* stream;
		freopen_s(&stream, "CONOUT$", "w", stdout);
		SetConsoleTitle(L"Debug Console");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

		HWND consoleWindow = GetConsoleWindow();

		SetWindowPos(consoleWindow, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
#endif

	UNREFERENCED_PARAMETER(_hPrevInstance);
	UNREFERENCED_PARAMETER(_lpCmdLine);

	if (!XMVerifyCPUSupport())
		return 1;

	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		return 1;

	TrySetStartupWorkingDirectory();

	g_game = std::make_unique<Game>();

	// Register class and create window
	{
		// Register class
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEXW);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = _hInstance;
		wcex.hIcon = LoadIconW(_hInstance, L"IDI_ICON");
		wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wcex.lpszClassName = L"GameWindowClass";
		wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
		if (!RegisterClassExW(&wcex))
			return 1;

		// Create window
		int w, h;
		g_game->GetDefaultSize(w, h);

		RECT rc = { 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };

		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		HWND hwnd =

#ifdef _DEBUG
			//Standard window in Debug
			CreateWindowExW(0, L"GameWindowClass", g_szAppName, WS_OVERLAPPEDWINDOW,
#else
				//fullscreen for release
				CreateWindowExW(WS_EX_TOPMOST, L"GameWindowClass", g_szAppName, WS_POPUP,
#endif
					CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
					nullptr, nullptr, _hInstance,
					g_game.get());

		if (!hwnd)
			return 1;

#ifdef _DEBUG
		ShowWindow(hwnd, _nCmdShow);
		SetWindowPos(hwnd, 0, 600, 100, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
#else
		ShowWindow(hwnd, SW_SHOWMAXIMIZED);
#endif

		GetClientRect(hwnd, &rc);

		if (!g_game->Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top))
			return 1;
	}

	// Main message loop
	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_game->Tick();
		}
	}

	g_game.reset();

	return static_cast<int>(msg.wParam);
}

// Windows procedure
LRESULT CALLBACK WndProc(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
	static bool s_in_sizemove = false;
	static bool s_in_suspend = false;
	static bool s_minimized = false;
	static bool s_fullscreen = false;
	// TODO: Set s_fullscreen to true if defaulting to full screen.

	auto game = reinterpret_cast<Game*>(GetWindowLongPtr(_hWnd, GWLP_USERDATA));

	switch (_message)
	{
	case WM_CREATE:
		if (_lParam)
		{
			auto params = reinterpret_cast<LPCREATESTRUCTW>(_lParam);
			SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(params->lpCreateParams));
		}
		break;

	case WM_PAINT:
		if (s_in_sizemove && game)
		{
			game->Tick();
		}
		else
		{
			PAINTSTRUCT ps;
			std::ignore = BeginPaint(_hWnd, &ps);
			EndPaint(_hWnd, &ps);
		}
		break;

	case WM_SIZE:
		if (_wParam == SIZE_MINIMIZED)
		{
			if (!s_minimized)
			{
				s_minimized = true;
				if (!s_in_suspend && game)
					game->OnSuspending();
				s_in_suspend = true;
			}
		}
		else if (s_minimized)
		{
			s_minimized = false;
			if (s_in_suspend && game)
				game->OnResuming();
			s_in_suspend = false;
		}
		else if (!s_in_sizemove && game)
		{
			game->OnWindowSizeChanged(LOWORD(_lParam), HIWORD(_lParam));
		}
		break;

	case WM_ENTERSIZEMOVE:
		s_in_sizemove = true;
		break;

	case WM_EXITSIZEMOVE:
		s_in_sizemove = false;
		if (game)
		{
			RECT rc;
			GetClientRect(_hWnd, &rc);

			game->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
		}
		break;

	case WM_GETMINMAXINFO:
		if (_lParam)
		{
			auto info = reinterpret_cast<MINMAXINFO*>(_lParam);
			info->ptMinTrackSize.x = 320;
			info->ptMinTrackSize.y = 200;
		}
		break;

	case WM_ACTIVATEAPP:
		if (game)
		{
			if (_wParam)
			{
				game->OnActivated();
			}
			else
			{
				game->OnDeactivated();
			}
		}
		Keyboard::ProcessMessage(_message, _wParam, _lParam);
		Mouse::ProcessMessage(_message, _wParam, _lParam);
		break;

	case WM_ACTIVATE:
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(_message, _wParam, _lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(_message, _wParam, _lParam);
		break;

	case WM_POWERBROADCAST:
		switch (_wParam)
		{
		case PBT_APMQUERYSUSPEND:
			if (!s_in_suspend && game)
				game->OnSuspending();
			s_in_suspend = true;
			return TRUE;

		case PBT_APMRESUMESUSPEND:
			if (!s_minimized)
			{
				if (s_in_suspend && game)
					game->OnResuming();
				s_in_suspend = false;
			}
			return TRUE;

		default:
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SYSKEYDOWN:
		Keyboard::ProcessMessage(_message, _wParam, _lParam);
		if (_wParam == VK_RETURN && (_lParam & 0x60000000) == 0x20000000)
		{
			// Implements the classic ALT+ENTER full screen toggle
			if (s_fullscreen)
			{
				SetWindowLongPtr(_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
				SetWindowLongPtr(_hWnd, GWL_EXSTYLE, 0);

				int width = 800;
				int height = 600;
				if (game)
					game->GetDefaultSize(width, height);

				ShowWindow(_hWnd, SW_SHOWNORMAL);

				SetWindowPos(_hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			else
			{
				SetWindowLongPtr(_hWnd, GWL_STYLE, WS_POPUP);
				SetWindowLongPtr(_hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

				SetWindowPos(_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

				ShowWindow(_hWnd, SW_SHOWMAXIMIZED);
			}

			s_fullscreen = !s_fullscreen;
		}
		break;

	case WM_MENUCHAR:
		// A menu is active and the user presses a key that does not correspond
		// to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
		return MAKELRESULT(0, MNC_CLOSE);

	default:
		break;
	}

	return DefWindowProc(_hWnd, _message, _wParam, _lParam);
}

// Exit helper
void ExitGame() noexcept
{
	PostQuitMessage(0);
}
