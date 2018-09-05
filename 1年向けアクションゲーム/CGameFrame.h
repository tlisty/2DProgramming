// t_maeshina用GameFrame.

#include<windows.h>
#include<d3dx9.h>
#include<string>
#include<vector>
#include<cassert>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")

class CEnemy;
class CFlyEnemy;

class CGameFrame
{
public:
	CGameFrame();
	~CGameFrame();

	bool Initialize(HINSTANCE aHInst,const int aCmdShow);
	void GameLoop();
	void Release();
	//TODO:ゆくゆくはメンバ関数にする.
	//LRESULT _stdcall WndFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:

	//関数=================================================.
	//LRESULT _stdcall WndFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Update();
	void LateUpdate();
	void Draw();
	void Draw2D();
	void Draw3D();
	void LoadText(LPDIRECT3DTEXTURE9 *lpTex, char fname[], int W, int H, D3DCOLOR Color);
	HWND CreateHWND(HINSTANCE aHInst, const int aCmdShow);
	WNDCLASS CreateWNDCLASS(HINSTANCE aHInst);
	bool CreateDirectX9();

	//定数=================================================.
	const int			mScrollWidth;
	const int			mScrollHeight;
	const std::string	mAppName;
	const bool			mIsFullScreen;
	const int			mFvFVertex;

	//変数=================================================.
	HWND					mpHwnd;
	LPDIRECT3D9				mpD3D;
	LPDIRECT3DDEVICE9		mpD3DDevice;
	D3DPRESENT_PARAMETERS	mD3Dpp;
	WNDCLASS				mWndClass;
	std::vector<CEnemy*>	mEnemyList;
	CFlyEnemy *				mpFlyEnemy;
};
