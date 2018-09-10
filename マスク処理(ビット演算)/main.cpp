/*
		授業で使うアクションゲームベース.
		64＊64のマップチップを使う.
		横は1280の20マス.
		縦は768の12マス.
*/

#include<windows.h>
#include<stdio.h>
#include<d3dx9.h>
#include<time.h>
#include<string>
#include"CGameFrame.h"
#include "GameBase.h"
#include "DebugRectLine.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

CGameFrame * pGameFrame = nullptr;

bool gameFullScreen;	// フルスクリーン（true,false)

#define	FVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
const bool isHitRect(const RECT & aRect, const RECT & bRect)
{
	const auto result = aRect.left < bRect.right && bRect.left < aRect.right && aRect.top < bRect.bottom && bRect.top < aRect.bottom;
	return result;
}

LRESULT APIENTRY WndFunc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg){
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch(wParam){
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		return 0;

	}
	return DefWindowProc(hwnd,msg,wParam,lParam);

}

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrev,
				   LPSTR lpszCmdParam,int nCmdshow)
{
	MSG msg;
	// 標準入出力に割り当てる
	FILE* fp = NULL;

	// コンソールを作成する
	AllocConsole();
	// 現在のコード
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);


	pGameFrame = new CGameFrame();
	pGameFrame->Initialize(hInst, nCmdshow);

	while(1){
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0))
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}else{
			pGameFrame->GameLoop();
		}
	}

	pGameFrame->Release();
	return (int)msg.wParam;
}