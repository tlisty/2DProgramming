// t_maeshina�pGameFrame.

#include<d3dx9.h>
#include<string>
#include<vector>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class CGameFrame
{
public:
	CGameFrame();
	~CGameFrame();

	bool Initalize(HINSTANCE aHInst,const int aCmdShow);
	void Release();
	void Update();
	void LateUpdate();
	void Draw();
	HWND CreateHWND(HINSTANCE aHInst, const int aCmdShow);

private:

	//�֐�=================================================.
	LRESULT APIENTRY WndFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//�萔=================================================.
	const int			mScrollWidth;
	const int			mScrollHeight;
	const std::string	mAppName;
	const bool			mIsFullScreen;

	//�ϐ�=================================================.
	HWND					mpHwnd;
	LPDIRECT3D9				mpD3D;
	LPDIRECT3DDEVICE9		mpD3DDevice;
	D3DPRESENT_PARAMETERS	mD3Dpp;

};

