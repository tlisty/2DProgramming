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

#define	SCRW		1280	// �E�B���h�E���iWidth
#define	SCRH		720		// �E�B���h�E�����iHeight

LPDIRECT3D9				lpD3D;		// Direct3D�C���^�[�t�F�C�X

LPDIRECT3DDEVICE9		lpD3DDevice;	// Direct3DDevice�C���^�[�t�F�C�X

D3DPRESENT_PARAMETERS d3dpp;


// ���_�t�H�[�}�b�g�̒�`
struct VERTEX
{
	D3DXVECTOR3 Pos;
	D3DCOLOR Color;
	D3DXVECTOR2 Tex;
};

////  �O���[�o���ϐ��錾

LPD3DXSPRITE lpSprite;	// �X�v���C�g
LPD3DXFONT lpFont;		// �t�H���g

bool gameFullScreen;	// �t���X�N���[���itrue,false)

// �w�i
LPDIRECT3DTEXTURE9 backTex;
D3DXMATRIX backMat, backMat2;
float backY;

// �v���C���[
LPDIRECT3DTEXTURE9 playerTex;
D3DXMATRIX playerMat;
float playerX, playerY;
int playerAnime;
int playerFlg;

// �e
LPDIRECT3DTEXTURE9 bulletTex;
const int bulletNum = 10;
D3DXMATRIX bulletMat[bulletNum];
float bulletX[bulletNum], bulletY[bulletNum];
int bulletFlg[bulletNum];
int bulletWait;

// �G
LPDIRECT3DTEXTURE9 enemyTex;
const int enemyNum = 10;
D3DXMATRIX enemyMat[ enemyNum ];
float enemyX[enemyNum], enemyY[enemyNum];
int enemyFlg[enemyNum];

// �����G�t�F�N�g
LPDIRECT3DTEXTURE9 expTex;
const int expNum = 10;
D3DXMATRIX expMat[expNum];
float expX[expNum];
float expY[expNum];
int expFlg[expNum];
int expAnime[expNum];

// �X�R�A
int gameScore;

//�o�͕���.
std::string outputString;

//�f�o�b�O�悤���C���`��.
LPD3DXLINE  pLine;
DebugRectLine rectLine;

POINT point;

#define	FVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

// �X�V����
void Update(void)
{
	// �w�i�̃X�N���[������
	backY += 3;
	if (backY >= 720)
	{
		backY -= 720;
	}
	// �s��쐬
	D3DXMatrixTranslation(&backMat, 0.0f, backY, 0.0f);
	D3DXMatrixTranslation(&backMat2, 0.0f, backY-720, 0.0f);
	
}

// 3D�`��
void Render3D(void)
{

}

// 2D�`��
void Render2D(void)
{
	//////////////////////////////////////////////////
	///	�X�v���C�g�̕`�揈��
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
	
	// �`��J�n
	lpSprite->Begin(D3DXSPRITE_ALPHABLEND);

	rectLine.Draw();
	
	/*
	/////  �w�i
	RECT rcBack = { 0,0,1280,720 };
	
	lpSprite->SetTransform(&backMat);
	lpSprite->Draw(backTex, &rcBack, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

	lpSprite->SetTransform(&backMat2);
	lpSprite->Draw(backTex, &rcBack, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

	// �`��I��
	lpSprite->End();
	*/

	// �����\��
	RECT rcScore = { 10,10,200,200 };
	
	char buf[256];
	sprintf(buf, "courol x = %d , y = %d ", point.x,point.y);
	lpFont->DrawText(NULL,                          //Sprite�̃|�C���^ NULL�ł�OK
		buf,										//�`�悷�镶����̃|�C���^���w�肵�܂��B
		-1,											//�����������܂��B-1�̏ꍇ�͑S�Ă�`�悵�܂��B
		&rcScore,								    //�`��̈�
		DT_LEFT | DT_NOCLIP,					    //�����ō������Ȃǂ�ݒ肵�܂��B
		D3DCOLOR_ARGB(255, 255, 255, 255));			//�����̐F�����߂܂��B

	std::string cordStr("#include<stdio.h>\nmain()\n{\n\tprintf(""helloworld!"")\n}");
	RECT rcCord = { 10,30,200,200 };
	lpFont->DrawText(nullptr,                          //Sprite�̃|�C���^ NULL�ł�OK
		cordStr.c_str(),										//�`�悷�镶����̃|�C���^���w�肵�܂��B
		-1,											//�����������܂��B-1�̏ꍇ�͑S�Ă�`�悵�܂��B
		&rcCord,								    //�`��̈�
		DT_LEFT | DT_NOCLIP,					    //�����ō������Ȃǂ�ݒ肵�܂��B
		D3DCOLOR_ARGB(255, 255, 0, 0));			//�����̐F�����߂܂��B


}

void DrawFrame(void)
{
	// �o�b�N�o�b�t�@�� Z �o�b�t�@���N���A
	lpD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,255),1.0f,0);


	// �X�V����
	Update();


	// �`��J�n
	lpD3DDevice->BeginScene();

	D3DXMATRIX mView,mProj;

	// ���_�s��̐ݒ�
	D3DXMatrixLookAtLH(&mView,
		&D3DXVECTOR3(0.0f,0.0f,-10.0f),	// �J�����̈ʒu
		&D3DXVECTOR3(0.0f,0.0f,0.0f),	// �J�����̎��_
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f)	// �J�����̓��̕���
	);

	// ���e�s��̐ݒ�
	D3DXMatrixPerspectiveFovLH(&mProj, D3DXToRadian(60), (float)SCRW/(float)SCRH, 1.0f, 1000.0f);

	//�s��ݒ�
	lpD3DDevice->SetTransform(D3DTS_VIEW, &mView);
	lpD3DDevice->SetTransform(D3DTS_PROJECTION, &mProj);



	// 3D�`��
	Render3D();

	// 2D�`��
	Render2D();

	

	// �`��I��
	lpD3DDevice->EndScene();

	// �o�b�N�o�b�t�@���v���C�}���o�b�t�@�ɃR�s�[
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

	gameFullScreen = false;	// �E�B���h�E���[�h

	if (gameFullScreen){
		ShowCursor(FALSE);
	}else{
		RECT rc={0,0,SCRW,SCRH};
		AdjustWindowRect(&rc,WS_OVERLAPPEDWINDOW,FALSE);
		SetWindowPos(hwnd,NULL,50,50,rc.right-rc.left,rc.bottom-rc.top,SWP_SHOWWINDOW|SWP_NOZORDER);
	}

	//---------------------DirectX Graphics�֘A-----------------------

	// Direct3D �I�u�W�F�N�g���쐬
	lpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!lpD3D)
	{
		// �I�u�W�F�N�g�쐬���s
		MessageBox(NULL,"Direct3D �̍쐬�Ɏ��s���܂����B","ERROR",MB_OK | MB_ICONSTOP);
		// �I������
		return 0;
	}
	int adapter;

	// �g�p����A�_�v�^�ԍ�
	adapter = D3DADAPTER_DEFAULT;

	// �E�C���h�E�̍쐬�����������̂ŁADirect3D ������������
	ZeroMemory(&d3dpp,sizeof(D3DPRESENT_PARAMETERS));
	// Direct3D �������p�����[�^�̐ݒ�
	if (gameFullScreen)
	{
		// �t���X�N���[�� : �قƂ�ǂ̃A�_�v�^�ŃT�|�[�g����Ă���t�H�[�}�b�g���g�p
		d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	}
	else
	{
		// �E�C���h�E : ���݂̉�ʃ��[�h���g�p
		D3DDISPLAYMODE disp;
		// ���݂̉�ʃ��[�h���擾
		lpD3D->GetAdapterDisplayMode(adapter,&disp);
		d3dpp.BackBufferFormat = disp.Format;
	}
	// �\���̈�T�C�Y�̐ݒ�
	d3dpp.BackBufferWidth = SCRW;
	d3dpp.BackBufferHeight = SCRH;
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;

	if (!gameFullScreen)
	{
		// �E�C���h�E���[�h
		d3dpp.Windowed = 1;
	}

	// Z �o�b�t�@�̎����쐬
	d3dpp.EnableAutoDepthStencil = 1;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	//�ޯ��ޯ̧��ۯ��\�ɂ���(GetDC���\�ɂȂ�)
	d3dpp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	// �f�o�C�X�̍쐬 - T&L HAL
	if (FAILED(lpD3D->CreateDevice(adapter,D3DDEVTYPE_HAL,hwnd,D3DCREATE_HARDWARE_VERTEXPROCESSING,&d3dpp,&lpD3DDevice)))
	{
		// ���s�����̂� HAL �Ŏ��s
		if (FAILED(lpD3D->CreateDevice(adapter,D3DDEVTYPE_HAL,hwnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&lpD3DDevice)))
		{
			// ���s�����̂� REF �Ŏ��s
			if (FAILED(lpD3D->CreateDevice(adapter,D3DDEVTYPE_REF,hwnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&lpD3DDevice)))
			{
				// ���ǎ��s����
				MessageBox(NULL,"DirectX9���������ł��܂���B\n���Ή��̃p�\�R���Ǝv���܂��B","ERROR",MB_OK | MB_ICONSTOP);
				lpD3D->Release();
				// �I������
				return 0;
			}
		}
	}

	// �����_�����O�E�X�e�[�g��ݒ�
	// Z �o�b�t�@�L����->�O��֌W�̌v�Z�𐳊m�ɂ��Ă����
	lpD3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

	// �A���t�@�u�����f�B���O�L����
	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);

	// �A���t�@�u�����f�B���O���@��ݒ�
	lpD3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	lpD3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	// �����_�����O���̃A���t�@�l�̌v�Z���@�̐ݒ�
	lpD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	// �e�N�X�`���̐F���g�p
	lpD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	// ���_�̐F���g�p
	lpD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
	// �����_�����O���̐F�̌v�Z���@�̐ݒ�
	lpD3DDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);

	//���ʃJ�����O
	lpD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	// �t�B���^�ݒ�
	lpD3DDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
	lpD3DDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	lpD3DDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

	// ���C�g
	lpD3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

	lpD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL);
	// ���_�t�H�[�}�b�g�̐ݒ�
	lpD3DDevice->SetFVF(FVF_VERTEX);

	timeBeginPeriod(1);



	// �Q�[���Ɋւ��鏉�������� ---------------------------
	// �w�i
	LoadText(&backTex, "back.png", 1280, 720, NULL);	// �w�i

	srand(time(0));

	// �X�v���C�g�쐬
	D3DXCreateSprite(lpD3DDevice,&lpSprite);
	lpSprite->OnResetDevice();

	// �t�H���g�쐬
	D3DXCreateFont(lpD3DDevice, 20, 20, FW_REGULAR, NULL, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "�l�r �S�V�b�N", &lpFont);

	lpFont->OnResetDevice();

	//Line��������
	D3DXCreateLine(lpD3DDevice, &pLine);
	rectLine.Initialize(lpD3DDevice);
	const LONG rectLength = 50;
	RECT startRect = { 300,300,350,350 };
	for (int index = 0; index < 30; index++)
	{
		rectLine.AddRect({ 0 + ( rectLength * index ),600,50 + (rectLength * index),650 });
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
			// �J�[�\���̈ʒu�擾�e�X�g
			GetCursorPos( &point );
			ScreenToClient( hwnd , &point );
			DrawFrame();
		}
	}


	// �Q�[���Ɋւ���I������ ---------------------------
	// ���\�[�X�̉��
	backTex->Release();

	lpSprite->Release();	// �X�v���C�g
	lpFont->Release();		// �t�H���g


	timeEndPeriod(1);

	// Direct3D �I�u�W�F�N�g�����
	lpD3DDevice->Release();
	lpD3D->Release();

	return (int)msg.wParam;
}