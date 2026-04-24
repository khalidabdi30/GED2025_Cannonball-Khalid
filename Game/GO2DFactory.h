#pragma once
#include <string>
class GameObject2D;

//A rather simple Factory using the base class GameObject2D
//generates a GameObject2D based on its type

class GO2DFactory
{
public:

	static GameObject2D* makeNewGO2D(std::string _type);
};

