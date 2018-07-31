#include<algorithm>
#include<vector>
#include<random>

#include "CGameFrame.h"
#include "CEnemy.h"
#include "CFlyEnemy.h"
#include "Collision.h"
#include "CSpriteObject.h"
#include "PrimitiveShape.h"


// 頂点フォーマットの定義
struct VERTEX
{
	D3DXVECTOR3 Pos;
	D3DCOLOR Color;
	D3DXVECTOR2 Tex;
};

//定数.
const int mapChipWidth = 64;
const int mapChipHeight = 64;
const int mapChipRow = 12;
const int mapChipColumn = 20;
const RECT skyRect = { 0,0,64,64 };
const RECT wallRect = { 64,0,128,64 };
const float gravity = 0.9f;
const int speedX = 6;
const int hitRange = 1;


std::vector<int> intList;
std::vector<CEnemy> testEnemyList;

////  グローバル変数宣言

LPD3DXSPRITE	lpSprite;	// スプライト
LPD3DXFONT		lpFont;		// フォント

							//ベーステクスチャ.
LPDIRECT3DTEXTURE9 baseChipTex, baseCharaTex;

struct tPlayerObject
{
	tSpriteObject mSpriteObject;
	D3DXMATRIX    mTransMatrix;
	D3DXMATRIX    mScaleMatrix;
	D3DXVECTOR3	  mScaleVector;
	D3DXVECTOR3	  mAcceleVector;
	bool		  mJumpFlg;
};

tPlayerObject player;
std::vector<std::vector<tChipObject*>> mapChipList;

void CGameFrame::LoadText(LPDIRECT3DTEXTURE9 *lpTex, char fname[], int W, int H, D3DCOLOR Color)
{
	if (W == 0)W = D3DX_DEFAULT;
	if (H == 0)H = D3DX_DEFAULT;
	D3DXCreateTextureFromFileEx(mpD3DDevice, fname, W, H, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, Color, NULL, NULL, lpTex);
}

bool LoadMapChipFile()
{
	FILE* fp = nullptr;
	int column = 0, row = 0;
	std::vector<tChipObject*> list;
	if ((fp = fopen("test.txt", "r")) != nullptr)
	{
		char ch;
		while ((ch = fgetc(fp)) != EOF)
		{
			if (ch == '\n')
			{
				row++;
				column = 0;
				mapChipList.push_back(list);
				list.clear();
			}
			else
			{
				auto mapChiObject = new tChipObject();
				mapChiObject->mSpriteObject.mpTex = baseChipTex;
				mapChiObject->mSpriteObject.mPos = D3DXVECTOR3(column * mapChipWidth, row * mapChipHeight, 0);
				D3DXMatrixTranslation(&mapChiObject->mSpriteObject.mMatrix, mapChiObject->mSpriteObject.mPos.x, mapChiObject->mSpriteObject.mPos.y, mapChiObject->mSpriteObject.mPos.z);
				mapChiObject->mType = static_cast<eChipType>(ch - 0x30);
				switch (mapChiObject->mType)
				{
				default:
					break;
				case eChipType::eSky:
					mapChiObject->mSpriteObject.mRect = skyRect;
					break;
				case eChipType::eWall:
					mapChiObject->mSpriteObject.mRect = wallRect;
					break;
				}
				list.push_back(mapChiObject);
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

LRESULT _stdcall WindowFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

CGameFrame::CGameFrame()
	: mScrollWidth( 1280 )
	, mScrollHeight( 768 )
	, mAppName( "TSystemAction" )
	, mIsFullScreen( false )
	, mFvFVertex(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
	, mpHwnd( nullptr )
	, mpD3D( nullptr )
	, mpD3DDevice( nullptr )
	, mD3Dpp()
{
}

CGameFrame::~CGameFrame()
{
}

bool CGameFrame::Initialize(HINSTANCE aHInst, const int aCmdShow)
{
	mWndClass = CreateWNDCLASS(aHInst);
	mpHwnd = CreateHWND( aHInst , aCmdShow );
	assert( mpHwnd != nullptr , "not create mpHwnd" );
	CreateDirectX9();

	LoadText(&baseChipTex, "Resorces/MapChip/chip01.png", 320, 320, NULL);	// マップチップ.
	LoadText(&baseCharaTex, "Resorces/Chara/chara.png", 64, 64, NULL);		// 自機.

	LoadMapChipFile();

	srand(timeGetTime());

	// 自機.
	//player = new tSpriteObject();
	player.mSpriteObject.mRect = { 0 , 0 , 64 , 64 };
	player.mSpriteObject.mpTex = baseCharaTex;
	player.mScaleVector = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	player.mSpriteObject.mPos = D3DXVECTOR3(150.0f, 150.0f, 0.0f);
	player.mSpriteObject.mFlg = true;

	// スプライト作成
	D3DXCreateSprite(mpD3DDevice,&lpSprite);
	lpSprite->OnResetDevice();

	LPDIRECT3DTEXTURE9 enemyTexture;
	LoadText(&enemyTexture, "Resorces/kuroTenemy.png", 64, 64, D3DCOLOR_ARGB(255, 255, 255, 255));

	//敵生成.
	auto * enemy = CEnemy::Create();
	mEnemyList.push_back(enemy);
	enemy->SetTexture( enemyTexture );
	enemy->SetPosition(D3DXVECTOR2(300.0f,500.0f));
	enemy->SetMovePoint(75.0f);
	enemy->SetMoveState(CEnemy::eMoveState::eRight);
	enemy->SetMoveVector(D3DXVECTOR2(1.0f, 0.0f));

	mpFlyEnemy = &CFlyEnemy::Create()
		.Texture(enemyTexture)
		.StartPosition(D3DXVECTOR2(400.0f, 400.0f));

	return true;
}

void CGameFrame::Release()
{
	delete(this);
}

void CGameFrame::Update()
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

	if (GetAsyncKeyState(VK_SPACE) & 0x8000 && player.mJumpFlg)
	{
		//player.mSpriteObject.mPos.y -= 3.0f;
		player.mAcceleVector.y = -20.0f;
		player.mJumpFlg = false;
	}

	if (GetAsyncKeyState('R') & 0x8000)
	{
		LoadMapChipFile();
		player.mSpriteObject.mFlg = true;
		//if( mEnemyList.empty() )
		{
			auto * enemy = CEnemy::Create();
			mEnemyList.push_back(enemy);

			LPDIRECT3DTEXTURE9 enemyTexture;
			LoadText(&enemyTexture, "Resorces/kuroTenemy.png", 64, 64, D3DCOLOR_ARGB(255, 255, 255, 255));
			enemy->SetTexture(enemyTexture);
			enemy->SetPosition(D3DXVECTOR2(rand() % 1100 + 64 , rand() % 600 + 64));
			enemy->SetMovePoint(75.0f);
			enemy->SetMoveState(CEnemy::eMoveState::eRight);
			enemy->SetMoveVector(D3DXVECTOR2(1.0f, 0.0f));
		}
	}

	player.mAcceleVector.y += gravity;

	const auto & playerPos = player.mSpriteObject.mPos;
	

	if (player.mSpriteObject.mFlg)
	{
		//敵と自機の当たり判定.
		for (const auto enemy : mEnemyList)
		{
			const auto result = RectCollision(D3DXVECTOR2(playerPos.x, playerPos.y), D3DXVECTOR2(player.mAcceleVector.x, player.mAcceleVector.y)
				, D3DXVECTOR3(32.0f, 32.0f, 32.0f), enemy->GetPosition(), enemy->GetCenterPoint());
			switch (result)
			{
			default:
			case eHitDirection::eNone:
				break;
			case eHitDirection::eUp:
				if(player.mAcceleVector.y > 0.0f )
				{
					printf("\nenemy break\n");
					player.mAcceleVector.y = -10.f;
					delete(enemy);
					auto result = std::remove(mEnemyList.begin(), mEnemyList.end() , enemy);
					mEnemyList.erase(result);
					printf("moveVector.y = %.3f ", player.mAcceleVector.y);
				}
				else
				{
					printf("\nplayer up break\n");
					player.mSpriteObject.mFlg = false;

				}
				break;
			case eHitDirection::eDown:
			case eHitDirection::eLeft:
			case eHitDirection::eRight:
				printf("\nplayer break\n");
				player.mSpriteObject.mFlg = false;
				break;
			}
		}
	}

	const auto nextY = player.mSpriteObject.mPos.y + player.mAcceleVector.y;
	const auto nextX = player.mSpriteObject.mPos.x + player.mAcceleVector.x;

	//マップチップ配置の中から自分がどこにいるか判定する.
	const auto startRow = playerPos.y / mapChipHeight;
	const auto startColumn = playerPos.x / mapChipWidth;
	for (int row = startRow - hitRange; row < startRow + hitRange; row++)
	{
		for (int column = startColumn - hitRange; column < startColumn + hitRange; column++)
		{
			if (mapChipList[row][column]->mType != eChipType::eWall) {continue; }

			auto & chipPos = mapChipList[row][column]->mSpriteObject.mPos;

			//キャラのX座標がマップチップのX座標の中に入っているかチェック通ったら上下判定に入る.
			if (chipPos.x < playerPos.x + (mapChipWidth / 2)
				&& chipPos.x + mapChipWidth > playerPos.x - (mapChipWidth / 2))
			{
				//上判定.
				//下辺だけの判定だとターゲットの下にいるときも判定してしまうので上辺がターゲットの上辺より上にいるかも確認する.
				if (nextY + (mapChipHeight / 2) > chipPos.y
					&& nextY - (mapChipHeight / 2)< chipPos.y)
				{
					player.mAcceleVector.y = 0;
					player.mSpriteObject.mPos.y = chipPos.y - (mapChipHeight / 2);
					player.mJumpFlg = true;
				}
				//下判定.
				//上辺だけの判定だとターゲットの上にいるときも判定してしまうので下辺がターゲットの下辺より上いるかも確認する.
				else if (nextY - (mapChipHeight / 2) < chipPos.y + mapChipHeight
					&& nextY + (mapChipHeight / 2)> chipPos.y + mapChipHeight)
				{

					player.mAcceleVector.y = 0;
					player.mSpriteObject.mPos.y = chipPos.y + mapChipHeight + (mapChipHeight / 2);
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
				}
				//左判定.
				else if (nextX - (mapChipWidth / 2) < chipPos.x + mapChipWidth
					&& nextX + (mapChipWidth / 2) > chipPos.x + mapChipWidth)
				{
					player.mAcceleVector.x = 0;
					player.mSpriteObject.mPos.x = chipPos.x + mapChipWidth + (mapChipWidth / 2);
				}
			}
		}
	}

	player.mSpriteObject.mPos.y += player.mAcceleVector.y;
	player.mSpriteObject.mPos.x += player.mAcceleVector.x;

	for (auto * enemy : mEnemyList)
	{
		enemy->Update(mapChipList);
	}

	mpFlyEnemy->Update();

}

void CGameFrame::LateUpdate()
{
	D3DXMatrixTranslation(&player.mTransMatrix, player.mSpriteObject.mPos.x, player.mSpriteObject.mPos.y, player.mSpriteObject.mPos.z);
	D3DXMatrixScaling(&player.mScaleMatrix, player.mScaleVector.x, player.mScaleVector.y, player.mScaleVector.z);
	player.mSpriteObject.mMatrix = player.mScaleMatrix * player.mTransMatrix;

	for (auto * enemy : mEnemyList)
	{
		enemy->LateUpdate();
	}
	mpFlyEnemy->LateUpdate();
}

void CGameFrame::Draw()
{
	Draw3D();
	Draw2D();
}

void CGameFrame::GameLoop()
{
	// バックバッファと Z バッファをクリア
	mpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);


	Update();
	LateUpdate();

	// 描画開始
	mpD3DDevice->BeginScene();

	D3DXMATRIX mView, mProj;

	// 視点行列の設定
	D3DXMatrixLookAtLH(&mView,
		&D3DXVECTOR3(0.0f, 0.0f, -10.0f),	// カメラの位置
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// カメラの視点
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f)	// カメラの頭の方向
	);

	// 投影行列の設定
	D3DXMatrixPerspectiveFovLH(&mProj, D3DXToRadian(60), (float)mScrollWidth / (float)mScrollHeight, 1.0f, 1000.0f);

	//行列設定
	mpD3DDevice->SetTransform(D3DTS_VIEW, &mView);
	mpD3DDevice->SetTransform(D3DTS_PROJECTION, &mProj);

	Draw();

	// 描画終了
	mpD3DDevice->EndScene();

	// バックバッファをプライマリバッファにコピー
	mpD3DDevice->Present(NULL, NULL, NULL, NULL);
}

void CGameFrame::Draw2D()
{
	// 描画開始
	lpSprite->Begin(D3DXSPRITE_ALPHABLEND);


	for (const auto & list : mapChipList)
	{
		for (const auto * mapChip : list)
		{
			lpSprite->SetTransform(&mapChip->mSpriteObject.mMatrix);
			//lpSprite->Draw(mapChip->mSpriteObject.mpTex, &mapChip->mSpriteObject.mRect, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
	/*
	for (int row = 0; row < mapChipRow; row++)
	{
		for (int column = 0; column < mapChipColumn; column++)
		{
			lpSprite->SetTransform(&mapChipList[row][column].mSpriteObject.mMatrix);
			lpSprite->Draw(mapChipList[row][column].mSpriteObject.mpTex, &mapChipList[row][column].mSpriteObject.mRect, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
	*/

	for (auto * enemy : mEnemyList)
	{
		enemy->Draw(lpSprite);
	}

	mpFlyEnemy->Draw(lpSprite);

	if(player.mSpriteObject.mFlg )
	{
		lpSprite->SetTransform(&player.mSpriteObject.mMatrix);
		lpSprite->Draw(player.mSpriteObject.mpTex, &player.mSpriteObject.mRect, &D3DXVECTOR3(32.0f, 32.0f, 0.0f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	// 描画終了
	lpSprite->End();
}

void CGameFrame::Draw3D()
{
	Primitive::Draw::Triangle(mpD3DDevice, D3DXVECTOR3(0.0f, 0.0f,0.0f), D3DXVECTOR3(0.5f, -0.5f,0.0f), D3DXVECTOR3(-0.5f,-0.5f, 0.0f), Color::Black);
	Primitive::Draw::Polygon(mpD3DDevice,3, D3DXVECTOR3(0.5f, 0.5f, 0.0f),1,0,true);
	Primitive::Draw::Rect(mpD3DDevice,0.0f,0.0f,2.0f,1.0f,90.0f,Color::Green);
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
	WNDCLASS result;
	result.style = CS_DBLCLKS;
	result.lpfnWndProc = WindowFunc;
	result.cbClsExtra = 0;
	result.cbWndExtra = 0;
	result.hInstance = aHInst;
	result.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	result.hCursor = LoadCursor(NULL, IDC_ARROW);
	result.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	result.lpszMenuName = NULL;
	result.lpszClassName = mAppName.c_str();

	RegisterClass(&result);
	return result;
}

bool CGameFrame::CreateDirectX9()
{
	// Direct3D オブジェクトを作成
	mpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if( mpD3D == nullptr )
	{
		// オブジェクト作成失敗
		MessageBox(NULL, "Direct3D の作成に失敗しました。", "ERROR", MB_OK | MB_ICONSTOP);
		// 終了する
		return false;
	}
	int adapter;

	// 使用するアダプタ番号
	adapter = D3DADAPTER_DEFAULT;

	// ウインドウの作成が完了したので、Direct3D を初期化する
	ZeroMemory(&mD3Dpp, sizeof(D3DPRESENT_PARAMETERS));
	// Direct3D 初期化パラメータの設定
	if (mIsFullScreen)
	{
		// フルスクリーン : ほとんどのアダプタでサポートされているフォーマットを使用
		mD3Dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	}
	else
	{
		// ウインドウ : 現在の画面モードを使用
		D3DDISPLAYMODE disp;
		// 現在の画面モードを取得
		mpD3D->GetAdapterDisplayMode(adapter, &disp);
		mD3Dpp.BackBufferFormat = disp.Format;
	}
	// 表示領域サイズの設定
	mD3Dpp.BackBufferWidth = mScrollWidth;
	mD3Dpp.BackBufferHeight = mScrollHeight;
	mD3Dpp.SwapEffect = D3DSWAPEFFECT_FLIP;

	if (mIsFullScreen == false)
	{
		// ウインドウモード
		mD3Dpp.Windowed = 1;
	}

	// Z バッファの自動作成
	mD3Dpp.EnableAutoDepthStencil = 1;
	mD3Dpp.AutoDepthStencilFormat = D3DFMT_D16;

	//ﾊﾞｯｸﾊﾞｯﾌｧをﾛｯｸ可能にする(GetDCも可能になる)
	mD3Dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	// デバイスの作成 - T&L HAL
	if (FAILED(mpD3D->CreateDevice(adapter, D3DDEVTYPE_HAL, mpHwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &mD3Dpp, &mpD3DDevice)))
	{
		// 失敗したので HAL で試行
		if (FAILED(mpD3D->CreateDevice(adapter, D3DDEVTYPE_HAL, mpHwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &mD3Dpp, &mpD3DDevice)))
		{
			// 失敗したので REF で試行
			if (FAILED(mpD3D->CreateDevice(adapter, D3DDEVTYPE_REF, mpHwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &mD3Dpp, &mpD3DDevice)))
			{
				// 結局失敗した
				MessageBox(NULL, "DirectX9が初期化できません。\n未対応のパソコンと思われます。", "ERROR", MB_OK | MB_ICONSTOP);
				mpD3D->Release();
				// 終了する
				return 0;
			}
		}
	}

	// レンダリング・ステートを設定
	// Z バッファ有効化->前後関係の計算を正確にしてくれる
	mpD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	mpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// アルファブレンディング有効化
	mpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	// アルファブレンディング方法を設定
	mpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	mpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// レンダリング時のアルファ値の計算方法の設定
	mpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	// テクスチャの色を使用
	mpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	// 頂点の色を使用
	mpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	// レンダリング時の色の計算方法の設定
	mpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	//裏面カリング
	mpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// フィルタ設定
	mpD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	mpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	mpD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// ライト
	mpD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	mpD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	// 頂点フォーマットの設定
	mpD3DDevice->SetFVF(mFvFVertex);
}
