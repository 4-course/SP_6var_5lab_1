#define _USE_MATH_DEFINES

#include <windows.h>
#include <math.h>
#include <conio.h>
#include <iostream>
#include "resource.h"

struct ARGS {
    HWND textHwnd;
    HWND mainHwnd;
}args;

HWND text;
DWORD threadID1;
HANDLE threadHandle;

int circleRadius;
double gradus;
double rad;
double centerX, centerY;
double X0, Y0,
rx, ry,
c, s,
X1, Y1;

void checkIsCLassRegistered(WNDCLASSEX windowClass) {
    if (!RegisterClassEx(&windowClass)) {
        MessageBox(NULL, L"Не удалось создать класс окна. Ошибка " + GetLastError(), L"Создание класса окна", MB_ICONERROR);
        exit(0);
    }
}

void checkIsWindowCreated(HWND window) {
    if (!window) {
        MessageBox(NULL, L"Не удалось создать окно. Ошибка " + GetLastError(), L"Создание окна", MB_ICONERROR);
        exit(0);
    }
}

DWORD WINAPI moveText(LPVOID hWnd) {
    ARGS* hwnds = (ARGS*)hWnd;
    for (int i = 0; i < 36; i++) {
        gradus = (i + 1.0) * 10.0;
        rad = (gradus * (M_PI / 180));
        c = cos(rad);
        s = sin(rad);
        X1 = centerX + rx * c - ry * s;
        Y1 = centerY + rx * s + ry * c;

        MoveWindow(hwnds->textHwnd, X1, Y1, 54, 20, FALSE);
        InvalidateRect(hwnds->mainHwnd, NULL, FALSE);
        //RedrawWindow(hwnds->mainHwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
        Sleep(400);
    }

    EnableMenuItem(GetMenu(hwnds->mainHwnd), ID_STOP1, MF_DISABLED | MF_GRAYED);
    EnableMenuItem(GetMenu(hwnds->mainHwnd), ID_RESUME1, MF_ENABLED);
    InvalidateRect(hwnds->mainHwnd, NULL, FALSE);
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    RECT rt;
    rt.left = 0;
    rt.top = 0;
    rt.right = 400;
    rt.bottom = 400;
    HDC hdc;
    PAINTSTRUCT ps;
    int d;
    switch (uMsg) {
        case WM_CREATE:
            //создать поток 1 в приостановленном состоянии
            text=CreateWindow(L"static", L"СТРОКА", WS_VISIBLE | WS_CHILD,
                         X0, Y0, 54, 20, hWnd, 0, NULL, NULL);
            if (text != NULL) {
                args = {text, hWnd};
                threadHandle = CreateThread(NULL, 0, moveText, &args, CREATE_SUSPENDED, &threadID1);
            }
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_RESUME1:
                    d = ResumeThread(threadHandle);
                    if (d == 0) {
                        threadHandle = CreateThread(NULL, 0, moveText, &args, CREATE_SUSPENDED, &threadID1);
                        if (threadHandle != 0)
                            ResumeThread(threadHandle);
                    }
                    EnableMenuItem(GetMenu(hWnd), ID_RESUME1, MF_DISABLED | MF_GRAYED);
                    EnableMenuItem(GetMenu(hWnd), ID_STOP1, MF_ENABLED);
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                case ID_STOP1:
                    SuspendThread(threadHandle);
                    EnableMenuItem(GetMenu(hWnd), ID_STOP1, MF_DISABLED | MF_GRAYED);
                    EnableMenuItem(GetMenu(hWnd), ID_RESUME1, MF_ENABLED);
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
            }
            break;
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);

            FillRect(hdc, &rt, (HBRUSH)(COLOR_WINDOW + 1));

            EndPaint(hWnd, &ps);
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst,
                   HINSTANCE hPreviousInst,
                   LPSTR lpCommandLine,
                   int nCommandShow) {
    double screenWidth = GetSystemMetrics(SM_CXSCREEN),
        screenHeight = GetSystemMetrics(SM_CYSCREEN);
    circleRadius = 100;
    centerX = 150.0;
    centerY = 150.0;
    X0 = centerX;
    Y0 = centerY - circleRadius;
    rx = X0 - centerX;
    ry = Y0 - centerY;
    MSG uMsg;
    WNDCLASSEX mainWindowClass;
    HWND mainWindow;

    memset(&mainWindowClass, 0, sizeof(WNDCLASSEX));
    mainWindowClass.cbSize = sizeof(WNDCLASSEX);
    mainWindowClass.hbrBackground = (HBRUSH)WHITE_BRUSH;
    mainWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    mainWindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    mainWindowClass.hInstance = hInst;
    mainWindowClass.lpfnWndProc = WindowProc;
    mainWindowClass.lpszClassName = L"mainWindow";
    mainWindowClass.lpszMenuName = (LPCWSTR)IDR_MENU1;

    checkIsCLassRegistered(mainWindowClass);
    mainWindow = CreateWindow(mainWindowClass.lpszClassName, L"Работа с потоками", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                              (screenWidth - 800) / 2, (screenHeight - 400) / 2,
                              800, 400, NULL, NULL, hInst, NULL);

    checkIsWindowCreated(mainWindow);

    ShowWindow(mainWindow, nCommandShow);

    while (GetMessage(&uMsg, NULL, NULL, NULL)) {
        TranslateMessage(&uMsg);
        DispatchMessage(&uMsg);
    }
    return uMsg.wParam;
    return 0;
}