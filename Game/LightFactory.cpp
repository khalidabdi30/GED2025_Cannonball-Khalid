#include "pch.h"
#include "LightFactory.h"
#include "Light.h"
#include <assert.h>

using std::string;

Light* LightFactory::makeNewLight(string _type)
{
	printf("LIGHT TYPE: %s \n", _type.c_str());
	if (_type == "LIGHT")
	{
		return new Light();
	}
	else
	{
		printf("UNKNOWN LIGHT TYPE!");
		assert(0);
		return nullptr;
	}
}
