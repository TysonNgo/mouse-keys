#include <windows.h>
#include <shellapi.h>
#include <algorithm>
//#include "debug.h"

#ifdef DEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif

// I should refactor everything

#define ID_TRAY_APP_ICON  225
#define ID_TRAY_EXIT      315
#define WM_SYSICON (WM_USER+1)

#ifndef MOD_ALT
#define MOD_NOREPEAT 0x0001
#endif
#ifndef MOD_CONTROL
#define MOD_NOREPEAT 0x0002
#endif
#ifndef MOD_SHIFT
#define MOD_NOREPEAT 0x0004
#endif
#ifndef MOD_WIN
#define MOD_NOREPEAT 0x0008
#endif
#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000
#endif

#define ICO1 101

UINT WM_TASKBAR = 0;
HWND hwnd;
HMENU hmenu;
NOTIFYICONDATA notifyIconData;
TCHAR szTIP[64] = TEXT("Tyson Mouse Keys");
char szClassName[] = "Mouse Keys";

POINT savePoints[10];

const int CURSOR_MOVE_MIN = 25;
const int CURSOR_MOVE_MAX = 500;
const int CURSOR_STEP = 25;
const int CURSOR_DEFAULT = 100;

int cursorMove = CURSOR_DEFAULT; // pixels to move cursor
///////
void SetRelCursorPos(int x, int y)
{
    POINT pt;
    GetCursorPos(&pt);
    SetCursorPos(pt.x+x, pt.y+y);
}

POINT getXYDirection(int numDir)
{
    int direction = 1;
    for (int y=1; y>=-1; y--)
    {
        for (int x=-1; x<=1; x++)
        {
            if (direction == numDir)
            {
                return POINT{x, y};
            }
            direction++;
        }
    }
    return POINT{0, 0};
}

void click(bool left=true, bool press=true)
{

    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
    
    if (left)
    {
        input.mi.dwFlags = input.mi.dwFlags | MOUSEEVENTF_LEFTDOWN;
        if (press)
        {
            input.mi.dwFlags = input.mi.dwFlags | MOUSEEVENTF_LEFTUP;
        }
    }
    else
    {
        input.mi.dwFlags = input.mi.dwFlags | MOUSEEVENTF_RIGHTDOWN;
        if (press)
        {
            input.mi.dwFlags = input.mi.dwFlags | MOUSEEVENTF_RIGHTUP;
        }
    }

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

    // cursor movement in the foreground window with numpad directions (edges and center)
    RegisterHotKey(NULL, 11, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD1);
    RegisterHotKey(NULL, 12, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD2);
    RegisterHotKey(NULL, 13, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD3);
    RegisterHotKey(NULL, 14, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD4);
    RegisterHotKey(NULL, 15, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD5);
    RegisterHotKey(NULL, 16, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD6);
    RegisterHotKey(NULL, 17, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD7);
    RegisterHotKey(NULL, 18, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD8);
    RegisterHotKey(NULL, 19, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_NUMPAD9);

    // cursor save states
    RegisterHotKey(NULL, 20, MOD_WIN | MOD_NOREPEAT, VK_NUMPAD0);
    RegisterHotKey(NULL, 21, MOD_WIN | MOD_NOREPEAT, VK_NUMPAD1);
    RegisterHotKey(NULL, 22, MOD_WIN | MOD_NOREPEAT, VK_NUMPAD2);
    RegisterHotKey(NULL, 23, MOD_WIN | MOD_NOREPEAT, VK_NUMPAD3);
    RegisterHotKey(NULL, 24, MOD_WIN | MOD_NOREPEAT, VK_NUMPAD4);
    RegisterHotKey(NULL, 25, MOD_WIN | MOD_NOREPEAT, VK_NUMPAD5);
    RegisterHotKey(NULL, 26, MOD_WIN | MOD_NOREPEAT, VK_NUMPAD6);
    RegisterHotKey(NULL, 27, MOD_WIN | MOD_NOREPEAT, VK_NUMPAD7);
    RegisterHotKey(NULL, 28, MOD_WIN | MOD_NOREPEAT, VK_NUMPAD8);
    RegisterHotKey(NULL, 29, MOD_WIN | MOD_NOREPEAT, VK_NUMPAD9);

    // cursor move to save states
    RegisterHotKey(NULL, 30, MOD_CONTROL | MOD_WIN | MOD_NOREPEAT, VK_NUMPAD0);
    RegisterHotKey(NULL, 31, MOD_CONTROL | MOD_WIN | MOD_NOREPEAT, VK_NUMPAD1);
    RegisterHotKey(NULL, 32, MOD_CONTROL | MOD_WIN | MOD_NOREPEAT, VK_NUMPAD2);
    RegisterHotKey(NULL, 33, MOD_CONTROL | MOD_WIN | MOD_NOREPEAT, VK_NUMPAD3);
    RegisterHotKey(NULL, 34, MOD_CONTROL | MOD_WIN | MOD_NOREPEAT, VK_NUMPAD4);
    RegisterHotKey(NULL, 35, MOD_CONTROL | MOD_WIN | MOD_NOREPEAT, VK_NUMPAD5);
    RegisterHotKey(NULL, 36, MOD_CONTROL | MOD_WIN | MOD_NOREPEAT, VK_NUMPAD6);
    RegisterHotKey(NULL, 37, MOD_CONTROL | MOD_WIN | MOD_NOREPEAT, VK_NUMPAD7);
    RegisterHotKey(NULL, 38, MOD_CONTROL | MOD_WIN | MOD_NOREPEAT, VK_NUMPAD8);
    RegisterHotKey(NULL, 39, MOD_CONTROL | MOD_WIN | MOD_NOREPEAT, VK_NUMPAD9);

    RegisterHotKey(NULL, 0, MOD_CONTROL, VK_NUMPAD0); // mouse hold
    RegisterHotKey(NULL, 10, MOD_CONTROL | MOD_ALT, VK_NUMPAD0); 
    RegisterHotKey(NULL, 5, MOD_CONTROL | MOD_NOREPEAT, VK_NUMPAD5); // mouse press

    // decrease cursor speed
    RegisterHotKey(NULL, 1000, MOD_CONTROL | MOD_NOREPEAT, VK_DIVIDE);
    // increase cursor speed
    RegisterHotKey(NULL, 1001, MOD_CONTROL | MOD_NOREPEAT, VK_MULTIPLY);
    // reset cursor speed to default
    RegisterHotKey(NULL, 1002, MOD_CONTROL | MOD_NOREPEAT, VK_SUBTRACT);
}

void handleHotKey(int hotkey){
    RECT fgWindow;
    POINT dir;
    POINT pt;
    if (hotkey >= 11 && hotkey <=19)
    {
        GetWindowRect(GetForegroundWindow(), &fgWindow);
    }

    switch (hotkey)
    {
        case 0:
            click(true, false);
            break;
        case 5:
            click();
            break;
        case 7: case 8: case 9:
        case 4:         case 6:
        case 1: case 2: case 3:
            dir = getXYDirection(hotkey);
            SetRelCursorPos(dir.x*cursorMove, dir.y*cursorMove);
            break;
        case 17: case 18: case 19:
        case 14: case 15: case 16:
        case 11: case 12: case 13:
            dir = getXYDirection(hotkey % 10);
            switch (dir.x)
            {
                case -1:
                    dir.x = fgWindow.left;
                    break;
                case 0:
                    dir.x = (fgWindow.left+fgWindow.right)/2;
                    break;
                case 1:
                    dir.x = fgWindow.right-5;
                    break;
            }

            switch (dir.y)
            {
                case -1:
                    dir.y = fgWindow.top;
                    break;
                case 0:
                    dir.y = (fgWindow.top+fgWindow.bottom-5)/2;
                    break;
                case 1:
                    dir.y = fgWindow.bottom-5;
                    break;
                default:
                    break;
            }
            SetCursorPos(dir.x, dir.y);
            break;
        // save states
        case 27: case 28: case 29:
        case 24: case 25: case 26:
        case 21: case 22: case 23:
        case 20:
            GetCursorPos(&pt);
            savePoints[hotkey % 10] = pt;
            break;
        // move cursor to saved state
        case 37: case 38: case 39:
        case 34: case 35: case 36:
        case 31: case 32: case 33:
        case 30:
            pt = savePoints[hotkey % 10];
            if (pt.x + pt.y != 0)
            {
                SetCursorPos(pt.x, pt.y);
            }
            break;
        case 1000: case 1001:
            cursorMove = (hotkey % 10) ? 
                std::min(CURSOR_MOVE_MAX, cursorMove+CURSOR_STEP) :
                std::max(CURSOR_MOVE_MIN, cursorMove-CURSOR_STEP);
            break;
        case 1002:
            cursorMove = CURSOR_DEFAULT;
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
    ShowWindow(hwnd, SW_HIDE);
    #ifndef DEBUG
        ShowWindow(GetConsoleWindow(), SW_HIDE);
    #endif

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
            AppendMenu(hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));
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

            /*if (lParam == WM_LBUTTONUP)
            {
                restore();
            }
            else*/ if (lParam == WM_RBUTTONDOWN)
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