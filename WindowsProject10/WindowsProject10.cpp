//#define _CRT_SECURE_NO_WARNINGS
#include "WindowsProject10.h"
#include "framework.h"
#include <set>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>


using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HWND htext;
HWND hEditSize;
HWND hEditRelation;
HWND hButton;
HWND hOutput;
HFONT hFontOutput;

HWND hEditStart;
HWND hEditEnd;
HWND hButtonFindPath;

vector<vector<int>> graphMatrix; 


vector<int> dijkstra(const vector<vector<int>>& graph, int start, vector<int>& parent) {
    int n = graph.size();
    vector<int> dist(n, 1e9);
    parent.assign(n, -1);
    vector<bool> visited(n, false);
    dist[start] = 0;

    for (int i = 0; i < n; ++i) {
        int u = -1;
        for (int j = 0; j < n; ++j)
            if (!visited[j] && (u == -1 || dist[j] < dist[u]))
                u = j;

        if (dist[u] == 1e9)
            break;

        visited[u] = true;

        for (int v = 0; v < n; ++v) {
            if (graph[u][v] < 1e9) {
                int new_dist = dist[u] + graph[u][v];
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                    parent[v] = u;
                }
            }
        }
    }

    return dist;
}

vector<int> getPath(int end, const vector<int>& parent) {
    vector<int> path;
    for (int v = end; v != -1; v = parent[v])
        path.push_back(v);
    reverse(path.begin(), path.end());
    return path;
}

void printPath(const vector<int>& path, wstringstream& wss) {
    for (size_t i = 0; i < path.size(); ++i) {
        wss << (path[i] + 1);
        if (i + 1 < path.size()) {
            wss << L" -> ";
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT10, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT10));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT10));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT10);
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
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   hFontOutput = CreateFontW(
       26,
       0,
       0,
       0,
       FW_NORMAL,
       FALSE,
       FALSE,
       FALSE,
       DEFAULT_CHARSET,
       OUT_DEFAULT_PRECIS,
       CLIP_DEFAULT_PRECIS,
       DEFAULT_QUALITY,
       DEFAULT_PITCH | FF_DONTCARE,
       L"Segoe UI"
   );

   htext = CreateWindowW(L"STATIC", L"Матрица смежности (nxm):", WS_CHILD | WS_VISIBLE | WS_BORDER,
       10, 10, 200, 30, hWnd, NULL, hInstance, NULL);

   hEditSize = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
       210, 10, 150, 30, hWnd, NULL, hInstance, NULL);

   hEditRelation = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
       10, 50, 350, 250, hWnd, NULL, hInstance, NULL);

   CreateWindowW(L"STATIC", L"Начальная вершина:", WS_CHILD | WS_VISIBLE,
       10, 310, 130, 35, hWnd, NULL, hInstance, NULL);
   hEditStart = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
       150, 310, 60, 25, hWnd, NULL, hInstance, NULL);

   CreateWindowW(L"STATIC", L"Конечная вершина:", WS_CHILD | WS_VISIBLE,
       220, 310, 130, 35, hWnd, NULL, hInstance, NULL);
   hEditEnd = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
       360, 310, 60, 25, hWnd, NULL, hInstance, NULL);

   hButton = CreateWindowW(L"BUTTON", L"Загрузить матрицу", WS_CHILD | WS_VISIBLE,
       380, 10, 150, 30, hWnd, (HMENU)1, hInstance, NULL);

   hButtonFindPath = CreateWindowW(L"BUTTON", L"Найти путь", WS_CHILD | WS_VISIBLE,
       430, 310, 100, 25, hWnd, (HMENU)2, hInstance, NULL);

   hOutput = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
       10, 370, 520, 150, hWnd, NULL, hInstance, NULL);


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   SendMessage(hOutput, WM_SETFONT, (WPARAM)hFontOutput, TRUE);
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
    switch (message)
    {
    case WM_COMMAND:
    {///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (LOWORD(wParam) == 1)
        {
            WCHAR bufferSize[64];
            GetWindowTextW(hEditSize, bufferSize, 64);

            int n = 0, m = 0;
            if (swscanf_s(bufferSize, L"%d %d", &n, &m) != 2 || n <= 0 || m <= 0) {
                SetWindowTextW(hOutput, L"Ошибка: введите корректные размеры n и m");
                break;
            }

            WCHAR bufferRelation[8192];
            GetWindowTextW(hEditRelation, bufferRelation, 8192);

            wstringstream ss(bufferRelation);
            wstring line;
            graphMatrix.clear();

            bool error = false;
            while (getline(ss, line)) {
                if (line.empty()) continue;
                wstringstream lineStream(line);
                vector<int> row;
                int val;
                while (lineStream >> val) {
                    row.push_back(val);
                }
                if ((int)row.size() != m) {
                    SetWindowTextW(hOutput, L"Ошибка: количество столбцов в строке не совпадает с m");
                    graphMatrix.clear();
                    error = true;
                    break;
                }
                graphMatrix.push_back(row);
            }
            if (error) break;

            if ((int)graphMatrix.size() != n) {
                SetWindowTextW(hOutput, L"Ошибка: количество строк матрицы не совпадает с n");
                graphMatrix.clear();
                break;
            }

            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    if (i != j && graphMatrix[i][j] == 0) {
                        graphMatrix[i][j] = 1e9;
                    }
                }
            }


            SetWindowTextW(hOutput, L"Матрица загружена. Отображение графа обновлено.");
            InvalidateRect(hWnd, NULL, TRUE);
        }
        else if (LOWORD(wParam) == 2) 
        {
            if (graphMatrix.empty()) {
                SetWindowTextW(hOutput, L"Ошибка: сначала загрузите матрицу смежности");
                break;
            }

            WCHAR startBuf[16], endBuf[16];
            GetWindowTextW(hEditStart, startBuf, 16);
            GetWindowTextW(hEditEnd, endBuf, 16);

            int start = -1, end = -1;
            if (swscanf_s(startBuf, L"%d", &start) != 1 || swscanf_s(endBuf, L"%d", &end) != 1) {
                SetWindowTextW(hOutput, L"Ошибка: введите корректные номера вершин");
                break;
            }

            start -= 1;  
            end -= 1;

            int n = (int)graphMatrix.size();
            if (start < 0 || start >= n || end < 0 || end >= n) {
                SetWindowTextW(hOutput, L"Ошибка: номера вершин выходят за границы");
                break;
            }


            vector<int> parent;
            vector<int> dist = dijkstra(graphMatrix, start, parent);

            if (dist[end] >= 1e9) {
                SetWindowTextW(hOutput, L"Путь не найден.");
                break;
            }

            vector<int> path = getPath(end, parent);

            wstringstream result;
            result << L"Кратчайший путь из " << (start + 1) << L" в " << (end + 1) << L":\r\n";

            if (dist[end] >= 1e9) {
                result << L"Путь не найден.";
            }
            else {
                printPath(path, result);
                result << L"\r\nРасстояние: " << dist[end];
            }

            SetWindowTextW(hOutput, result.str().c_str());
        }
    
    break;
    }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    
    case WM_PAINT:
        {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SelectObject(hdc, hFontOutput);
        TextOutW(hdc, 10, 50, L"Размер nxm", lstrlenW(L"Размер nxm"));
        TextOutW(hdc, 40, 90, L"nxm", lstrlenW(L"nxm"));

        if (!graphMatrix.empty()) {
            const int radius = 20;
            const int nodeCount = (int)graphMatrix.size();
            const int centerX = 1200;
            const int centerY = 300;
            const int circleRadius = 200;

            vector<POINT> positions;

            for (int i = 0; i < nodeCount; ++i) {
                double angle = 2 * 3.14159265 * i / nodeCount;
                int x = (int)(centerX + circleRadius * cos(angle));
                int y = (int)(centerY + circleRadius * sin(angle));
                positions.push_back({ x, y });
                Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);
                WCHAR label[4];
                swprintf(label, 4, L"%d", i+1);
                TextOutW(hdc, x - 6, y - 6, label, lstrlenW(label));
            }

            for (int i = 0; i < nodeCount; ++i) {
                for (int j = 0; j < (int)graphMatrix[i].size(); ++j) {
                    if (graphMatrix[i][j] > 0 && graphMatrix[i][j] < 1e9) {
                        int x1 = positions[i].x;
                        int y1 = positions[i].y;
                        int x2 = positions[j].x;
                        int y2 = positions[j].y;

                        MoveToEx(hdc, x1, y1, NULL);
                        LineTo(hdc, x2, y2);

                        int dx = x2 - x1, dy = y2 - y1;
                        double angle = atan2(dy, dx);
                        int arrowSize = 10;
                        POINT arrow[3] = {
                            {x2, y2},
                            {(int)(x2 - arrowSize * cos(angle - 0.3)), (int)(y2 - arrowSize * sin(angle - 0.3))},
                            {(int)(x2 - arrowSize * cos(angle + 0.3)), (int)(y2 - arrowSize * sin(angle + 0.3))}
                        };
                        Polygon(hdc, arrow, 3);
                    }
                }
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
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
