#include "pch.h"
#include "SoundFactory.h"
#include "TestSound.h"
#include "loop.h"
#include <assert.h>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

Sound* SoundFactory::makeNewSound(string _type)
{
	if (_type == "SOUND")
	{
		return new Sound();
	}
	else if (_type == "TEST")
	{
		return new TestSound();
	}
	else if (_type == "LOOP")
	{
		return new Loop();
	}
	else
	{
		cout << "UNKNOWN SOUND TYPE : " << _type << endl;
		assert(0);
		return nullptr;
	}
}
