#include "CGameFrame.h"



CGameFrame::CGameFrame()
	: mScrollWidth( 1280 )
	, mScrollHeight( 768 )
	, mAppName( "TSystemAction" )
	, mIsFullScreen( false )
	, mpHwnd( nullptr )
	, mpD3D( nullptr )
	, mpD3DDevice( nullptr )
	, mD3Dpp()
{
}

CGameFrame::~CGameFrame()
{
}

HWND CGameFrame::CreateHWND(HINSTANCE aHInst, const int aCmdShow)
{
	HWND result;
	result = CreateWindowEx(
		0,
		mAppName.c_str(),
		"Direct X",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		mScrollWidth, mScrollHeight,
		NULL, NULL, aHInst,
		NULL);

	if (!result)return nullptr;

	ShowWindow(result, aCmdShow);
	UpdateWindow(result);
	SetFocus(result);

	if (mIsFullScreen) {
		ShowCursor(FALSE);
	}
	else {
		RECT rc = { 0,0,mScrollWidth,mScrollHeight };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		SetWindowPos(result, NULL, 50, 50, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW | SWP_NOZORDER);
	}

	return result;
}

WNDCLASS CGameFrame::CreateWNDCLASS(HINSTANCE aHInst)
{
	WNDCLASS wc;
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WndFunc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = aHInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = mAppName.c_str();

	RegisterClass(&wc);
}

LRESULT APIENTRY WndFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		return 0;

	}
	return DefWindowProc(hwnd, msg, wParam, lParam);

}