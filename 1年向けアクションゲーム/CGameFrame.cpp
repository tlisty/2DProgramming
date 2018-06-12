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