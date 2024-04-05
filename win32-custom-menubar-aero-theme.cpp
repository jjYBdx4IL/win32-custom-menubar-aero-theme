// win32-custom-menubar-aero-theme.cpp : Defines the entry point for the application.
//

#include "win32-custom-menubar-aero-theme.h"
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

static HHOOK hook_ = NULL;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

#ifndef NDEBUG
    // set a hook for debugging purposes only
    ASSERT(!hook_);
    hook_ = SetWindowsHookEx(WH_CBT, CBTProc, NULL, GetCurrentThreadId());
    ASSERT(hook_);
    OutputDebugString(L"CBT Hook installed.\n");
#endif

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

    if (hook_) {
        UnhookWindowsHookEx(hook_);
        hook_ = NULL;
        OutputDebugString(L"CBT Hook uninstalled.\n");
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

    wcex.style = CS_HREDRAW | CS_VREDRAW; // | BS_OWNERDRAW;
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

   initDarkMode(); // dark popup menus
   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 400, 240, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   setImmersiveDarkMode(hWnd); // dark window title bar

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// https://github.com/microsoftarchive/msdn-code-gallery-microsoft/blob/master/OneCodeTeam/Owner-drawn%20menu%20item%20in%20C%2B%2B%20Windows%20app%20(CppWindowsOwnerDrawnMenu)/%5BC%2B%2B%5D-Owner-drawn%20menu%20item%20in%20C%2B%2B%20Windows%20app%20(CppWindowsOwnerDrawnMenu)/C%2B%2B/CppWindowsOwnerDrawnMenu/CppWindowsOwnerDrawnMenu.cpp
// Sructure associated with Character menu items
typedef struct tagCHARMENUITEM
{
    // Font of text on the menu item.
    HFONT hFont;

    // The length of the string pointed to by szItemText.
    int cchItemText;

    // A pointer to a buffer that specifies the text string. The string does 
    // not need to be null-terminated, because the c parameter specifies the 
    // length of the string.
    wchar_t szItemText[1];

} CHARMENUITEM, * PCHARMENUITEM;
#define MENUITEM_TEXTCOLOR      RGB(255,0,0) // or GetSysColor(COLOR_MENUTEXT)
#define MENUITEM_BACKCOLOR      RGB(0,255,0) // or GetSysColor(COLOR_MENU)
#define HIGHLIGHT_TEXTCOLOR     GetSysColor(COLOR_HIGHLIGHTTEXT)
#define HIGHLIGHT_BACKCOLOR     GetSysColor(COLOR_HIGHLIGHT)

HFONT CreateMenuItemFont()
{
    LOGFONT lf = { sizeof(lf) };
    wcscpy_s(lf.lfFaceName, ARRAYSIZE(lf.lfFaceName), L"Times New Roman");
    lf.lfHeight = 20;
    return CreateFontIndirect(&lf);
}
static HFONT menuItemFont = CreateMenuItemFont();

void prepareMenu(HMENU hMenu) {
    BOOL fSucceeded = TRUE;
    PCHARMENUITEM pcmi = NULL;

    for (UINT uID = 0; uID < GetMenuItemCount(hMenu); uID++)
    {
        MENUITEMINFO mii = { sizeof(mii) };

        // To retrieve a menu item of type MFT_STRING, first find the length 
        // of the string by setting the dwTypeData member of MENUITEMINFO to 
        // NULL and then calling GetMenuItemInfo. The value of cch is the 
        // length of the menu item text.
        mii.fMask = MIIM_STRING;
        mii.dwTypeData = NULL;
        if (!GetMenuItemInfo(hMenu, uID, TRUE, &mii))
        {
            int a = GetLastError();
            fSucceeded = FALSE;
            break;
        }

        // Then allocate a buffer of this size.
        pcmi = (PCHARMENUITEM)LocalAlloc(LPTR,
            sizeof(*pcmi) + mii.cch * sizeof(*pcmi->szItemText));
        if (NULL == pcmi)
        {
            fSucceeded = FALSE;
            break;
        }

        // Place the pointer to the buffer in dwTypeData, increment cch, and 
        // call GetMenuItemInfo once again to fill the buffer with the string. 
        pcmi->cchItemText = mii.cch;
        mii.dwTypeData = pcmi->szItemText;
        mii.cch++;
        if (!GetMenuItemInfo(hMenu, uID, TRUE, &mii))
        {
            fSucceeded = FALSE;
            break;
        }

        // Create the font used to draw the item. 
        pcmi->hFont = menuItemFont;
        if (NULL == pcmi->hFont)
        {
            fSucceeded = FALSE;
            break;
        }

        // Change the item to an owner-drawn item, and save the 
        // address of the item structure as item data. 
        mii.fMask = MIIM_FTYPE | MIIM_DATA;
        mii.fType = MF_OWNERDRAW;
        mii.dwItemData = (ULONG_PTR)pcmi;
        if (!SetMenuItemInfo(hMenu, uID, TRUE, &mii))
        {
            fSucceeded = FALSE;
            break;
        }
    }

    // Clean up the allocated resource when applicable.
    if (!fSucceeded && pcmi)
    {
        LocalFree(pcmi);
    }
}

//
//   FUNCTION: OnCreate(HWND, LPCREATESTRUCT)
//
//   PURPOSE: Process the WM_CREATE message. Because a menu template cannot 
//   specify owner-drawn items, the menu initially contains four text menu 
//   items with the following strings: "Regular," "Bold," "Italic," and 
//   "Underline." This function changes these to owner-drawn items, and 
//   attaches to each menu item a CHARMENUITEM structure that will be used 
//   when the menu item is created and drawn.
//
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    HMENU hMenu = GetMenu(hWnd);
    prepareMenu(hMenu);
    return true;
}

//
//   FUNCTION: OnMeasureItem(HWND, MEASUREITEMSTRUCT *)
//
//   PURPOSE: Process the WM_MEASUREITEM message. A WM_MEASUREITEM message is 
//   sent for each owner-drawn menu item the first time it is displayed. The 
//   application processes this message by selecting the font for the menu 
//   item into a device context and then determining the space required to 
//   display the menu item text in that font. The font and menu item text are 
//   both specified by the menu item's CHARMENUITEM structure (the structure 
//   defined by the application). 
//
void OnMeasureItem(HWND hWnd, MEASUREITEMSTRUCT* lpMeasureItem)
{
    // Retrieve the menu item's CHARMENUITEM structure.
    PCHARMENUITEM pcmi = (PCHARMENUITEM)lpMeasureItem->itemData;
    if (pcmi)
    {
        // Retrieve a device context for the main window. 
        HDC hdc = GetDC(hWnd);

        // Select the font associated with the item into the main window's 
        // device context.
        HFONT hFontOld = (HFONT)SelectObject(hdc, pcmi->hFont);

        // Retrieve the width and height of the item's string, and then copy 
        // the width and height into the MEASUREITEMSTRUCT structure's 
        // itemWidth and itemHeight members.
        SIZE size;
        GetTextExtentPoint32(hdc, pcmi->szItemText, pcmi->cchItemText, &size);
        lpMeasureItem->itemWidth = size.cx;
        lpMeasureItem->itemHeight = size.cy;

        // Restore the original font and release the device context.
        SelectObject(hdc, hFontOld);
        ReleaseDC(hWnd, hdc);
    }
}


//
//   FUNCTION: OnDrawItem(HWND, const DRAWITEMSTRUCT *)
//
//   PURPOSE: Process the WM_DRAWITEM message by displaying the menu item 
//   text in the appropriate font. The font and menu item text are both 
//   specified by the menu item's CHARMENUITEM structure. The application 
//   selects text and background colors appropriate to the menu item's state.
//
void OnDrawItem(HWND hWnd, const DRAWITEMSTRUCT* lpDrawItem)
{
    // Retrieve the menu item's CHARMENUITEM structure.
    PCHARMENUITEM pcmi = (PCHARMENUITEM)lpDrawItem->itemData;
    ASSERT(pcmi);
    if (pcmi)
    {
        COLORREF clrPrevText, clrPrevBkgnd;
        HFONT hFontPrev;
        int x, y;

        // Set the appropriate foreground and background colors.
        if (lpDrawItem->itemState & ODS_SELECTED)
        {
            clrPrevText = SetTextColor(lpDrawItem->hDC, HIGHLIGHT_TEXTCOLOR);
            clrPrevBkgnd = SetBkColor(lpDrawItem->hDC, HIGHLIGHT_BACKCOLOR);
        }
        else
        {
            clrPrevText = SetTextColor(lpDrawItem->hDC, MENUITEM_TEXTCOLOR);
            clrPrevBkgnd = SetBkColor(lpDrawItem->hDC, MENUITEM_BACKCOLOR);
        }

        // Determine where to draw and leave space for a check mark. 
        x = lpDrawItem->rcItem.left;
        y = lpDrawItem->rcItem.top;
        x += GetSystemMetrics(SM_CXMENUCHECK);

        // Select the font and draw the text. 
        //hFontPrev = (HFONT)SelectObject(lpDrawItem->hDC, pcmi->hFont);
        ExtTextOut(lpDrawItem->hDC, x, y, ETO_OPAQUE, &lpDrawItem->rcItem,
            pcmi->szItemText, pcmi->cchItemText, NULL);
        TCHAR buf[512];
        ZeroMemory(buf, 512);
        memcpy_s(buf, 512, (void*)pcmi->szItemText, pcmi->cchItemText * sizeof(TCHAR));
        OutputDebugString(L"*** OnDrawItem: ");
        OutputDebugString(buf);
        OutputDebugString(L"\n");

        // Restore the original font and colors. 
        //SelectObject(lpDrawItem->hDC, hFontPrev);
        SetTextColor(lpDrawItem->hDC, clrPrevText);
        SetBkColor(lpDrawItem->hDC, clrPrevBkgnd);
    }
}

void OnInitMenuPopup(HMENU hMenu, UINT nIndex, BOOL bSysMenu)
{
    UNREFERENCED_PARAMETER(nIndex);
    prepareMenu(hMenu);
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
    dbgMsg(hWnd, 0, message, wParam, lParam);

    switch (message)
    {
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
        default:
            break;
        }
        break;
    }
    case WM_CREATE:
    {
        OnCreate(hWnd, (LPCREATESTRUCT)lParam);
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    case WM_DRAWITEM:
    {
        OnDrawItem(hWnd, (LPDRAWITEMSTRUCT)lParam);
        break;
    }
    case WM_INITMENUPOPUP:
    {
        OnInitMenuPopup((HMENU)wParam, LOWORD(lParam), HIWORD(lParam));
        break;
    }
    case WM_KEYDOWN:
    {
        if (wParam == 70) { // 'f'
            //https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
            static WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };
            DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            ASSERT(dwStyle);
            if (dwStyle & WS_OVERLAPPEDWINDOW) { // switch to fullscreen
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
            else { // restore / exit from fullscreen
                SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
                SetWindowPlacement(hWnd, &g_wpPrev);
                SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                    SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            }
        }
        else if (wParam == 81) { // 'q'
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;
    }
    case WM_MEASUREITEM:
    {
        //if (wParam == 0) { // handle menus only
            OnMeasureItem(hWnd, (LPMEASUREITEMSTRUCT)lParam);
        //}
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_SHOWWINDOW:
        break;
    default:
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
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
