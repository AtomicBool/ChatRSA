#pragma once

#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <iomanip>

#include "ui/RenderUtils.h"
#include "ui/UI.h"

#include "utils/Contact.h"
#include "utils/keyboard/InputEvents.h"

#include "crypto/RSA.h"
#include "utils/os/file.h"
#include "utils/os/conversion.h"

class Application
{
public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

private:
	bool initWindow();
    bool loadKeyPair(std::string privateKeyPath = "private.key", std::string publicKeyPath = "public.key");
    void Update();
    void Render();
    void UpdateWindowState();

    // now event-driven (replaces pending flag logic)
    void ProcessUIEvents();

private:
    HWND m_hwnd = nullptr;
    DWORD m_dwExStyle = WS_EX_TOPMOST | WS_EX_LAYERED;

    UIState m_uiState;
    ContactManager m_contactManager;
    InputEvents m_input;
    
    RSA m_userRSA, m_peerRSA;

	Contact m_selectedContact;

    bool m_done = false;
};