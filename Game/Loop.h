#pragma once
#include "Sound.h"

//Extends the basic sound to make a looing ambient sound
// Info about it in this wiki tutorial, plus more info about the base "Audio Engine"
//https://github.com/Microsoft/DirectXTK/wiki/Adding-audio-to-your-project

class Loop : public Sound
{
public:

	Loop();
	~Loop();

	virtual void Init(AudioEngine* _audEngine) override; //initialize the sound

	virtual void Tick(GameState* _GS) override; //update per frame

	virtual void Play() override; //play the sound

	bool GetPlaying() { return m_playing; } //is the sound currently playing
	void TogglePlaying() { m_playing = !m_playing; } //toggle play state
	void SetPlaying(bool _playing) { m_playing = _playing; } //set play state

protected:
	std::unique_ptr<DirectX::SoundEffectInstance> m_loop;
	bool m_playing = false;
};

