#pragma once

//=================================================================
//Data to be passed by game to all Game Objects via Tick
//=================================================================

#include "FSMState.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "GamePad.h"

using namespace DirectX;

struct GameState
{
	float m_dt; //time step since last frame
	FSMState m_FSM; //Global FSM State

	//player input
	Keyboard::State m_KBS;
	Mouse::State m_MS;
	Keyboard::KeyboardStateTracker m_KBS_tracker;
	GamePad::State m_GPS[4];
};

