#pragma once

#include <string>
#include <windows.h>

class Clipboard
{
public:
    static bool SetText(const std::string& text)
    {
        if (!OpenClipboard(nullptr))
            return false;

        EmptyClipboard();

        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
        if (!hMem)
        {
            CloseClipboard();
            return false;
        }

        memcpy(GlobalLock(hMem), text.c_str(), text.size() + 1);
        GlobalUnlock(hMem);

        SetClipboardData(CF_TEXT, hMem);

        CloseClipboard();
        return true;
    }

    static std::string GetText()
    {
        std::string result;

        if (!OpenClipboard(nullptr))
            return result;

        HANDLE hData = GetClipboardData(CF_TEXT);
        if (hData)
        {
            char* pszText = static_cast<char*>(GlobalLock(hData));
            if (pszText)
            {
                result = pszText;
                GlobalUnlock(hData);
            }
        }

        CloseClipboard();
        return result;
    }
};