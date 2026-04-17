#pragma once

#include <string>
#include <windows.h>

class Clipboard
{
private:
    // UTF-8 -> UTF-16
    static std::wstring Utf8ToWide(const std::string& str)
    {
        if (str.empty()) return L"";

        int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        if (size == 0) return L"";

        std::wstring result(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);

        result.pop_back(); // remove null terminator
        return result;
    }

    // UTF-16 -> UTF-8
    static std::string WideToUtf8(const std::wstring& wstr)
    {
        if (wstr.empty()) return "";

        int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (size == 0) return "";

        std::string result(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);

        result.pop_back(); // remove null terminator
        return result;
    }

    static bool OpenClipboardWithRetry()
    {
        for (int i = 0; i < 5; ++i)
        {
            if (OpenClipboard(nullptr))
                return true;
            Sleep(10);
        }
        return false;
    }

public:
    static bool SetText(const std::string& text)
    {
        std::wstring wtext = Utf8ToWide(text);

        if (!OpenClipboardWithRetry())
            return false;

        EmptyClipboard();

        SIZE_T size = (wtext.size() + 1) * sizeof(wchar_t);
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
        if (!hMem)
        {
            CloseClipboard();
            return false;
        }

        void* ptr = GlobalLock(hMem);
        if (!ptr)
        {
            GlobalFree(hMem);
            CloseClipboard();
            return false;
        }

        memcpy(ptr, wtext.c_str(), size);
        GlobalUnlock(hMem);

        if (!SetClipboardData(CF_UNICODETEXT, hMem))
        {
            GlobalFree(hMem);
            CloseClipboard();
            return false;
        }

        CloseClipboard();
        return true;
    }

    static std::string GetText()
    {
        std::string result;

        if (!OpenClipboardWithRetry())
            return result;

        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData)
        {
            wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
            if (pszText)
            {
                result = WideToUtf8(pszText);
                GlobalUnlock(hData);
            }
        }

        CloseClipboard();
        return result;
    }
};