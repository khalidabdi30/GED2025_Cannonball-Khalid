#pragma once
#include <string>

class GameObject;

//Abstract Base Class for all Power Ups.
class PowerUp
{
public:
	PowerUp() = default;
	virtual ~PowerUp() = default;

	virtual void Activate(GameObject* user) = 0;
};