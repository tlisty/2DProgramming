#pragma once

#include<d3dx9.h>
#include<string>
#include<vector>

enum eChipType;
struct tChipObject;

struct tMovePoint
{
	tMovePoint(const float aStart,const float aEnd);
	tMovePoint(){};
	void SetPoint(const float aStart, const float aEnd);
	void SetPoint(const tMovePoint & aMovePoint);
	float mStart;
	float mEnd;
};

class CEnemy
{
public:


	enum eMoveState
	{
		eNone,
		eRight,
		eLeft,
	};
	CEnemy();
	~CEnemy();

	static CEnemy * Create();

	void Update(const std::vector<std::vector<tChipObject*>> & aChipObjects);
	void LateUpdate();
	void Draw(LPD3DXSPRITE apSprite);
	void SetTexture(const LPDIRECT3DTEXTURE9 apTexture);
	void SetPosition(const D3DXVECTOR2 & aPosition);
	void AddPosition(const D3DXVECTOR2 & aPosition);
	const D3DXVECTOR2 & GetPosition() const;
	void SetMoveVector(const D3DXVECTOR2 & aMoveVector);
	void SetMovePoint( const tMovePoint & aMovePoint );
	void SetMovePoint(const float aDistance);
	void SetMoveState(const eMoveState aMoveState);
	const D3DXVECTOR3 & GetCenterPoint() const;

private:
	
	void RightMove();
	void LeftMove();

	LPDIRECT3DTEXTURE9	mpTexture;
	D3DXVECTOR2			mPosition;
	D3DXVECTOR2			mMoveVector;
	D3DXVECTOR3			mCenterPoint;
	D3DXMATRIX			mMatrix;
	RECT				mRect;
	D3DCOLOR            mColor;
	tMovePoint			mMovePoint;
	eMoveState			mMoveState;
};

