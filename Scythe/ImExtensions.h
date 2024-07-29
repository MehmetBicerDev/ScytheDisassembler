#pragma once
#include "includes.h"

#include <imgui.h>

namespace ImExtensions
{
	void InitImGuiD3D11(HWND hWnd, ID3D11Device* g_pd3dDevice, ID3D11DeviceContext* g_pd3dDeviceContext);
	void SetStyleWindowsDark();
	bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data);
	bool InputTextMultiline(const char* label, std::string* str, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data);
	bool InputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data);
	bool FileSelectorManual(const std::string& ItemName, std::string& FilePath, bool& ShowFileSelector);

	void DrawAssemblyInstruction(const std::string& text);
}