#include "main.h"
#include "RenderUtils.h"
#include "search.h"
#include "UI.h"

// UI States
UIState uiState;
ContactManager contact_manager("contacts.csv");

// Record of last key press times to implement debouncing
ULONGLONG g_lastKeyTime[256] = { 0 };

HWND hwnd;
DWORD dwExStyle = WS_EX_TOPMOST | WS_EX_LAYERED;

extern ID3D11Device*            g_pd3dDevice;
extern ID3D11DeviceContext*     g_pd3dDeviceContext;
extern IDXGISwapChain*          g_pSwapChain;
extern bool                     g_SwapChainOccluded;
extern UINT                     g_ResizeWidth, g_ResizeHeight;
extern ID3D11RenderTargetView*  g_mainRenderTargetView;

static bool KeyPressed(int vkCode, ULONGLONG interval = 150) {
    ULONGLONG now = GetTickCount64();
    if (GetAsyncKeyState(vkCode) & 0x8000) {
        if (now - g_lastKeyTime[vkCode] > interval) {
            g_lastKeyTime[vkCode] = now;
            return true;
        }
    }
    return false;
}

// Main code
int main(int, char**)
{
    // 1. Setup Window and DPI
    auto winConfig = SetupWindowEnv(0.65f, 0.3f);
    hwnd = CreateAppWindow(winConfig, WndProc);
    if (!hwnd) return 1;

    // 2. Initialize Alpha Compositing and Direct3D
    ImGui_ImplWin32_EnableAlphaCompositing(hwnd);
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        return 1;
    }

    // 3. Show the window
    ::SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // 4. Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup style and scaling
    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(winConfig.scale);
    ImGui::GetStyle().FontScaleDpi = winConfig.scale;

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Main loop
    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        {
            if (KeyPressed(VK_F2)) {
                uiState.display = !uiState.display;
                if (uiState.display) {
                    SetWindowLongPtr(hwnd, GWL_EXSTYLE, dwExStyle);
                    SetForegroundWindow(hwnd);
                    uiState.firstFrame = true;
                }
                else {
                    SetWindowLongPtr(hwnd, GWL_EXSTYLE, dwExStyle | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
                }
                SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
            }

            UI::Render(uiState, contact_manager);
        }

        // Rendering
        ImGui::Render();
        const float fClear[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        const float fDebug[4] = { 0.0f, 0.3f, 0.3f, 0.3f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (uiState.debug) ? fDebug : fClear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(L"LocalCipher", GetModuleHandle(nullptr));

    return 0;
}
