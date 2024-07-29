#include "ui.h"
#include "ImExtensions.h"
#include <dwmapi.h>
#include <imgui_impl_dx11.h>
#include "menu.h"

#pragma comment(lib, "d3d11")
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // Forward declare message handler from imgui_impl_win32.cpp

namespace ui
{
	bool should_run = true;

	HWND hwnd = 0;
	RECT rc;
	ID3D11Device* g_pd3dDevice = NULL;
	ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	IDXGISwapChain* g_pSwapChain = NULL;
	ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void CleanupRenderTarget();

	void CleanupDeviceD3D()
	{
		CleanupRenderTarget();
		if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
		if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	}

	void CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}

	void CleanupRenderTarget()
	{
		if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
	}
	bool CreateDeviceD3D(HWND hWnd)
	{
		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
			return false;

		CreateRenderTarget();

		return true;
	}
	bool init()
	{
		WNDCLASSEXW wc;
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.style = CS_CLASSDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = NULL;
		wc.cbWndExtra = NULL;
		wc.hInstance = nullptr;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = L"ImGui";
		wc.lpszClassName = L"Class01";
		wc.hIconSm = LoadIcon(0, IDI_APPLICATION);

		RegisterClassExW(&wc);
		hwnd = CreateWindowExW(NULL, wc.lpszClassName, L"ImGui Window", WS_POPUP, (GetSystemMetrics(SM_CXSCREEN) / 2) - (ui::size.x / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (ui::size.y / 2), ui::size.x, ui::size.y, 0, 0, 0, 0);

		SetWindowLongA(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

		MARGINS margins = { -1 };
		// DwmExtendFrameIntoClientArea(hwnd, &margins);

		POINT mouse;
		rc = { 0 };
		GetWindowRect(hwnd, &rc);

		// Initialize Direct3D
		if (!CreateDeviceD3D(hwnd))
		{
			CleanupDeviceD3D();
			::UnregisterClass((LPCSTR)wc.lpszClassName, wc.hInstance);
			return 1;
		}

		// Show the window
		::ShowWindow(hwnd, SW_SHOWDEFAULT);
		::UpdateWindow(hwnd);

		ImExtensions::InitImGuiD3D11(hwnd, g_pd3dDevice, g_pd3dDeviceContext);
		return true;
	}
	void update()
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				should_run = false;
		}
		if (!should_run)
			return;

		menu::update();

		const float clear_color_with_alpha[4] = { 0 };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0);
	}

	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg)
		{
		case WM_SIZE:
			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				CleanupRenderTarget();
				g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xFFF0) == SC_KEYMENU) // Disable ALT application menu
				return 0L;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0L;
		case WM_LBUTTONDOWN:
			ui::position = MAKEPOINTS(lParam);
			return 0L;
		case WM_MOUSEMOVE:
			if (wParam == MK_LBUTTON)
			{
				const auto points = MAKEPOINTS(lParam);
				auto rect = ::RECT{ };

				GetWindowRect(hWnd, &rect);

				rect.left += points.x - ui::position.x;
				rect.top += points.y - ui::position.y;

				if (ui::position.x >= 0 &&
					ui::position.x >= 0 && ui::position.x <= (ui::size.x - (ui::title_height + (ImGui::GetStyle().WindowBorderSize * 2))) &&
					ui::position.y >= 0 && ui::position.y <= ui::title_height)
				{
					SetWindowPos(
						hWnd,
						HWND_TOPMOST,
						rect.left, rect.top,
						0, 0,
						SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
					);
				}
			}

			return 0L;
		}

		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}
}