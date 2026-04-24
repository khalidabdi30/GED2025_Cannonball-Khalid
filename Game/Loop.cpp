#include "pch.h"
#include "Loop.h"
#include "AudioSystem.h"

void Loop::Init(AudioEngine* _audEngine)
{
	Sound::Init(_audEngine);
	AudioSystem::Init(_audEngine);
	AudioSystem::PlayMusic("crowd-cheering-in-stadium");

	if (m_sfx)
	{
		m_loop = m_sfx->CreateInstance();
		this->Play();
	}
}



Loop::Loop()
{
	m_type = "LOOP";
}

Loop::~Loop()
{
	if (m_loop)
	{
		m_loop->Stop(true);
		m_loop.reset();
	}
}

void Loop::Play()
{
	if (m_playing)
	{
		m_loop->Stop(true);
	}
	else
	{
		m_loop->Play(true);
	}
	m_playing = !m_playing;

}


void Loop::Tick(GameState* _GS)
{
	AudioSystem::Update();
	_GS;
	//Testing SFX for things like collision and stuff
	static bool wasKDown = false;

	bool isKDown = (GetAsyncKeyState('K') & 0x8000) != 0;

	if (isKDown && !wasKDown)
	{
		AudioSystem::PlaySFX("voice-message");
	}

	wasKDown = isKDown;
	//End of SFX test code

	if (m_loop)
	{
		m_loop->SetVolume(m_volume);
		m_loop->SetPitch(m_pitch);
		m_loop->SetPan(m_pan);
	}
}
