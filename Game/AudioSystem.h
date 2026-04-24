#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <Audio.h>

class AudioSystem
{
public:
    
    static void Init(DirectX::AudioEngine* engine);
    static void Update();

    static void PlaySFX(const std::string& name);
    static void PlayMusic(const std::string& name, bool loop = true);
    static void StopMusic();

private:
    
    static DirectX::AudioEngine* audioEngine;

    static std::unordered_map<
        std::string,
        std::unique_ptr<DirectX::SoundEffect>
    > sounds;

    static std::unique_ptr<DirectX::SoundEffectInstance> musicInstance;

    static std::wstring ToWide(const std::string& s);
};
