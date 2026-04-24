#pragma once
#include <string>
class GameObject;

//A rather simple Factory using the base class GameObject2D
//generates a GameObject2D based on its type

class GOFactory
{
public:

	static GameObject* makeNewGO(std::string _type);
};

