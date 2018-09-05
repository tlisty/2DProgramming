#pragma once
#include<d3dx9.h>

struct tSpriteObject
{
	LPDIRECT3DTEXTURE9	mpTex;
	D3DXVECTOR3			mPos;
	D3DXMATRIX			mMatrix;
	RECT				mRect;
	bool				mFlg;
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