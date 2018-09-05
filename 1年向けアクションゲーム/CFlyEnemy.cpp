#include "CFlyEnemy.h"
#include<cassert>
#include<cmath>


CFlyEnemy::CFlyEnemy()
	: mpTexture(nullptr)
	, mPosition(0.0f, 0.0f)
	, mStartPosition(0.0f, 0.0f)
	, mMoveVector(0.0f, 0.0f)
	, mCenterPoint(0.0f, 0.0f, 0.0f)
	, mMatrix()
	, mRect({ 0,0,64,64 })
	, mColor(D3DCOLOR_ARGB(255, 255, 255, 255))
	, mDegree( 0.0f )
{
}


CFlyEnemy::~CFlyEnemy()
{
}

CFlyEnemy & CFlyEnemy::Create()
{
	//std::nothrowで例外を発生させないようにする.
	//メモリ確保に失敗した場合はnullptrを返す.
	auto * result = new(std::nothrow)CFlyEnemy;
	assert(result != nullptr, "not create CFlyEnemy");
	return *result;
}

void CFlyEnemy::Update()
{
	mDegree += 1.00f;
	if (mDegree > 360.0f)
	{
		mDegree = 0.0f;
		printf("\nreset degree\n");
	}
	float sinTest = std::sin(D3DXToRadian(mDegree));
	//printf("sinTest = %.3f\ntest = %.3f\n", sinTest,mDegree);
	mPosition.y = mStartPosition.y + ( sinTest * 50.0f );
}

void CFlyEnemy::LateUpdate()
{
	D3DXMatrixTranslation(&mMatrix, mPosition.x, mPosition.y, 0.0f);
}

void CFlyEnemy::Draw(LPD3DXSPRITE apSprite)
{
	apSprite->SetTransform(&mMatrix);
	apSprite->Draw(mpTexture, &mRect, &mCenterPoint, nullptr, mColor);
}

CFlyEnemy & CFlyEnemy::Texture(const LPDIRECT3DTEXTURE9 apTexture)
{
	mpTexture = apTexture;
	return *this;
}

CFlyEnemy & CFlyEnemy::StartPosition(const D3DXVECTOR2 & aPosition)
{
	mPosition = aPosition;
	return *this;
}

void CFlyEnemy::AddPosition(const D3DXVECTOR2 & aPosition)
{
	mPosition += aPosition;
}

const D3DXVECTOR2 & CFlyEnemy::GetPosition() const
{
	return mPosition;
}

CFlyEnemy & CFlyEnemy::MoveVector(const D3DXVECTOR2 & aMoveVector)
{
	mMoveVector = aMoveVector;
	return *this;
}
CFlyEnemy & CFlyEnemy::MovePoint(const float aDistance)
{
	return *this;
}
