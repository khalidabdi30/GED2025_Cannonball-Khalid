#pragma once
#include "Audio.h"
#include <string>
#include <codecvt>
struct GameState;

using namespace std;
using namespace DirectX;

// Essential sound effect stuff here
// https://github.com/microsoft/DirectXTK/wiki/Audio

class Sound
{
public:
	Sound();
	virtual ~Sound();

	virtual void Tick(GameState* _GS) { _GS; };

	virtual void Init(AudioEngine* _audEngine);

	virtual void Load(ifstream& _file);

	virtual void Play();

	void SetVolume(float _vol) { m_volume = _vol; }
	float GetVolume() { return m_volume; }

	void SetPitch(float _pitch) { m_pitch = _pitch; }
	float GetPitch() { return m_pitch; }

	void SetPan(float _pan) { m_pan = _pan; }
	float GetPan() { return m_pan; }

	string GetName() { return m_name; }

protected:
	std::unique_ptr<DirectX::SoundEffect> m_sfx;

	float m_volume = 1.0f;
	float m_pitch = 0.0f;
	float m_pan = 0.0f;

	string m_type = "";
	string m_name = "";
	string m_file = "";
};

