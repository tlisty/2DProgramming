#include "CEnemy.h"
#include<cassert>

tMovePoint::tMovePoint(const float aStart, const float aEnd)
	: mStart(0.0f)
	, mEnd(0.0f)
{
	SetPoint(aStart, aEnd);
}
void tMovePoint::SetPoint(const float aStart, const float aEnd)
{
	mStart	= aStart;
	mEnd	= aEnd;
}

void tMovePoint::SetPoint(const tMovePoint & aMovePoint)
{
	SetPoint(aMovePoint.mStart, aMovePoint.mEnd);
}


CEnemy::CEnemy()
	: mpTexture(nullptr)
	, mPosition(0.0f,0.0f)
	, mMoveVector(0.0f,0.0f)
	, mCenterPoint(0.0f,0.0f,0.0f)
	, mMatrix()
	, mRect({0,0,64,64})
	, mColor(D3DCOLOR_ARGB(255,255,255,255))
	, mMovePoint()
	, mMoveState( eNone )
{
}


CEnemy::~CEnemy()
{
}

CEnemy * CEnemy::Create()
{
	//std::nothrow‚Å—áŠO‚ð”­¶‚³‚¹‚È‚¢‚æ‚¤‚É‚·‚é.
	//ƒƒ‚ƒŠŠm•Û‚ÉŽ¸”s‚µ‚½ê‡‚Ínullptr‚ð•Ô‚·.
	auto * result = new(std::nothrow)CEnemy;
	assert(result != nullptr, "not create CEnemy");
	return result;
}

void CEnemy::Update()
{
	switch (mMoveState)
	{
	case eRight:
		RightMove();
		break;
	case eLeft:
		LeftMove();
		break;
	case eNone:
	default:
		break;
	}
}

void CEnemy::LateUpdate()
{
	D3DXMatrixTranslation(&mMatrix, mPosition.x, mPosition.y, 0.0f);
}

void CEnemy::Draw(LPD3DXSPRITE apSprite)
{
	apSprite->SetTransform(&mMatrix);
	apSprite->Draw(mpTexture, &mRect, &mCenterPoint, nullptr, mColor);
}

void CEnemy::SetTexture(const LPDIRECT3DTEXTURE9 apTexture)
{
	mpTexture = apTexture;
}

void CEnemy::SetPosition(const D3DXVECTOR2 & aPosition)
{
	mPosition = aPosition;
}

void CEnemy::AddPosition(const D3DXVECTOR2 & aPosition)
{
	mPosition += aPosition;
}

const D3DXVECTOR2 & CEnemy::GetPosition() const
{
	return mPosition;
}

void CEnemy::SetMoveVector(const D3DXVECTOR2 & aMoveVector)
{
	mMoveVector = aMoveVector;
}

void CEnemy::SetMovePoint(const tMovePoint & aMovePoint)
{
	mMovePoint.SetPoint(aMovePoint);
}

void CEnemy::SetMovePoint(const float aDistance)
{
	tMovePoint movePoint( mPosition.x - aDistance , mPosition.x + aDistance );
	mMovePoint.SetPoint(movePoint);
}

void CEnemy::SetMoveState(const eMoveState aMoveState)
{
	mMoveState = aMoveState;
}

void CEnemy::RightMove()
{
	mPosition.x += mMoveVector.x;
	const bool isEnd = (mPosition.x > mMovePoint.mEnd);
	if (isEnd)
	{
		mPosition.x = mMovePoint.mEnd;
		mMoveState = eLeft;
	}
}

void CEnemy::LeftMove()
{
	mPosition.x -= mMoveVector.x;
	const bool isEnd = (mPosition.x < mMovePoint.mStart);
	if (isEnd)
	{
		mPosition.x = mMovePoint.mStart;
		mMoveState = eRight;
	}
}