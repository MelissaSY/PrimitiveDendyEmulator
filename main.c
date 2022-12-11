#include "main.h"

#define EMU_RUN	1
#define WM_FILE_SELECT		62
#define WM_FILE_NES			63
#define WM_FILE_PAL			64
#define LEN_NAME			1024


BYTE state, opCode;

HMENU hMenu, hMainMenu;

UINT_PTR timer;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HDC backDC;
	static RECT Rect;
	PAINTSTRUCT ps;
	HBITMAP hBitmap;
	HDC hdc;
	int errorDraw;
	static RECT scrollRect;

	OPENFILENAMEW ofn;

	switch (message)
	{
	case WM_CREATE:
		ppu_read_pallete(L"current_pallete.pal");
		break;
	case WM_LBUTTONDBLCLK:
		
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &Rect);

		backDC = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, Rect.right, Rect.bottom);
		SelectObject(backDC, hBitmap);
		
		drawcurrent(backDC, Rect);

		BitBlt(hdc, 0, 0, Rect.right, Rect.bottom, backDC, 0, 0, SRCCOPY);

		DeleteObject(hBitmap);
		DeleteDC(backDC);

		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_COMMAND:
			if ((UINT)LOWORD(wParam) == WM_FILE_NES) {
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFilter = L"NES files\0*.nes;*.md\0All files\0*.*\0\0";
				ofn.nFilterIndex = 1;
				LPWSTR wszFileName = (LPWSTR)calloc(LEN_NAME, sizeof(WCHAR));
				ofn.lpstrFile = wszFileName;
				ofn.nMaxFile = LEN_NAME;
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
				wszFileName[0] = L'\0';
				if (!GetOpenFileName(&ofn))
					break;
				insert_cartridge(wszFileName);
				if (timer != NULL)
				{
					KillTimer(hWnd, timer);
				}
				SetTimer(hWnd, timer, 1, NULL);
				free(wszFileName);
			}
			else if ((UINT)LOWORD(wParam) == WM_FILE_PAL) {
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFilter = L"Palettes files\0*.pal;*.md\0All files\0*.*\0\0";
				ofn.nFilterIndex = 1;
				LPWSTR wszFileName = (LPWSTR)calloc(LEN_NAME, sizeof(WCHAR));
				ofn.lpstrFile = wszFileName;
				ofn.nMaxFile = LEN_NAME;
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
				wszFileName[0] = L'\0';
				if (!GetOpenFileName(&ofn))
					break;
				ppu_read_pallete(wszFileName);
				free(wszFileName);
			}
			InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_KEYDOWN:
		if (wParam == 0x41) {
			if (GetKeyState(VK_CONTROL) == 1) {

			}
		}
		break;
	case WM_TIMER:
		clock_bus();
		if (frame_complete) {

			InvalidateRect(hWnd, NULL, FALSE);
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex; HWND hWnd; MSG msg;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"SYNES";
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);

	hWnd = CreateWindow(L"SYNES", L"SYNES", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	hMenu = CreateMenu(); 
	hMainMenu = CreateMenu();

	AppendMenu(hMenu, MF_STRING, WM_FILE_NES, L"ROM");
	AppendMenu(hMenu, MF_STRING, WM_FILE_PAL, L"Palette");
	AppendMenu(hMainMenu, MF_POPUP, (UINT_PTR)hMenu, L"File");
	SetMenu(hWnd, hMainMenu);


	init_system();

	ShowWindow(hWnd, nCmdShow);

	UpdateWindow(hWnd);
	DrawMenuBar(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	finish_system();
	return (int)msg.wParam;
}