#pragma once
#include <string>
class Sound;

//A rather simple Factory using the base class Sound
//generates a Sound based on its type

class SoundFactory
{
public:

	static Sound* makeNewSound(std::string type);
};