#include<windows.h>
#include<stdio.h>
#include<d3dx9.h>
#include<time.h>
#include<string>
#include "GameBase.h"
#include "DebugRectLine.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#define	SCRW		1280	// ウィンドウ幅（Width
#define	SCRH		720		// ウィンドウ高さ（Height

LPDIRECT3D9				lpD3D;		// Direct3Dインターフェイス

LPDIRECT3DDEVICE9		lpD3DDevice;	// Direct3DDeviceインターフェイス

D3DPRESENT_PARAMETERS d3dpp;


// 頂点フォーマットの定義
struct VERTEX
{
	D3DXVECTOR3 Pos;
	D3DCOLOR Color;
	D3DXVECTOR2 Tex;
};

////  グローバル変数宣言

LPD3DXSPRITE lpSprite;	// スプライト
LPD3DXFONT lpFont;		// フォント

bool gameFullScreen;	// フルスクリーン（true,false)

// 背景
LPDIRECT3DTEXTURE9 backTex;
D3DXMATRIX backMat, backMat2;
float backY;

// プレイヤー
LPDIRECT3DTEXTURE9 playerTex;
D3DXMATRIX playerMat;
float playerX, playerY;
int playerAnime;
int playerFlg;

// 弾
LPDIRECT3DTEXTURE9 bulletTex;
const int bulletNum = 10;
D3DXMATRIX bulletMat[bulletNum];
float bulletX[bulletNum], bulletY[bulletNum];
int bulletFlg[bulletNum];
int bulletWait;

// 敵
LPDIRECT3DTEXTURE9 enemyTex;
const int enemyNum = 10;
D3DXMATRIX enemyMat[ enemyNum ];
float enemyX[enemyNum], enemyY[enemyNum];
int enemyFlg[enemyNum];

// 爆発エフェクト
LPDIRECT3DTEXTURE9 expTex;
const int expNum = 10;
D3DXMATRIX expMat[expNum];
float expX[expNum];
float expY[expNum];
int expFlg[expNum];
int expAnime[expNum];

// スコア
int gameScore;

//出力文字.
std::string outputString;

//デバッグようライン描画.
LPD3DXLINE  pLine;
std::vector< DebugRectLine * > rectLineList;
std::vector< std::vector< DebugRectLine* > > rectBlockList;
DebugRectLine rectPlayer;
RECT rectPlayerPos;
const LONG rectLength = 50;

const LONG groundY = 600;
D3DXVECTOR2 playerAddPos(0,3);
bool jumpFlg = false;

POINT point;

#define	FVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

const bool isHitRect(const RECT & aRect, const RECT & bRect)
{
	const auto result = aRect.left < bRect.right && bRect.left < aRect.right && aRect.top < bRect.bottom && bRect.top < aRect.bottom;
	return result;
}

//行列に確定させる更新処理.
void LateUpdate()
{
	rectPlayer.SetRect(rectPlayerPos);
	// 行列作成
	D3DXMatrixTranslation(&backMat, 0.0f, backY, 0.0f);
	D3DXMatrixTranslation(&backMat2, 0.0f, backY - 720, 0.0f);
}

// 更新処理
void Update(void)
{
	// 背景のスクロール処理
	backY += 3;
	if (backY >= 720)
	{
		backY -= 720;
	}

	playerAddPos.x = 0.F;
	
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		playerAddPos.x = 3.F;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		playerAddPos.x = -3.F;
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000 && jumpFlg == true)
	{
		jumpFlg = false;
		playerAddPos.y = -12.F;
	}

	if (jumpFlg == false)
	{
		playerAddPos.y += 0.9f;
	}
	else
	{
		playerAddPos.y = 0;
	}
	for (const auto list : rectBlockList)
	{
		for (const auto block : list)
		{
			if (block == nullptr) { continue; }
			const auto & blockRect = block->GetRect();

			if (rectPlayerPos.right > blockRect.left && blockRect.right > rectPlayerPos.left)
			{
				//上側判定.
				if (blockRect.bottom > rectPlayerPos.top + playerAddPos.y && rectPlayerPos.top > blockRect.top)
				{
					playerAddPos.y = blockRect.bottom - (rectPlayerPos.top + playerAddPos.y);

				}
				//下側判定.
				if (rectPlayerPos.bottom + playerAddPos.y > blockRect.top && blockRect.bottom > rectPlayerPos.bottom)
				{
					playerAddPos.y = blockRect.top - rectPlayerPos.bottom;
					jumpFlg = true;
				}
			}

			if (rectPlayerPos.bottom > blockRect.top && blockRect.bottom > rectPlayerPos.top)
			{
				//左側判定.
				if (blockRect.right > rectPlayerPos.left + playerAddPos.x && rectPlayerPos.left > blockRect.left)
				{
					playerAddPos.x = blockRect.right - (rectPlayerPos.left + playerAddPos.x);
				}
				//右側判定.
				if (rectPlayerPos.right + playerAddPos.x > blockRect.left && blockRect.right > rectPlayerPos.right)
				{
					playerAddPos.x = blockRect.left - (rectPlayerPos.right + playerAddPos.x);
				}
			}
			/*
			const auto isHit = isHitRect(rectPlayerPos, blockRect);
			if (isHit)
			{
				if (playerAddPos.y < 0  && rectPlayerPos.top < blockRect.bottom)
				{
					rectPlayerPos.bottom = blockRect.bottom + rectLength;
					rectPlayerPos.top = blockRect.bottom;
					playerAddPos.y = 0;
				}
				else if(playerAddPos.y > 0 && rectPlayerPos.bottom > blockRect.top)
				{
					rectPlayerPos.bottom = blockRect.top ;
					rectPlayerPos.top = blockRect.top - rectLength;
					playerAddPos.y = 0;
					jumpFlg = true;
				}
				
				
				if (playerAddPos.x < 0 && rectPlayerPos.left < blockRect.right)
				{
					rectPlayerPos.right = blockRect.right + rectLength;
					rectPlayerPos.left = blockRect.right;
					playerAddPos.x = 0;
				}
				else if(playerAddPos.x > 0 && rectPlayerPos.right > blockRect.left)
				{
					rectPlayerPos.right = blockRect.left;
					rectPlayerPos.left = blockRect.left - rectLength; 
					playerAddPos.x = 0;
				}
				rectPlayer.SetRect(rectPlayerPos);
				//playerAddPos = D3DXVECTOR2(0, 0);
			}
			*/
		}
	}

	rectPlayerPos.left += playerAddPos.x;
	rectPlayerPos.right += playerAddPos.x;
	rectPlayerPos.top += playerAddPos.y;
	rectPlayerPos.bottom += playerAddPos.y;

	if (groundY > rectPlayerPos.bottom + playerAddPos.y)
	{
		rectPlayerPos.top += playerAddPos.y;
		rectPlayerPos.bottom += playerAddPos.y;
	}
	else
	{
		rectPlayerPos.top = groundY - 50;
		rectPlayerPos.bottom = groundY;
		jumpFlg = true;
	}
}

// 3D描画
void Render3D(void)
{

}

// 2D描画
void Render2D(void)
{
	//////////////////////////////////////////////////
	///	スプライトの描画処理
	//////////////////////////////////////////////////

	D3DXVECTOR2 vec[] = {
		D3DXVECTOR2(300,100),
		D3DXVECTOR2(400,100),
	};
	pLine->SetAntialias(TRUE);
	pLine->SetWidth(1);
	pLine->Begin();
	pLine->Draw(vec, 2, D3DCOLOR_ARGB(255, 255, 0, 0));
	pLine->End();
	
	// 描画開始
	lpSprite->Begin(D3DXSPRITE_ALPHABLEND);

	for( const auto & rectLine : rectLineList )
	{
		rectLine->Draw();
	}
	for( const auto rectList : rectBlockList )
	{
		for (const auto & rect : rectList)
		{
			if (rect == nullptr) { continue; }
			rect->Draw();
		}
	}
	rectPlayer.Draw();
	
	/*
	/////  背景
	RECT rcBack = { 0,0,1280,720 };
	
	lpSprite->SetTransform(&backMat);
	lpSprite->Draw(backTex, &rcBack, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

	lpSprite->SetTransform(&backMat2);
	lpSprite->Draw(backTex, &rcBack, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

	// 描画終了
	lpSprite->End();
	*/

	// 文字表示
	RECT rcScore = { 10,10,200,200 };
	
	char buf[256];
	sprintf(buf, "courol x = %d , y = %d ", point.x,point.y);
	lpFont->DrawText(NULL,                          //Spriteのポインタ NULLでもOK
		buf,										//描画する文字列のポインタを指定します。
		-1,											//文字数をします。-1の場合は全てを描画します。
		&rcScore,								    //描画領域
		DT_LEFT | DT_NOCLIP,					    //ここで左揃えなどを設定します。
		D3DCOLOR_ARGB(255, 255, 255, 255));			//文字の色を決めます。

	std::string cordStr("#include<stdio.h>\nmain()\n{\n\tprintf(""helloworld!"")\n}");
	RECT rcCord = { 10,30,200,200 };
	lpFont->DrawText(nullptr,                          //Spriteのポインタ NULLでもOK
		cordStr.c_str(),										//描画する文字列のポインタを指定します。
		-1,											//文字数をします。-1の場合は全てを描画します。
		&rcCord,								    //描画領域
		DT_LEFT | DT_NOCLIP,					    //ここで左揃えなどを設定します。
		D3DCOLOR_ARGB(255, 255, 0, 0));			//文字の色を決めます。


}

void DrawFrame(void)
{
	// バックバッファと Z バッファをクリア
	lpD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,255),1.0f,0);


	// 更新処理
	Update();
	LateUpdate();

	// 描画開始
	lpD3DDevice->BeginScene();

	D3DXMATRIX mView,mProj;

	// 視点行列の設定
	D3DXMatrixLookAtLH(&mView,
		&D3DXVECTOR3(0.0f,0.0f,-10.0f),	// カメラの位置
		&D3DXVECTOR3(0.0f,0.0f,0.0f),	// カメラの視点
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f)	// カメラの頭の方向
	);

	// 投影行列の設定
	D3DXMatrixPerspectiveFovLH(&mProj, D3DXToRadian(60), (float)SCRW/(float)SCRH, 1.0f, 1000.0f);

	//行列設定
	lpD3DDevice->SetTransform(D3DTS_VIEW, &mView);
	lpD3DDevice->SetTransform(D3DTS_PROJECTION, &mProj);



	// 3D描画
	Render3D();

	// 2D描画
	Render2D();

	

	// 描画終了
	lpD3DDevice->EndScene();

	// バックバッファをプライマリバッファにコピー
	lpD3DDevice->Present(NULL,NULL,NULL,NULL);
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

void LoadText(LPDIRECT3DTEXTURE9 *lpTex,char fname[],int W,int H,D3DCOLOR Color)
{
	if(W==0)W=D3DX_DEFAULT;
	if(H==0)H=D3DX_DEFAULT;
	D3DXCreateTextureFromFileEx(lpD3DDevice,fname,W,H,1,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,Color,NULL,NULL,lpTex);
}

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrev,
				   LPSTR lpszCmdParam,int nCmdshow)
{
	MSG msg;

	HWND hwnd;
	WNDCLASS wc;
	char szAppName[]="Generic Game SDK Window";

	wc.style=CS_DBLCLKS;
	wc.lpfnWndProc=WndFunc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hInst;
	wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName=NULL;
	wc.lpszClassName=szAppName;

	RegisterClass(&wc);

	hwnd=CreateWindowEx(
		0,
		szAppName,
		"Direct X",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCRW,SCRH,
		NULL,NULL,hInst,
		NULL);

	if(!hwnd)return FALSE;

	ShowWindow(hwnd,nCmdshow);
	UpdateWindow(hwnd);
	SetFocus(hwnd);

	gameFullScreen = false;	// ウィンドウモード

	if (gameFullScreen){
		ShowCursor(FALSE);
	}else{
		RECT rc={0,0,SCRW,SCRH};
		AdjustWindowRect(&rc,WS_OVERLAPPEDWINDOW,FALSE);
		SetWindowPos(hwnd,NULL,50,50,rc.right-rc.left,rc.bottom-rc.top,SWP_SHOWWINDOW|SWP_NOZORDER);
	}

	//---------------------DirectX Graphics関連-----------------------

	// Direct3D オブジェクトを作成
	lpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!lpD3D)
	{
		// オブジェクト作成失敗
		MessageBox(NULL,"Direct3D の作成に失敗しました。","ERROR",MB_OK | MB_ICONSTOP);
		// 終了する
		return 0;
	}
	int adapter;

	// 使用するアダプタ番号
	adapter = D3DADAPTER_DEFAULT;

	// ウインドウの作成が完了したので、Direct3D を初期化する
	ZeroMemory(&d3dpp,sizeof(D3DPRESENT_PARAMETERS));
	// Direct3D 初期化パラメータの設定
	if (gameFullScreen)
	{
		// フルスクリーン : ほとんどのアダプタでサポートされているフォーマットを使用
		d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	}
	else
	{
		// ウインドウ : 現在の画面モードを使用
		D3DDISPLAYMODE disp;
		// 現在の画面モードを取得
		lpD3D->GetAdapterDisplayMode(adapter,&disp);
		d3dpp.BackBufferFormat = disp.Format;
	}
	// 表示領域サイズの設定
	d3dpp.BackBufferWidth = SCRW;
	d3dpp.BackBufferHeight = SCRH;
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;

	if (!gameFullScreen)
	{
		// ウインドウモード
		d3dpp.Windowed = 1;
	}

	// Z バッファの自動作成
	d3dpp.EnableAutoDepthStencil = 1;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	//ﾊﾞｯｸﾊﾞｯﾌｧをﾛｯｸ可能にする(GetDCも可能になる)
	d3dpp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	// デバイスの作成 - T&L HAL
	if (FAILED(lpD3D->CreateDevice(adapter,D3DDEVTYPE_HAL,hwnd,D3DCREATE_HARDWARE_VERTEXPROCESSING,&d3dpp,&lpD3DDevice)))
	{
		// 失敗したので HAL で試行
		if (FAILED(lpD3D->CreateDevice(adapter,D3DDEVTYPE_HAL,hwnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&lpD3DDevice)))
		{
			// 失敗したので REF で試行
			if (FAILED(lpD3D->CreateDevice(adapter,D3DDEVTYPE_REF,hwnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&lpD3DDevice)))
			{
				// 結局失敗した
				MessageBox(NULL,"DirectX9が初期化できません。\n未対応のパソコンと思われます。","ERROR",MB_OK | MB_ICONSTOP);
				lpD3D->Release();
				// 終了する
				return 0;
			}
		}
	}

	// レンダリング・ステートを設定
	// Z バッファ有効化->前後関係の計算を正確にしてくれる
	lpD3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

	// アルファブレンディング有効化
	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);

	// アルファブレンディング方法を設定
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	// レンダリング時のアルファ値の計算方法の設定
	lpD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	// テクスチャの色を使用
	lpD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	// 頂点の色を使用
	lpD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
	// レンダリング時の色の計算方法の設定
	lpD3DDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);

	//裏面カリング
	lpD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	// フィルタ設定
	lpD3DDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
	lpD3DDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	lpD3DDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

	// ライト
	lpD3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

	lpD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL);
	// 頂点フォーマットの設定
	lpD3DDevice->SetFVF(FVF_VERTEX);

	timeBeginPeriod(1);



	// ゲームに関する初期化処理 ---------------------------

	// 背景
	LoadText(&backTex, "back.png", 1280, 720, NULL);	// 背景

	srand(time(0));

	// スプライト作成
	D3DXCreateSprite(lpD3DDevice,&lpSprite);
	lpSprite->OnResetDevice();

	// フォント作成
	D3DXCreateFont(lpD3DDevice, 20, 20, FW_REGULAR, NULL, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "ＭＳ ゴシック", &lpFont);

	lpFont->OnResetDevice();

	//Lineさくせい
	D3DXCreateLine(lpD3DDevice, &pLine);
	rectPlayer.Initialize(lpD3DDevice);


	const LONG playerX = 100;
	const LONG playerY = 200;
	rectPlayerPos = { playerX , playerY , playerX + rectLength , playerY + rectLength };
	rectPlayer.SetRect(rectPlayerPos);
	for (int index = 0; index < 30; index++)
	{
		DebugRectLine * rectLine = new DebugRectLine();
		rectLine->Initialize(lpD3DDevice);
		RECT startRect = { 0 + (rectLength * index),600,50 + (rectLength * index),650 };
		rectLine->SetRect(startRect);
		rectLineList.push_back( rectLine );
	}

	srand(time(0));
	for (int height = 0; height < 3; height++)
	{
		std::vector< DebugRectLine * > list;
		for (int width = 0; width < 10; width++)
		{
			DebugRectLine * result = nullptr;
			auto isCreate = ( (rand() % 2) == 0 );
			if (isCreate)
			{
				result = new DebugRectLine();
				result->Initialize(lpD3DDevice);
				RECT startRect = { 200 + (rectLength * width),400 + ( rectLength * height ),250 + (rectLength * width),450 + (rectLength * height) };
				result->SetRect(startRect);
				list.push_back(result);
			}
			list.push_back(result);
		}
		rectBlockList.push_back(list);
	}
	//Init();

	while(1){
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0))
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}else{
			// カーソルの位置取得テスト
			GetCursorPos( &point );
			ScreenToClient( hwnd , &point );
			DrawFrame();
		}
	}


	// ゲームに関する終了処理 ---------------------------
	// リソースの解放
	backTex->Release();

	lpSprite->Release();	// スプライト
	lpFont->Release();		// フォント


	timeEndPeriod(1);

	// Direct3D オブジェクトを解放
	lpD3DDevice->Release();
	lpD3D->Release();

	return (int)msg.wParam;
}