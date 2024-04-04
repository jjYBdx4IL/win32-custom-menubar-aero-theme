#pragma once
LPCWSTR get_message_name(DWORD msg);
void initDarkMode();
void setImmersiveDarkMode(HWND hWnd);
void dbgMsg(HWND hWnd, UINT_PTR subclass, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);

#define ASSERT_LAST_ERR(x) \
{ \
BOOL rc = x; \
if (!rc) { \
    std::wstringstream ss; \
    ss << std::endl << std::endl << L"  ***  GetLastError() = " << GetLastError() << std::endl << std::endl; \
    OutputDebugString(ss.str().c_str()); \
} \
assert(rc); \
}
