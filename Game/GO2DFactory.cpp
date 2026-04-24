#include "pch.h"
#include "GO2DFactory.h"
#include "ImageGO2D.h"
#include "TextGO2D.h"
#include "PauseLogo.h"
#include "RTImage.h"

using std::string;

GameObject2D* GO2DFactory::makeNewGO2D(std::string _type)
{
	if (_type == "IMAGE")
	{
		return new ImageGO2D();
	}
	else if (_type == "TEXT")
	{
		return new TextGO2D();
	}
	else if (_type == "PAUSE")
	{
		return new PauseLogo();
	}
	else if (_type == "RENDERTARGET")
	{
		return new RTImage();
	}
	else
	{
		printf("UNKNOWN 2D GAME OBJECT TYPE: %s \n", _type.c_str());
		assert(0);
		return nullptr;
	}
}
