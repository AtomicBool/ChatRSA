#pragma once
#include <unordered_map>
#include <windows.h>

#include "utils/keyboard/KeyboardState.h"

class InputEvents
{
public:
    bool IsPressed(int vk, ULONGLONG cooldownMs = 165)
    {
        ULONGLONG now = GetTickCount64();

        bool down = KeyboardState::IsDown(vk);
        bool& prevDown = m_prev[vk];
        ULONGLONG& lastTime = m_last[vk];

        // edge detection
        bool isNewPress = (down && !prevDown);

        prevDown = down;

        if (!isNewPress)
            return false;

        if (now - lastTime < cooldownMs)
            return false;

        lastTime = now;
        return true;
    }

private:
    std::unordered_map<int, bool> m_prev;
    std::unordered_map<int, ULONGLONG> m_last;
};