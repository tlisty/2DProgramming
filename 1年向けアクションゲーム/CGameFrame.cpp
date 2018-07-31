#include<algorithm>
#include<vector>
#include<random>

#include "CGameFrame.h"
#include "CEnemy.h"
#include "CFlyEnemy.h"
#include "Collision.h"
#include "CSpriteObject.h"
#include "PrimitiveShape.h"


// ���_�t�H�[�}�b�g�̒�`
struct VERTEX
{
	D3DXVECTOR3 Pos;
	D3DCOLOR Color;
	D3DXVECTOR2 Tex;
};

//�萔.
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

////  �O���[�o���ϐ��錾

LPD3DXSPRITE	lpSprite;	// �X�v���C�g
LPD3DXFONT		lpFont;		// �t�H���g

							//�x�[�X�e�N�X�`��.
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
		_ASSERT_EXPR(false, L"�t�@�C���ǂݍ��ݎ��s");
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

	LoadText(&baseChipTex, "Resorces/MapChip/chip01.png", 320, 320, NULL);	// �}�b�v�`�b�v.
	LoadText(&baseCharaTex, "Resorces/Chara/chara.png", 64, 64, NULL);		// ���@.

	LoadMapChipFile();

	srand(timeGetTime());

	// ���@.
	//player = new tSpriteObject();
	player.mSpriteObject.mRect = { 0 , 0 , 64 , 64 };
	player.mSpriteObject.mpTex = baseCharaTex;
	player.mScaleVector = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	player.mSpriteObject.mPos = D3DXVECTOR3(150.0f, 150.0f, 0.0f);
	player.mSpriteObject.mFlg = true;

	// �X�v���C�g�쐬
	D3DXCreateSprite(mpD3DDevice,&lpSprite);
	lpSprite->OnResetDevice();

	LPDIRECT3DTEXTURE9 enemyTexture;
	LoadText(&enemyTexture, "Resorces/kuroTenemy.png", 64, 64, D3DCOLOR_ARGB(255, 255, 255, 255));

	//�G����.
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
		//�G�Ǝ��@�̓����蔻��.
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

	//�}�b�v�`�b�v�z�u�̒����玩�����ǂ��ɂ��邩���肷��.
	const auto startRow = playerPos.y / mapChipHeight;
	const auto startColumn = playerPos.x / mapChipWidth;
	for (int row = startRow - hitRange; row < startRow + hitRange; row++)
	{
		for (int column = startColumn - hitRange; column < startColumn + hitRange; column++)
		{
			if (mapChipList[row][column]->mType != eChipType::eWall) {continue; }

			auto & chipPos = mapChipList[row][column]->mSpriteObject.mPos;

			//�L������X���W���}�b�v�`�b�v��X���W�̒��ɓ����Ă��邩�`�F�b�N�ʂ�����㉺����ɓ���.
			if (chipPos.x < playerPos.x + (mapChipWidth / 2)
				&& chipPos.x + mapChipWidth > playerPos.x - (mapChipWidth / 2))
			{
				//�㔻��.
				//���ӂ����̔��肾�ƃ^�[�Q�b�g�̉��ɂ���Ƃ������肵�Ă��܂��̂ŏ�ӂ��^�[�Q�b�g�̏�ӂ���ɂ��邩���m�F����.
				if (nextY + (mapChipHeight / 2) > chipPos.y
					&& nextY - (mapChipHeight / 2)< chipPos.y)
				{
					player.mAcceleVector.y = 0;
					player.mSpriteObject.mPos.y = chipPos.y - (mapChipHeight / 2);
					player.mJumpFlg = true;
				}
				//������.
				//��ӂ����̔��肾�ƃ^�[�Q�b�g�̏�ɂ���Ƃ������肵�Ă��܂��̂ŉ��ӂ��^�[�Q�b�g�̉��ӂ��ア�邩���m�F����.
				else if (nextY - (mapChipHeight / 2) < chipPos.y + mapChipHeight
					&& nextY + (mapChipHeight / 2)> chipPos.y + mapChipHeight)
				{

					player.mAcceleVector.y = 0;
					player.mSpriteObject.mPos.y = chipPos.y + mapChipHeight + (mapChipHeight / 2);
				}
			}

			//�L������Y���W���}�b�v�`�b�v��Y���W�̒��ɓ����Ă��邩�`�F�b�N�ʂ����獶�E����ɓ���.
			if (chipPos.y < playerPos.y + (mapChipHeight / 2)
				&& chipPos.y + mapChipHeight > playerPos.y - (mapChipHeight / 2))
			{
				//�E����.
				if (nextX + (mapChipWidth / 2) > chipPos.x
					&& nextX - (mapChipWidth / 2) < chipPos.x)
				{
					player.mAcceleVector.x = 0;
					player.mSpriteObject.mPos.x = chipPos.x - (mapChipWidth / 2);
				}
				//������.
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
	// �o�b�N�o�b�t�@�� Z �o�b�t�@���N���A
	mpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);


	Update();
	LateUpdate();

	// �`��J�n
	mpD3DDevice->BeginScene();

	D3DXMATRIX mView, mProj;

	// ���_�s��̐ݒ�
	D3DXMatrixLookAtLH(&mView,
		&D3DXVECTOR3(0.0f, 0.0f, -10.0f),	// �J�����̈ʒu
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// �J�����̎��_
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f)	// �J�����̓��̕���
	);

	// ���e�s��̐ݒ�
	D3DXMatrixPerspectiveFovLH(&mProj, D3DXToRadian(60), (float)mScrollWidth / (float)mScrollHeight, 1.0f, 1000.0f);

	//�s��ݒ�
	mpD3DDevice->SetTransform(D3DTS_VIEW, &mView);
	mpD3DDevice->SetTransform(D3DTS_PROJECTION, &mProj);

	Draw();

	// �`��I��
	mpD3DDevice->EndScene();

	// �o�b�N�o�b�t�@���v���C�}���o�b�t�@�ɃR�s�[
	mpD3DDevice->Present(NULL, NULL, NULL, NULL);
}

void CGameFrame::Draw2D()
{
	// �`��J�n
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

	// �`��I��
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
	// Direct3D �I�u�W�F�N�g���쐬
	mpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if( mpD3D == nullptr )
	{
		// �I�u�W�F�N�g�쐬���s
		MessageBox(NULL, "Direct3D �̍쐬�Ɏ��s���܂����B", "ERROR", MB_OK | MB_ICONSTOP);
		// �I������
		return false;
	}
	int adapter;

	// �g�p����A�_�v�^�ԍ�
	adapter = D3DADAPTER_DEFAULT;

	// �E�C���h�E�̍쐬�����������̂ŁADirect3D ������������
	ZeroMemory(&mD3Dpp, sizeof(D3DPRESENT_PARAMETERS));
	// Direct3D �������p�����[�^�̐ݒ�
	if (mIsFullScreen)
	{
		// �t���X�N���[�� : �قƂ�ǂ̃A�_�v�^�ŃT�|�[�g����Ă���t�H�[�}�b�g���g�p
		mD3Dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	}
	else
	{
		// �E�C���h�E : ���݂̉�ʃ��[�h���g�p
		D3DDISPLAYMODE disp;
		// ���݂̉�ʃ��[�h���擾
		mpD3D->GetAdapterDisplayMode(adapter, &disp);
		mD3Dpp.BackBufferFormat = disp.Format;
	}
	// �\���̈�T�C�Y�̐ݒ�
	mD3Dpp.BackBufferWidth = mScrollWidth;
	mD3Dpp.BackBufferHeight = mScrollHeight;
	mD3Dpp.SwapEffect = D3DSWAPEFFECT_FLIP;

	if (mIsFullScreen == false)
	{
		// �E�C���h�E���[�h
		mD3Dpp.Windowed = 1;
	}

	// Z �o�b�t�@�̎����쐬
	mD3Dpp.EnableAutoDepthStencil = 1;
	mD3Dpp.AutoDepthStencilFormat = D3DFMT_D16;

	//�ޯ��ޯ̧��ۯ��\�ɂ���(GetDC���\�ɂȂ�)
	mD3Dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	// �f�o�C�X�̍쐬 - T&L HAL
	if (FAILED(mpD3D->CreateDevice(adapter, D3DDEVTYPE_HAL, mpHwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &mD3Dpp, &mpD3DDevice)))
	{
		// ���s�����̂� HAL �Ŏ��s
		if (FAILED(mpD3D->CreateDevice(adapter, D3DDEVTYPE_HAL, mpHwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &mD3Dpp, &mpD3DDevice)))
		{
			// ���s�����̂� REF �Ŏ��s
			if (FAILED(mpD3D->CreateDevice(adapter, D3DDEVTYPE_REF, mpHwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &mD3Dpp, &mpD3DDevice)))
			{
				// ���ǎ��s����
				MessageBox(NULL, "DirectX9���������ł��܂���B\n���Ή��̃p�\�R���Ǝv���܂��B", "ERROR", MB_OK | MB_ICONSTOP);
				mpD3D->Release();
				// �I������
				return 0;
			}
		}
	}

	// �����_�����O�E�X�e�[�g��ݒ�
	// Z �o�b�t�@�L����->�O��֌W�̌v�Z�𐳊m�ɂ��Ă����
	mpD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	mpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// �A���t�@�u�����f�B���O�L����
	mpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	// �A���t�@�u�����f�B���O���@��ݒ�
	mpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	mpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// �����_�����O���̃A���t�@�l�̌v�Z���@�̐ݒ�
	mpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	// �e�N�X�`���̐F���g�p
	mpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	// ���_�̐F���g�p
	mpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	// �����_�����O���̐F�̌v�Z���@�̐ݒ�
	mpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	//���ʃJ�����O
	mpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// �t�B���^�ݒ�
	mpD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	mpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	mpD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// ���C�g
	mpD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	mpD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	// ���_�t�H�[�}�b�g�̐ݒ�
	mpD3DDevice->SetFVF(mFvFVertex);
}
