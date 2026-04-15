#pragma once

#include "RenderUtils.h"
#include "contact.h"
#include "UI.h"
#include <vector>

class Application {
public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    void Update();
    void Render();
    bool HandleEvents();
    bool IsKeyPressed(int vkCode, ULONGLONG interval = 150);

private:
    HWND m_hwnd = nullptr;
    UIState m_uiState;
    ContactManager m_contactManager;
    
    // Input state
    ULONGLONG m_lastKeyTime[256] = { 0 };
    DWORD m_dwExStyle = WS_EX_TOPMOST | WS_EX_LAYERED;
    
    bool m_done = false;
};
