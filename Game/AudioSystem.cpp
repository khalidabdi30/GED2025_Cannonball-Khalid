#include "pch.h"
#include "AudioSystem.h"

using namespace DirectX;


AudioEngine* AudioSystem::audioEngine = nullptr;
std::unordered_map<std::string, std::unique_ptr<SoundEffect>> AudioSystem::sounds;
std::unique_ptr<SoundEffectInstance> AudioSystem::musicInstance;


void AudioSystem::Init(AudioEngine* engine)
{
    audioEngine = engine; 
}


void AudioSystem::Update()
{
    if (audioEngine)
        audioEngine->Update();
}


std::wstring AudioSystem::ToWide(const std::string& str)
{
    return std::wstring(str.begin(), str.end());
}


void AudioSystem::PlaySFX(const std::string& name)
{
    if (sounds.find(name) == sounds.end())
    {
        auto path = L"../Sounds/" + ToWide(name) + L".wav";
        sounds[name] = std::make_unique<SoundEffect>(audioEngine, path.c_str());
    }

    sounds[name]->Play();
}


void AudioSystem::PlayMusic(const std::string& name, bool loop)
{
    if (sounds.find(name) == sounds.end())
    {
        auto path = L"../Sounds/" + ToWide(name) + L".wav";
        sounds[name] = std::make_unique<SoundEffect>(audioEngine, path.c_str());
    }

    musicInstance = sounds[name]->CreateInstance();
    musicInstance->Play(loop);
}

void AudioSystem::StopMusic()
{
    if (musicInstance)
        musicInstance->Stop();
}
