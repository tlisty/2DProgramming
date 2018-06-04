/*
		���ƂŎg���A�N�V�����Q�[���x�[�X.
		64��64�̃}�b�v�`�b�v���g��.
		����1280��20�}�X.
		�c��640��12�}�X.
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

//�萔.
const int scrollWidth = 1280;
const int scrollHeight = 768;
const int mapChipWidth = 64;
const int mapChipHeight = 64; 
const int mapChipRow = 12;
const int mapChipColumn = 20;
const RECT skyRect = {0,0,64,64};
const RECT wallRect = { 64,0,128,64 };

////  �O���[�o���ϐ��錾

LPD3DXSPRITE	lpSprite;	// �X�v���C�g
LPD3DXFONT		lpFont;		// �t�H���g

bool gameFullScreen;	// �t���X�N���[���itrue,false)

//�x�[�X�e�N�X�`��.
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
	float		  mGravity;
};

tPlayerObject player;
tChipObject mapChipList[mapChipRow][mapChipColumn];
//std::vector<std::vector<tSpriteObject*>> mapChipList;

//�o�͕���.
std::string outputString;

POINT point;

#define	FVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

const bool isHitRect(const RECT & aRect, const RECT & bRect)
{
	const auto result = aRect.left < bRect.right && bRect.left < aRect.right && aRect.top < bRect.bottom && bRect.top < aRect.bottom;
	return result;
}

//�s��Ɋm�肳����X�V����.
void LateUpdate()
{
	// �s��쐬
	D3DXMatrixTranslation(&player.mTransMatrix, player.mSpriteObject.mPos.x, player.mSpriteObject.mPos.y, player.mSpriteObject.mPos.z);
	D3DXMatrixScaling(&player.mScaleMatrix, player.mScaleVector.x, player.mScaleVector.y, player.mScaleVector.z);
	player.mSpriteObject.mMatrix = player.mScaleMatrix * player.mTransMatrix;
}

// �X�V����
void Update(void)
{

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		player.mSpriteObject.mPos.x += 3.0f;
		player.mScaleVector.x = 1.0f;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		player.mSpriteObject.mPos.x -= 3.0f;
		player.mScaleVector.x = -1.0f;
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		//player.mSpriteObject.mPos.y -= 3.0f;
		player.mGravity = -15.f;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		player.mSpriteObject.mPos.y += 3.0f;
	}

	player.mGravity += 0.9f;

	auto nextY = player.mSpriteObject.mPos.y + player.mGravity;
	const auto & playerPos = player.mSpriteObject.mPos;
	for (int row = 0; row < mapChipRow; row++)
	{
		for (int column = 0; column < mapChipColumn; column++)
		{
			if (mapChipList[row][column].mType != eChipType::eWall) { continue; }
			auto & chipPos = mapChipList[row][column].mSpriteObject.mPos;
			//�L������X���W���}�b�v�`�b�v��X���W�̒��ɓ����Ă��邩�`�F�b�N.
			if (chipPos.x < playerPos.x + (mapChipWidth / 2) && chipPos.x + mapChipWidth > playerPos.x - (mapChipWidth / 2))
			{
				/*
				������ӈ�̂̔��肪�C������������
				http://monooki.ldblog.jp/archives/35959913.html
				*/
				//�㑤����.
				if (nextY + (mapChipWidth / 2) > chipPos.y && playerPos.y < chipPos.y )
				{
					player.mGravity = 0; //( nextY + (mapChipWidth / 2) ) - chipPos.y;
					player.mSpriteObject.mPos.y = chipPos.y - (mapChipWidth / 2);
				}
				else if (nextY - (mapChipWidth / 2) < chipPos.y + mapChipHeight && playerPos.y > chipPos.y + mapChipHeight)
				{
					
					player.mGravity = 0; //( nextY + (mapChipWidth / 2) ) - chipPos.y;
					player.mSpriteObject.mPos.y = chipPos.y + mapChipHeight + (mapChipWidth / 2);
					
				}
			}
		}
	}

	player.mSpriteObject.mPos.y += player.mGravity;

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
	
	// �`��J�n
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


	// �`��I��
	lpSprite->End();
	

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

	/*
	std::string cordStr("#include<stdio.h>\nmain()\n{\n\tprintf(""helloworld!"")\n}");
	RECT rcCord = { 10,30,200,200 };
	lpFont->DrawText(nullptr,                          //Sprite�̃|�C���^ NULL�ł�OK
		cordStr.c_str(),										//�`�悷�镶����̃|�C���^���w�肵�܂��B
		-1,											//�����������܂��B-1�̏ꍇ�͑S�Ă�`�悵�܂��B
		&rcCord,								    //�`��̈�
		DT_LEFT | DT_NOCLIP,					    //�����ō������Ȃǂ�ݒ肵�܂��B
		D3DCOLOR_ARGB(255, 255, 0, 0));			//�����̐F�����߂܂��B
		*/

}

void DrawFrame(void)
{
	// �o�b�N�o�b�t�@�� Z �o�b�t�@���N���A
	lpD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,255),1.0f,0);


	// �X�V����
	Update();
	LateUpdate();

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
	D3DXMatrixPerspectiveFovLH(&mProj, D3DXToRadian(60), (float)scrollWidth/(float)scrollHeight, 1.0f, 1000.0f);

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
		scrollWidth, scrollHeight,
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
		RECT rc={0,0,scrollWidth,scrollHeight };
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
	d3dpp.BackBufferWidth = scrollWidth;
	d3dpp.BackBufferHeight = scrollHeight;
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
	//�e�N�X�`���ǂݍ���.
	LoadText(&baseChipTex, "Resorces/MapChip/chip01.png", 320, 320, NULL);	// �}�b�v�`�b�v.
	LoadText(&baseCharaTex, "Resorces/Chara/chara.png", 64, 64, NULL);		// ���@.

	FILE * fp;
	int column = 0, row = 0;
	if ( (fp = fopen("test.txt", "r") ) != NULL)
	{
		char ch;
		while((ch = fgetc(fp)) != EOF)
		{
			if( ch == '\n' )
			{
				row++;
				column = 0;
			}
			else
			{
				mapChipList[row][column].mSpriteObject.mpTex = baseChipTex;
				mapChipList[row][column].mSpriteObject.mPos = D3DXVECTOR3( column * mapChipWidth , row * mapChipHeight , 0 );
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
	fclose(fp);

	// ���@.
	//player = new tSpriteObject();
	player.mSpriteObject.mRect = { 0 , 0 , 64 , 64 };
	player.mSpriteObject.mpTex = baseCharaTex;
	player.mScaleVector = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	player.mSpriteObject.mPos = D3DXVECTOR3(100.0f,100.0f,0.0f);

	srand(time(0));

	// �X�v���C�g�쐬
	D3DXCreateSprite(lpD3DDevice,&lpSprite);
	lpSprite->OnResetDevice();

	// �t�H���g�쐬
	D3DXCreateFont(lpD3DDevice, 20, 20, FW_REGULAR, NULL, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "�l�r �S�V�b�N", &lpFont);

	lpFont->OnResetDevice();

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
	baseCharaTex->Release();
	baseChipTex->Release();

	lpSprite->Release();	// �X�v���C�g
	lpFont->Release();		// �t�H���g


	timeEndPeriod(1);

	// Direct3D �I�u�W�F�N�g�����
	lpD3DDevice->Release();
	lpD3D->Release();

	return (int)msg.wParam;
}