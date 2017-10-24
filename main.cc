#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <algorithm>

#define ID_TRAY_APP_ICON  225
#define ID_TRAY_EXIT      315
#define WM_SYSICON (WM_USER+1)

#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000
#endif

#define ICO1 101

UINT WM_TASKBAR = 0;
HWND hwnd;
HMENU hmenu;
NOTIFYICONDATA notifyIconData;
TCHAR szTIP[64] = TEXT("asdasddas");
char szClassName[] = "zczxzzxczxczxc";

int cursorMove = 50; // pixels to move cursor
const int CURSOR_MOVE_MAX = 500;
const int CURSOR_STEP = 25;

///////
void SetRelCursorPos(int x, int y){
    POINT pt;
    GetCursorPos(&pt);
    SetCursorPos(pt.x+x, pt.y+y);
}

void moveCursor(int numDir)
{
    int direction = 1;
    for (int y=1; y>=-1; y--)
    {
        for (int x=-1; x<=1; x++)
        {
            if (direction == numDir)
            {
                SetRelCursorPos(x*cursorMove, y*cursorMove);
                return;
            }
            direction++;
        }
    }
}

void Click()
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = (
        MOUSEEVENTF_ABSOLUTE |
        MOUSEEVENTF_LEFTDOWN |
        MOUSEEVENTF_LEFTUP
    );
    SendInput(1, &input, sizeof(INPUT));
}

void RegisterHotKeys(){
    /*
     [7][8][9]
     [4][ ][6]    movement hotkeys
     [1][2][3]
    */
    // relative cursor movement corresponding with the numpad directions
    RegisterHotKey(NULL, 1, MOD_CONTROL, VK_NUMPAD1);
    RegisterHotKey(NULL, 2, MOD_CONTROL, VK_NUMPAD2);
    RegisterHotKey(NULL, 3, MOD_CONTROL, VK_NUMPAD3);
    RegisterHotKey(NULL, 4, MOD_CONTROL, VK_NUMPAD4);
    RegisterHotKey(NULL, 6, MOD_CONTROL, VK_NUMPAD6);
    RegisterHotKey(NULL, 7, MOD_CONTROL, VK_NUMPAD7);
    RegisterHotKey(NULL, 8, MOD_CONTROL, VK_NUMPAD8);
    RegisterHotKey(NULL, 9, MOD_CONTROL, VK_NUMPAD9);

    // 
    RegisterHotKey(NULL, 11, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD1);
    RegisterHotKey(NULL, 12, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD2);
    RegisterHotKey(NULL, 13, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD3);
    RegisterHotKey(NULL, 14, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD4);
    RegisterHotKey(NULL, 15, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD5);
    RegisterHotKey(NULL, 16, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD6);
    RegisterHotKey(NULL, 17, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD7);
    RegisterHotKey(NULL, 18, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD8);
    RegisterHotKey(NULL, 19, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD9);


    RegisterHotKey(NULL, 0, MOD_CONTROL, VK_NUMPAD0);
    RegisterHotKey(NULL, 10, MOD_CONTROL | MOD_ALT, VK_NUMPAD0);
    RegisterHotKey(NULL, 5, MOD_CONTROL | MOD_NOREPEAT, VK_NUMPAD5);

    // decrease cursor speed
    RegisterHotKey(NULL, 1000, MOD_CONTROL | MOD_NOREPEAT, VK_DIVIDE);
    // increase cursor speed
    RegisterHotKey(NULL, 1001, MOD_CONTROL | MOD_NOREPEAT, VK_MULTIPLY);
}

void handleHotKey(int hotkey){
    RECT fgWindow;
    if (hotkey >= 11 && hotkey <=19){
        GetWindowRect(GetForegroundWindow(), &fgWindow);
    }

    switch (hotkey)
    {
        case 5:
            Click();
            break;
        case 7: case 8: case 9:
        case 4:         case 6:
        case 1: case 2: case 3:
            moveCursor(hotkey);
            break;
        case 1000:
            cursorMove = std::max(0, cursorMove-CURSOR_STEP);
            break;
        case 1001:
            cursorMove = std::min(CURSOR_MOVE_MAX, cursorMove+CURSOR_STEP);
            break;
    }
}
///////


LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
void minimize();
void restore();
void initNotifyIconData();


int WINAPI WinMain(
    HINSTANCE hThisInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpszArgument,
    int nCmdShow
){
    MSG messages;
    WNDCLASSEX wincl;
    WM_TASKBAR = RegisterWindowMessageA("TaskbarCreated");

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WndProc;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);

    wincl.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
    wincl.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    //wincl.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(255, 255, 255)));

    if (!RegisterClassEx (&wincl))
    {
        return 0;
    }

    hwnd = CreateWindowEx(
        0,
        szClassName,
        szClassName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        200,
        200,
        HWND_DESKTOP,
        NULL,
        hThisInstance,
        NULL
    );

    initNotifyIconData();
    ShowWindow(hwnd, nCmdShow);

    RegisterHotKeys();

    while (GetMessage(&messages, NULL, 0, 0))
    {
        if (messages.message == WM_HOTKEY)
        {
            handleHotKey(messages.wParam);
        }
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}


LRESULT CALLBACK WndProc (
    HWND Hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
){
    /*MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        
    } */

    if (message == WM_TASKBAR && !IsWindowVisible(hwnd))
    {
        minimize();
        return 0;
    }

    switch(message)
    {
        case WM_ACTIVATE:
            Shell_NotifyIcon(NIM_ADD, &notifyIconData);
            break;
        case WM_CREATE:
            ShowWindow(hwnd, SW_HIDE);
            hmenu = CreatePopupMenu();
            AppendMenu(hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("exit"));
            break;
        case WM_SYSCOMMAND:
            switch (wParam & 0xFFF0)
            {
                case SC_MINIMIZE:
                case SC_CLOSE:
                    minimize();
                    return 0;
                    break;
            }
            break;
        case WM_SYSICON:
        {
            switch (wParam)
            {
                case ID_TRAY_APP_ICON:
                SetForegroundWindow(hwnd);
                break;
            }

            if (lParam == WM_LBUTTONUP)
            {
                restore();
            }
            else if (lParam == WM_RBUTTONDOWN)
            {
                POINT curPoint;
                GetCursorPos(&curPoint);
                SetForegroundWindow(hwnd);

                UINT clicked = TrackPopupMenu(hmenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0, Hwnd, NULL);

                SendMessage(Hwnd, WM_NULL, 0, 0);

                if (clicked == ID_TRAY_EXIT)
                {
                    Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
                    PostQuitMessage(0);
                }
            }
        }
        break;
        case WM_CLOSE:
            minimize();
            return 0;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(Hwnd, message, wParam, lParam);
}


void minimize()
{
    ShowWindow(hwnd, SW_HIDE);
}

void restore()
{
    ShowWindow(hwnd, SW_SHOW);
}

void initNotifyIconData()
{
    memset(&notifyIconData, 0, sizeof(NOTIFYICONDATA));

    notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
    notifyIconData.hWnd = hwnd;
    notifyIconData.uID = ID_TRAY_APP_ICON;
    notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    notifyIconData.uCallbackMessage = WM_SYSICON;
    //notifyIconData.hIcon = (HICON) LoadImage(NULL, TEXT("somethig.ico"))
    notifyIconData.hIcon = (HICON) LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
    strncpy(notifyIconData.szTip, szTIP, sizeof(szTIP));
}