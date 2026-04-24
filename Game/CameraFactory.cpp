#include "pch.h"
#include "CameraFactory.h"
#include "Camera.h"
#include "FPSCamera.h"
#include "LookAtCamera.h"
#include "ViewportCam.h"
#include "RenderTarget.h"
#include "MinimapCamera.h" //minimap camera include
#include <assert.h>

using std::string;

Camera* CameraFactory::makeNewCam(string _type)
{
	if (_type == "CAMERA")
	{
		return new Camera();
	}
	else if (_type == "FPSCAM")
	{
		return new FPSCamera();
	}
	else if (_type == "LOOKAT")
	{
		return new LookAtCamera();
	}
	else if (_type == "VIEWPORT")
	{
		return new ViewportCam();
	}
	else if (_type == "MINIMAPCAMERA") //adds minimap camera to scene
	{
		return new MiniMapCamera();
	}
	else if (_type == "RENDERTARGET")
	{
		return new RenderTarget();
	}
	else
	{
		printf("UNKNOWN CAMERA TYPE!");
		assert(0);
		return nullptr;
	}
}
