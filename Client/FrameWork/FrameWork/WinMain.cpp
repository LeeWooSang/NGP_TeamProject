#include "MyInclude.h"
#include <crtdbg.h>
#include "FWMain.h"
#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")
#ifndef _DEBUG
#define new new(_CLIENT_BLOCK,__FILE__,__LINE)
#endif
using namespace std;
FWMain* main = &(FWMain::getMain());
int x = main->getWindowsSizeX();
int y = main->getWindowsSizeY();

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = main->getWindowsName();
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(main->getWindowsName(), main->getWindowsName(), WS_OVERLAPPEDWINDOW,
		300, 100, main->getWindowsSizeX(), main->getWindowsSizeY(),
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	delete main;
	return Message.wParam;
	//LoadBitmap(g_hInst, "D:\\FrameWork\\FrameWork\\image\\background.bmp");
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HDC memdc;
	HBITMAP hBackBit, hOldBitmap;
	switch (iMessage) {
	case WM_CREATE:
		SetTimer(hWnd, 1, 16.5, NULL);
		break;
	case WM_DESTROY:
		main->getScene()->Destroy();
		PostQuitMessage(0);
		return 0;
	case WM_TIMER:
		main->getScene()->Update();
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		memdc = CreateCompatibleDC(hdc);
		hBackBit = CreateCompatibleBitmap(hdc, x,y );
		hOldBitmap = (HBITMAP)SelectObject(memdc, hBackBit);
		main->getScene()->Render(&memdc);

		BitBlt(hdc, 0, 0, x, y, memdc, 0, 0, SRCCOPY);
		
		SelectObject(memdc, hOldBitmap);
		DeleteObject(hBackBit);
		DeleteDC(memdc);

		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:											//키 입력이나 스킬이 한번 눌리면 두번 발생하는 현상 발생중 - by 명진
	case WM_KEYUP:												//(따로 처리해야 되지 않을까 싶음)
		main->getScene()->KeyboardInput(iMessage, wParam);
		InvalidateRect(hWnd, NULL, FALSE);
		/*iMessage, wParam*/
		break;
	case WM_CHAR:
		main->getScene()->KeyboardCharInput(wParam);
		InvalidateRect(hWnd, NULL, FALSE);
		//(TCHAR)wParam;
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		main->getScene()->MouseInput(iMessage,x,y);
		break;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}