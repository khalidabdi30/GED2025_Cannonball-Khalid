#pragma once
#include <string>
class Light;

//A  simple Factory using the base class Camera
//generates a Camera based on its type

class LightFactory
{
public:

	static Light* makeNewLight(std::string type);
};