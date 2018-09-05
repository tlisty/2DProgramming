#include "CEnemy.h"
#include "Collision.h"
#include<cassert>
#include"CSpriteObject.h"

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
	, mCenterPoint(32.0f,32.0f,0.0f)
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
	//std::nothrowで例外を発生させないようにする.
	//メモリ確保に失敗した場合はnullptrを返す.
	auto * result = new(std::nothrow)CEnemy;
	assert(result != nullptr, "not create CEnemy");
	return result;
}

void CEnemy::Update(const std::vector<std::vector<tChipObject*>> & aChipObjects)
{

	mMoveVector.x = 0.0f;
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

	mMoveVector.y += 1.0f;

	const auto & playerPos = mPosition;
	//マップチップ配置の中から自分がどこにいるか判定する.
	const auto startRow = playerPos.y / 64;
	const auto startColumn = playerPos.x / 64;

	for (int row = startRow - 1; row < startRow + 1; row++)
	{
		for (int column = startColumn - 1; column < startColumn + 1; column++)
		{
			if (aChipObjects[row][column]->mType != eChipType::eWall) 
			{ 
				continue; 
			}

			auto & chipPos = aChipObjects[row][column]->mSpriteObject.mPos;
			const auto directon = Collision::Rect(mPosition,mMoveVector,mCenterPoint,D3DXVECTOR2(chipPos.x, chipPos.y),D3DXVECTOR3(0.0f,0.0f,0.0f));
			switch (directon)
			{
			case eHitDirection::eUp:
				mMoveVector.y = 0;
				mPosition.y = chipPos.y - 32;
				break;
			case eHitDirection::eDown:
				mMoveVector.y = 0;
				mPosition.y = chipPos.y + 32 + 64;
				break;
			case eHitDirection::eLeft:
				mMoveVector.x = 0;
				mPosition.x = chipPos.x - 32;
				mMoveState = eMoveState::eLeft;
				break;
			case eHitDirection::eRight:
				mMoveVector.x = 0;
				mPosition.x = chipPos.x + 32 + 64;
				mMoveState = eMoveState::eRight;
				break;
			case eHitDirection::eNone:
			default:
				break;
			}
		}
	}
	
}

void CEnemy::LateUpdate()
{
	mPosition += mMoveVector;
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

const D3DXVECTOR3 & CEnemy::GetCenterPoint() const
{
	return mCenterPoint;
}

void CEnemy::RightMove()
{
	mMoveVector.x += 3.0f;
}

void CEnemy::LeftMove()
{
	mMoveVector.x -= 3.0f;
}