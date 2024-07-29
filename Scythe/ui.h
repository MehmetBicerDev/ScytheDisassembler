#pragma once
#include "includes.h"
#include <d3d11.h>

namespace ui
{
	static POINTS position = {};
	static POINT size = { 900, 500 };
	static int title_height = 20;

	bool init();
	void update();

	extern ID3D11Device* g_pd3dDevice;
	extern ID3D11DeviceContext* g_pd3dDeviceContext;
}
