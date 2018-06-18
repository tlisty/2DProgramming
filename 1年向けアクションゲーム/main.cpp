/*
		授業で使うアクションゲームベース.
		64＊64のマップチップを使う.
		横は1280の20マス.
		縦は640の12マス.
*/

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

//定数.
const int scrollWidth = 1280;
const int scrollHeight = 768;
const int mapChipWidth = 64;
const int mapChipHeight = 64; 
const int mapChipRow = 12;
const int mapChipColumn = 20;
const RECT skyRect = {0,0,64,64};
const RECT wallRect = { 64,0,128,64 };
const float gravity = 0.9f;
const int speedX = 6;
const int hitRange = 1;

////  グローバル変数宣言

LPD3DXSPRITE	lpSprite;	// スプライト
LPD3DXFONT		lpFont;		// フォント

bool gameFullScreen;	// フルスクリーン（true,false)

//ベーステクスチャ.
LPDIRECT3DTEXTURE9 baseChipTex,baseCharaTex;

struct tSpriteObject
{
	LPDIRECT3DTEXTURE9	mpTex;
	D3DXVECTOR3			mPos;
	D3DXMATRIX			mMatrix;
	RECT				mRect;
};

enum eChipType
{
	eWall,
	eSky
};

struct tChipObject
{
	tSpriteObject mSpriteObject;
	eChipType     mType;
};

struct tPlayerObject
{
	tSpriteObject mSpriteObject;
	D3DXMATRIX    mTransMatrix;
	D3DXMATRIX    mScaleMatrix;
	D3DXVECTOR3	  mScaleVector;
	D3DXVECTOR3	  mAcceleVector;
};

tPlayerObject player;
tChipObject mapChipList[mapChipRow][mapChipColumn];
//std::vector<std::vector<tSpriteObject*>> mapChipList;

//出力文字.
std::string outputString;

POINT point;

#define	FVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

bool LoadMapChipFile();

const bool isHitRect(const RECT & aRect, const RECT & bRect)
{
	const auto result = aRect.left < bRect.right && bRect.left < aRect.right && aRect.top < bRect.bottom && bRect.top < aRect.bottom;
	return result;
}

//行列に確定させる更新処理.
void LateUpdate()
{
	// 行列作成
	D3DXMatrixTranslation(&player.mTransMatrix, player.mSpriteObject.mPos.x, player.mSpriteObject.mPos.y, player.mSpriteObject.mPos.z);
	D3DXMatrixScaling(&player.mScaleMatrix, player.mScaleVector.x, player.mScaleVector.y, player.mScaleVector.z);
	player.mSpriteObject.mMatrix = player.mScaleMatrix * player.mTransMatrix;
}

// 更新処理
void Update(void)
{

	player.mAcceleVector.x = 0.0f;

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		player.mAcceleVector.x += speedX;
		player.mScaleVector.x = 1.0f;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		player.mAcceleVector.x -= speedX;
		player.mScaleVector.x = -1.0f;
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		//player.mSpriteObject.mPos.y -= 3.0f;
		player.mAcceleVector.y = -15.f;
	}

	if (GetAsyncKeyState('R') & 0x8000)
	{
		LoadMapChipFile();
	}

	player.mAcceleVector.y += gravity;

	const auto nextY = player.mSpriteObject.mPos.y + player.mAcceleVector.y;
	const auto nextX = player.mSpriteObject.mPos.x + player.mAcceleVector.x;
	const auto & playerPos = player.mSpriteObject.mPos;
	//マップチップ配置の中から自分がどこにいるか判定する.
	const auto startRow = playerPos.y / mapChipHeight;
	const auto startColumn = playerPos.x / mapChipWidth;
	for (int row = startRow - hitRange; row < startRow + hitRange; row++)
	{
		for (int column = startColumn - hitRange; column < startColumn + hitRange; column++)
		{
			if (mapChipList[row][column].mType != eChipType::eWall) { continue; }

			auto & chipPos = mapChipList[row][column].mSpriteObject.mPos;
			
			//キャラのX座標がマップチップのX座標の中に入っているかチェック通ったら上下判定に入る.
			if (chipPos.x < playerPos.x + (mapChipWidth / 2) 
				&& chipPos.x + mapChipWidth > playerPos.x - (mapChipWidth / 2))
			{
				//上判定.
				if (nextY + (mapChipHeight / 2) > chipPos.y 
					&& nextY- (mapChipHeight / 2)< chipPos.y )
				{
					player.mAcceleVector.y = 0;
					player.mSpriteObject.mPos.y = chipPos.y - (mapChipHeight / 2);
					printf("upHit\n");
				}
				//下判定.
				else if (nextY - (mapChipHeight / 2) < chipPos.y + mapChipHeight 
					&& nextY + (mapChipHeight / 2)> chipPos.y + mapChipHeight)
				{
					
					player.mAcceleVector.y = 0; 
					player.mSpriteObject.mPos.y = chipPos.y + mapChipHeight + (mapChipHeight / 2);
					printf("downHit\n");
				}
			}

			//キャラのY座標がマップチップのY座標の中に入っているかチェック通ったら左右判定に入る.
			if (chipPos.y < playerPos.y + (mapChipHeight / 2)
				&& chipPos.y + mapChipHeight > playerPos.y - (mapChipHeight / 2))
			{
				//右判定.
				if (nextX + (mapChipWidth / 2) > chipPos.x
					&& nextX - (mapChipWidth / 2) < chipPos.x)
				{
					player.mAcceleVector.x = 0;
					player.mSpriteObject.mPos.x = chipPos.x - (mapChipWidth / 2);
					//printf("\nplayerPos.x = %.3f\n", playerPos.x);
					//printf("playerPos.y = %.3f\n", playerPos.y);
					printf("rightHit\n");
				}
				//左判定.
				else if (nextX - (mapChipWidth / 2) < chipPos.x + mapChipWidth
					&& nextX + (mapChipWidth / 2) > chipPos.x + mapChipWidth)
				{
					player.mAcceleVector.x = 0;
					player.mSpriteObject.mPos.x = chipPos.x + mapChipWidth + (mapChipWidth / 2);

					//printf("\nplayerPos.x = %.3f\n", playerPos.x);
					//printf("playerPos.y = %.3f\n", playerPos.y);
					printf("leftHit\n");

				}
			}
		}
	}

	player.mSpriteObject.mPos.y += player.mAcceleVector.y;
	player.mSpriteObject.mPos.x += player.mAcceleVector.x;

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
	
	// 描画開始
	lpSprite->Begin(D3DXSPRITE_ALPHABLEND);
	
	
	for (int row = 0; row < mapChipRow; row++)
	{
		for (int column = 0; column < mapChipColumn; column++)
		{
			lpSprite->SetTransform(&mapChipList[row][column].mSpriteObject.mMatrix);
			lpSprite->Draw(mapChipList[row][column].mSpriteObject.mpTex, &mapChipList[row][column].mSpriteObject.mRect, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}

	lpSprite->SetTransform(&player.mSpriteObject.mMatrix);
	lpSprite->Draw(player.mSpriteObject.mpTex, &player.mSpriteObject.mRect, &D3DXVECTOR3(32.0f, 32.0f, 0.0f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));


	// 描画終了
	lpSprite->End();
	

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

	/*
	std::string cordStr("#include<stdio.h>\nmain()\n{\n\tprintf(""helloworld!"")\n}");
	RECT rcCord = { 10,30,200,200 };
	lpFont->DrawText(nullptr,                          //Spriteのポインタ NULLでもOK
		cordStr.c_str(),										//描画する文字列のポインタを指定します。
		-1,											//文字数をします。-1の場合は全てを描画します。
		&rcCord,								    //描画領域
		DT_LEFT | DT_NOCLIP,					    //ここで左揃えなどを設定します。
		D3DCOLOR_ARGB(255, 255, 0, 0));			//文字の色を決めます。
		*/

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
	D3DXMatrixPerspectiveFovLH(&mProj, D3DXToRadian(60), (float)scrollWidth/(float)scrollHeight, 1.0f, 1000.0f);

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
		scrollWidth, scrollHeight,
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
		RECT rc={0,0,scrollWidth,scrollHeight };
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
	d3dpp.BackBufferWidth = scrollWidth;
	d3dpp.BackBufferHeight = scrollHeight;
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

	// 標準入出力に割り当てる
	FILE* fp = NULL;
	
	// コンソールを作成する
	AllocConsole();
	// 現在のコード
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
	
	// ゲームに関する初期化処理 ---------------------------
	//テクスチャ読み込み.

	LoadText(&baseChipTex, "Resorces/MapChip/chip01.png", 320, 320, NULL);	// マップチップ.
	LoadText(&baseCharaTex, "Resorces/Chara/chara.png", 64, 64, NULL);		// 自機.

	LoadMapChipFile();

	// 自機.
	//player = new tSpriteObject();
	player.mSpriteObject.mRect = { 0 , 0 , 64 , 64 };
	player.mSpriteObject.mpTex = baseCharaTex;
	player.mScaleVector = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	player.mSpriteObject.mPos = D3DXVECTOR3(150.0f,150.0f,0.0f);

	srand(time(0));

	// スプライト作成
	D3DXCreateSprite(lpD3DDevice,&lpSprite);
	lpSprite->OnResetDevice();

	// フォント作成
	D3DXCreateFont(lpD3DDevice, 20, 20, FW_REGULAR, NULL, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "ＭＳ ゴシック", &lpFont);

	lpFont->OnResetDevice();

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
	baseCharaTex->Release();
	baseChipTex->Release();

	lpSprite->Release();	// スプライト
	lpFont->Release();		// フォント


	timeEndPeriod(1);

	// Direct3D オブジェクトを解放
	lpD3DDevice->Release();
	lpD3D->Release();

	return (int)msg.wParam;
}

bool LoadMapChipFile()
{
	FILE* fp = nullptr;
	int column = 0, row = 0;
	if ((fp = fopen("test.txt", "r")) != nullptr)
	{
		char ch;
		while ((ch = fgetc(fp)) != EOF)
		{
			if (ch == '\n')
			{
				row++;
				column = 0;
			}
			else
			{
				mapChipList[row][column].mSpriteObject.mpTex = baseChipTex;
				mapChipList[row][column].mSpriteObject.mPos = D3DXVECTOR3(column * mapChipWidth, row * mapChipHeight, 0);
				D3DXMatrixTranslation(&mapChipList[row][column].mSpriteObject.mMatrix, mapChipList[row][column].mSpriteObject.mPos.x, mapChipList[row][column].mSpriteObject.mPos.y, mapChipList[row][column].mSpriteObject.mPos.z);
				mapChipList[row][column].mType = static_cast<eChipType>(ch - 0x30);
				switch (mapChipList[row][column].mType)
				{
				default:
					break;
				case eChipType::eSky:
					mapChipList[row][column].mSpriteObject.mRect = skyRect;
					break;
				case eChipType::eWall:
					mapChipList[row][column].mSpriteObject.mRect = wallRect;
					break;
				}

				column++;
			}
		}
	}
	else
	{
		_ASSERT_EXPR(false, L"ファイル読み込み失敗");
		return false;
	}
	fclose(fp);
	return true;
}