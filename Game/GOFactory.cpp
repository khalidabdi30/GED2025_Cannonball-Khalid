#include "pch.h"
#include "GOFactory.h"
#include "CMOGO.h"
#include "fileVBGO.h"
#include "VBCube.h"
#include "VBPillow.h"
#include "VBspiral.h"
#include "VBSnail.h"
#include "GamePadPhysCMOGO.h"
#include "GeoPrim.h"
#include "BulletCMOGO.h"
#include "WorldLimit.h"
#include "PlayerObject.h"
#include "PowerUpBox.h"

using std::string;

GameObject* GOFactory::makeNewGO(std::string _type)
{
	if (_type == "CMOGO")
	{
		return new CMOGO();
	}
	else if (_type == "FILEVBGO")
	{
		return new FileVBGO();
	}
	else if (_type == "VBCUBE")
	{
		return new VBCube();
	}
	else if (_type == "VBPILLOW")
	{
		return new VBPillow();
	}
	else if (_type == "VBSPIRAL")
	{
		return new VBSpiral();
	}
	else if (_type == "VBSNAIL")
	{
		return new VBSnail();
	}
	else if (_type == "GAMEPAD_PHYS_CMOGO")
	{
		return new GamePadPhysCMOGO();
	}
	else if (_type == "GEOPRIM")
	{
		return new GeoPrim();
	}
	else if (_type == "BULLET_CMOGO")
	{
		return new BulletCMOGO();
	}
	else if (_type == "WORLD_LIMIT")
	{
		return new WorldLimit();
	}
	else if (_type == "PLAYER")
	{
		return new PlayerObject();
	}
	else if (_type == "POWERUPBOX")
	{
		return new PowerUpBox();
	}
	else
	{
		printf("UNKNOWN 3D GAME OBJECT TYPE: %s \n", _type.c_str());
		assert(0);
		return nullptr;
	}
}
