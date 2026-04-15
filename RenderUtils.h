#pragma once

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include <d3d11.h>

struct WindowConfig {
    int width, height;
    int x, y;
    float scale;
};

WindowConfig SetupWindowEnv(float widthPercent, float heightPercent);
HWND CreateAppWindow(const WindowConfig& config, WNDPROC wndProc);

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);