// win32-custom-menubar-aero-theme.cpp : Defines the entry point for the application.
//

#include "win32-custom-menubar-aero-theme.h"
#include "UAHMenuBar.h"
#include "utils.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    // https://gist.github.com/rounk-ctrl/b04e5622e30e0d62956870d5c22b7017
    enum class PreferredAppMode
    {
        Default,
        AllowDark,
        ForceDark,
        ForceLight,
        Max
    };
    using fnShouldAppsUseDarkMode = bool (WINAPI*)(); // ordinal 132
    using fnAllowDarkModeForWindow = bool (WINAPI*)(HWND hWnd, bool allow); // ordinal 133
    using fnSetPreferredAppMode = PreferredAppMode(WINAPI*)(PreferredAppMode appMode); // ordinal 135, in 1903
    HMODULE hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    fnSetPreferredAppMode SetPreferredAppMode;
    SetPreferredAppMode = (fnSetPreferredAppMode)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
    SetPreferredAppMode(PreferredAppMode::AllowDark);
    FreeLibrary(hUxtheme);



    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32CUSTOMMENUBARAEROTHEME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32CUSTOMMENUBARAEROTHEME));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32CUSTOMMENUBARAEROTHEME));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32CUSTOMMENUBARAEROTHEME);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 400, 240, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // https://stackoverflow.com/questions/39261826/change-the-color-of-the-title-bar-caption-of-a-win32-application
   BOOL USE_DARK_MODE = true;
   BOOL SET_IMMERSIVE_DARK_MODE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(
       hWnd, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
       &USE_DARK_MODE, sizeof(USE_DARK_MODE)));
   ASSERT(SET_IMMERSIVE_DARK_MODE_SUCCESS);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}




//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int dupe_counter = 0;
    static int last_message = -1;
    if (message == last_message) {
        dupe_counter++;
    }
    else {
        if (dupe_counter) {
            std::wstringstream str;
            str << L"Suppressed " << dupe_counter << " sequential messages of the same type" << std::endl;
            OutputDebugString(str.str().c_str());
            dupe_counter = 0;
        }
        last_message = message;
    }
    if (dupe_counter == 0) {
        std::wstringstream str;
        WCHAR tmp[5];
        swprintf_s(tmp, 5, L"%04x", message);
        std::wstring s1 = std::format(L"{:%T}", std::chrono::system_clock::now());
        str << s1 << L" WndProc(" << hWnd << ", " << get_message_name(message) << " 0x" << tmp << " " << message << ", " << wParam << ", " << lParam << ")" << std::endl;
        switch (message) { // https://wiki.winehq.org/List_Of_Windows_Messages
        case WM_SETCURSOR: // 0x0020 32
        case WM_NCMOUSEMOVE: // 0x00a0		160
        case WM_NCHITTEST: // 0x0084		132
        case WM_MOUSEMOVE: // 0x0200 512
        case WM_NCMOUSELEAVE: // 0x02a2		674
            break;
        default:
            OutputDebugString(str.str().c_str());
        }
    }

    LRESULT lr = 0;
    if (UAHWndProc(hWnd, message, wParam, lParam, &lr)) {
        return lr;
    }

    switch (message)
    {
    case WM_KEYDOWN:
        if (wParam == 70) { // 'f'
            //https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
            static WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };
            DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            ASSERT(dwStyle);
            if (dwStyle & WS_OVERLAPPEDWINDOW) {
                MONITORINFO mi = { sizeof(mi) };
                if (GetWindowPlacement(hWnd, &g_wpPrev) &&
                    GetMonitorInfo(MonitorFromWindow(hWnd,
                        MONITOR_DEFAULTTOPRIMARY), &mi)) {
                    SetWindowLong(hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
                    BOOL res = SetWindowPos(hWnd, HWND_TOP,
                        mi.rcMonitor.left,
                        mi.rcMonitor.top,
                        mi.rcMonitor.right - mi.rcMonitor.left,
                        mi.rcMonitor.bottom - mi.rcMonitor.top,
                        SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
                    ASSERT(res);
                }
            }
            else {
                SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
                SetWindowPlacement(hWnd, &g_wpPrev);
                SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                    SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            }
        }
        if (wParam == 81) { // 'q'
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_INITMENUPOPUP:
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
