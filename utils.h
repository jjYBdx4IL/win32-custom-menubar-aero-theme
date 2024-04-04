#pragma once
LPCWSTR get_message_name(DWORD msg);
void initDarkMode();
void setImmersiveDarkMode(HWND hWnd);
void dbgMsg(HWND hWnd, UINT_PTR subclass, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);
