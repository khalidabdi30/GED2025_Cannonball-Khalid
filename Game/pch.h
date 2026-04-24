// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// Use the C++ standard templated min/max
#define NOMINMAX

// add headers that you want to pre-compile here
#include "framework.h"

#include <Windows.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <exception>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <tuple>

#include <wrl/client.h>


#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			throw std::exception();
		}
	}
}


#endif //PCH_H
