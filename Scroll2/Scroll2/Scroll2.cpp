#include <Windows.h>
#include "resource.h"

//структура "фигуры"(x,y,size,color), функция отрисовки квадрата, функция отрисовки круга, клавиатура(телефон)

#define COLS 3
#define ROWS 2

//====================================================

// Структура для хранения информации о прямоугольниках
struct RectInfo {
    RECT rect;
    bool isHovered = false;
    int rectWidth; // Ширина прямоугольника
    int rectHeight; // Высота прямоугольника
};

//====================================================

// Функция централизации окна
void CenterWindow(HWND hWnd) {
    RECT windowRect;
    GetWindowRect(hWnd, &windowRect);
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
    SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

//====================================================

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//====================================================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    LPCTSTR szClass = TEXT("sdfghjkllkjhgf");

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = szClass;
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!::RegisterClass(&wc)) { return -1; }


    HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCEA(IDC_SCROLL2));

    HWND hMainWindow = ::CreateWindow(szClass, "Главное окно", WS_OVERLAPPEDWINDOW, 0,
        0, 800, 500, NULL, hMenu, hInstance, NULL);
    if (!hMainWindow) {
        return -1; // Если окно не удалось создать, то возвращает ошибку
    }

    // Создание виртуальной области рисования
    SCROLLINFO scrollInf = { sizeof(SCROLLINFO), SIF_ALL, 0, 2000, 800, 0, 0 };
    SetScrollInfo(hMainWindow, SB_HORZ, &scrollInf, TRUE);
    SetScrollInfo(hMainWindow, SB_VERT, &scrollInf, TRUE);

    CenterWindow(hMainWindow);
    ::ShowWindow(hMainWindow, nCmdShow);

    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    return 0;
}

//====================================================
void dividingClientArea(RECT clientRect, HDC hdc, RectInfo rects[]) {

    int width = clientRect.right - clientRect.left; // Ширина окна
    int height = clientRect.bottom - clientRect.top; // Высота окна

    rects->rectWidth = width / COLS; // Ширина прямоугольника
    rects->rectHeight = height / ROWS; // Высота прямоугольника

    for (int i = 0; i < (COLS * ROWS); ++i) {
        int x = (i % COLS) * rects->rectWidth;
        int y = (i / COLS) * rects->rectHeight;

        rects[i].rect = { x, y, x + rects->rectWidth, y + rects->rectHeight };
    }
}

void fillingRectangle(HDC hdc, RectInfo rects[]) {
    for (int i = 0; i < (COLS * ROWS); i++) {
        if (rects[i].isHovered) {
            HBRUSH hBrsh = ::CreateSolidBrush(RGB(222, 233, 205));
            FillRect(hdc, &rects[i].rect, hBrsh);
            DeleteObject(hBrsh);
        }
    }
}

void drawingGrid(HDC hdc, int rectWidth, int rectHeight, RECT clientRect) {
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN oPen = (HPEN)SelectObject(hdc, hPen);

    for (int i = 0; i <= COLS; ++i)
    {
        MoveToEx(hdc, i * rectWidth, 0, NULL);
        LineTo(hdc, i * rectWidth, clientRect.bottom);
    }
    for (int i = 0; i <= ROWS; ++i)
    {
        MoveToEx(hdc, 0, i * rectHeight, NULL);
        LineTo(hdc, clientRect.right, i * rectHeight);
    }

    SelectObject(hdc, oPen);
    DeleteObject(hPen);
}
// Обработчик сообщений для окон
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

//======================================================

    // Получение текущих значений прокрутки
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_POS;
    GetScrollInfo(hWnd, SB_HORZ, &si);
    int scrollX = si.nPos;
    GetScrollInfo(hWnd, SB_VERT, &si);
    int scrollY = si.nPos;

    static RectInfo rects[COLS * ROWS];
    HDC hdc;
    PAINTSTRUCT ps;
//=================================================

    switch (message) {
    case WM_CREATE:

        return 0;
    case WM_HSCROLL:
    {
        // Обработка горизонтального скроллинга
        SCROLLINFO si;
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_HORZ, &si);
        int scrollPos = si.nPos;
        switch (LOWORD(wParam))
        {
        case SB_LINELEFT:
            //000000000000
            scrollPos -= 10;
            break;
        case SB_LINERIGHT:
            //000000000000
            scrollPos += 10;
            break;
        case SB_PAGELEFT:
            scrollPos -= si.nPage;
            break;
        case SB_PAGERIGHT:
            scrollPos += si.nPage;
            break;
        case SB_THUMBPOSITION:
            scrollPos = HIWORD(wParam);
            break;
        default:
            break;
        }
        si.nPos = scrollPos;
        SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }
    case WM_VSCROLL:
    {
        // Обработка вертикального скроллинга
        SCROLLINFO si;
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_VERT, &si);
        int scrollPos = si.nPos;
        switch (LOWORD(wParam))
        {
        case SB_LINEUP:
            //000000000000
            scrollPos -= 10;
            break;
        case SB_LINEDOWN:
            //000000000000
            scrollPos += 10;
            break;
        case SB_PAGEUP:
            scrollPos -= si.nPage;
            break;
        case SB_PAGEDOWN:
            scrollPos += si.nPage;
            break;
        case SB_THUMBPOSITION:
            scrollPos = HIWORD(wParam);
            break;
        default:
            break;
        }
        si.nPos = scrollPos;
        SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_EXIT:
            ::DestroyWindow(hWnd);
            break;
        }
        return 0;

    case WM_SIZE:
        return 0;

    case WM_PAINT: {
        hdc = BeginPaint(hWnd, &ps);

        RECT clientRect = {0,0,1000,1000};

        dividingClientArea(clientRect, hdc, rects);
        fillingRectangle(hdc, rects);
        drawingGrid(hdc, rects->rectWidth, rects->rectHeight, clientRect);

        EndPaint(hWnd, &ps);
        return 0;
    }
//=================================================
    case WM_DESTROY: // Закрытие окна
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam); // Обработка других сообщений
}