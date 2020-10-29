// put a bitmap image on a Windows Console display
// BCX basic original by Joe Caverly and Kevin Diggins
// BCX generated C code modified for PellesC/Dev-C++

#include <pch.h>
#include <commctrl.h>

HWND  hConWnd;
HHOOK hhk = 0;
LRESULT Hookproc(int code, WPARAM wParam, LPARAM lParam)
{
    CWPRETSTRUCT* msg = (CWPRETSTRUCT*)lParam;
    printf("\r\n****Received %d\r\n", code);
    return CallNextHookEx(hhk, code, wParam, lParam);
}

// draw the bitmap
HWND BCX_Bitmap(char* Text, HWND hWnd, int id, int X, int Y, int W, int H, int Res, int Style, int Exstyle)
{
    hhk = SetWindowsHookExA(WH_CALLWNDPROCRET, Hookproc, NULL, 0);
    if(!hhk)
    {
        DWORD err = GetLastError();
            return 0;
    }
    HBITMAP hBitmap;
    RECT rect;
    if (!GetClientRect(hWnd, &rect))
    {
        DWORD err = GetLastError();
        return 0;
    }
    // Text contains filename
    hBitmap = (HBITMAP)LoadImageA(0, Text, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    DWORD err = GetLastError();
    // auto-adjust width and height
    W = 161;
    H = 58;

    LONG wnd_width=rect.right-rect.left;

    HDC hdc = GetDC(hWnd);
    HDC hMemDC = CreateCompatibleDC(hdc);
    SelectObject(hMemDC, hBitmap);

    // get the image size and display the whole thing
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);
    BitBlt(hdc, wnd_width - W, 0, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

    // release objects
    DeleteDC(hMemDC);
    DeleteObject(hBitmap);
    ReleaseDC(hWnd, hdc);
 
    return 0;
}
// tricking Windows just a little ...
HWND GetConsoleWndHandle(void)
{
    HWND hConWnd;
    char szTempTitle[64], szClassName[128], szOriginalTitle[1024];

    GetConsoleTitleA(szOriginalTitle, sizeof(szOriginalTitle));
    sprintf(szTempTitle, "%u - %u", GetTickCount(), GetCurrentProcessId());
    SetConsoleTitleA(szTempTitle);
    Sleep(40);
    // handle for NT and XP
    hConWnd = FindWindowA(NULL, szTempTitle);
    SetConsoleTitleA(szOriginalTitle);
    // may not work on WIN9x
    hConWnd = GetWindow(hConWnd, GW_CHILD);
    if (hConWnd == NULL) 
        return 0;
    GetClassNameA(hConWnd, szClassName, sizeof(szClassName));
    // while ( _stricmp( szClassName, "ttyGrab" ) != 0 )
    while (strcmp(szClassName, "ttyGrab") != 0)
    {
        hConWnd = GetNextWindow(hConWnd, GW_HWNDNEXT);
        if (hConWnd == NULL) return 0;
        GetClassNameA(hConWnd, szClassName, sizeof(szClassName));
    }
    return hConWnd;
}
