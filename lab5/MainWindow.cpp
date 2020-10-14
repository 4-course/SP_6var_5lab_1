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

struct PARAMS {
    double x0;
    double y0;
    double diag1;
    double diag2;
    HWND hWnd;
}params;

HWND text, rombX, rombY, diagonal1, diagonal2;
DWORD threadID1;
HANDLE threadHandle;
DWORD threadID2;
HANDLE threadHandle2;

int circleRadius;
double gradus;
double rad;
double centerX, centerY;
double X0, Y0,
rx, ry,
c, s,
X1, Y1;

HDC memDC, hdc;
HBITMAP memBM;
HANDLE handle;

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

DWORD WINAPI thread2(LPVOID params) {
    PARAMS* parameters = (PARAMS*)params;
    PAINTSTRUCT ps;
    RECT r;
    hdc = GetDC(parameters->hWnd);

    memDC = CreateCompatibleDC(hdc);
    memBM = CreateCompatibleBitmap(hdc, 800, 400);
    handle = SelectObject(memDC, memBM);

    SetRect(&r, 0, 0, 800, 400);
    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(memDC, &r, brush);

    MoveToEx(memDC, 555 + parameters->x0, 200 - parameters->y0 + parameters->diag1 / 2, NULL);
    LineTo(memDC, 555 + parameters->x0 - parameters->diag2 / 2, 200 - parameters->y0);
    LineTo(memDC, 555 + parameters->x0, 200 - parameters->y0 - parameters->diag1 / 2);
    LineTo(memDC, 555 + parameters->x0 + parameters->diag2 / 2, 200 - parameters->y0);
    LineTo(memDC, 555 + parameters->x0, 200 - parameters->y0 + parameters->diag1 / 2);
    MoveToEx(memDC, 555, 420, NULL);
    LineTo(memDC, 555, 55);

    DeleteObject(brush);
    InvalidateRect(parameters->hWnd, NULL, TRUE);
     return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    RECT rt;
    rt.left = 0;
    rt.top = 0;
    rt.right = 310;
    rt.bottom = 310;
    HDC hdc;
    PAINTSTRUCT ps;
    int d;
    double lineLength = 5 * sqrt(3);
    wchar_t rombx[5], romby[5], diag1[5], diag2[5];
    switch (uMsg) {
        case WM_CREATE:
            //создать поток 1 в приостановленном состоянии
            text=CreateWindow(L"static", L"СТРОКА", WS_VISIBLE | WS_CHILD,
                         X0, Y0, 54, 20, hWnd, 0, NULL, NULL);
            if (text != NULL) {
                args = {text, hWnd};
                threadHandle = CreateThread(NULL, 0, moveText, &args, CREATE_SUSPENDED, &threadID1);
            }

            rombX = CreateWindow(L"edit", L"0", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 
                                 340, 9, 50, 25, hWnd, 0, 
                                 ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            rombY = CreateWindow(L"edit", L"0", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 
                                 430, 9, 50, 25, hWnd, 0, 
                                 ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            diagonal1 = CreateWindow(L"edit", L"40", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 
                                 540, 9, 50, 25, hWnd, 0, 
                                 ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            diagonal2 = CreateWindow(L"edit", L"50", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 
                                 650, 9, 50, 25, hWnd, 0, 
                                 ((LPCREATESTRUCT)lParam)->hInstance, NULL);
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
                case ID_CREATE2:
                    GetWindowText(rombX, rombx, sizeof(rombx));
                    GetWindowText(rombY, romby, sizeof(romby));
                    GetWindowText(diagonal1, diag1, sizeof(diag1));
                    GetWindowText(diagonal2, diag2, sizeof(diag2));

                    params.x0 = _wtof(rombx);
                    params.y0 = _wtof(romby);
                    params.diag1 = _wtof(diag1);
                    params.diag2 = _wtof(diag2);
                    params.hWnd = hWnd;

                    if (params.diag1 <= 0 || params.diag2 <= 0) {
                        MessageBox(hWnd, L"Диагонали не могут быть меньше нуля", L"Диагонали ромба", MB_ICONERROR);
                        break;
                    }

                    threadHandle2 = CreateThread(NULL, 0, thread2, &params, NULL, &threadID2);
                    EnableMenuItem(GetMenu(hWnd), ID_CREATE2, MF_DISABLED | MF_GRAYED);

                    break;
            }
            break;
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);

            if (memDC != NULL) {
                BitBlt(hdc, 0, 0, 800, 400, memDC, 0, 0, SRCCOPY);
                DeleteObject(memBM);
                DeleteDC(memDC);
            }

            TextOut(hdc, 310, 12, L"X0=", 3);
            TextOut(hdc, 400, 12, L"Y0=", 3);
            TextOut(hdc, 490, 12, L"diag1=", 6);
            TextOut(hdc, 600, 12, L"diag2=", 6);

            MoveToEx(hdc, 555, 400, NULL);
            LineTo(hdc, 555, 35);
            LineTo(hdc, 560, 35 + 5*sqrt(3));
            MoveToEx(hdc, 555, 35, NULL);
            LineTo(hdc, 550, 35 + 5 * sqrt(3));

            MoveToEx(hdc, 310, 200, NULL);
            LineTo(hdc, 780, 200);
            LineTo(hdc, 780 - 5 * sqrt(3), 205);
            MoveToEx(hdc, 780, 200, NULL);
            LineTo(hdc, 780 - 5 * sqrt(3), 195);
            

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