#pragma once

#include<d3dx9.h>
#include<utility>

enum eHitDirection
{
	eUp,
	eDown,
	eLeft,
	eRight,
	eNone,
};

//線分構造体.
struct tSegment
{
	tSegment( const D3DXVECTOR2 & aPosition , const D3DXVECTOR2 & aVector )
	{
		mPosition = aPosition;
		mVector   = aVector;
	}
	D3DXVECTOR2 mPosition;
	D3DXVECTOR2 mVector;
};



class Collision
{
public:
	using SegmentsResult = std::pair<bool, D3DXVECTOR2>;
// 線分の衝突
	static const SegmentsResult Segments(
		tSegment &seg1,          // 線分1
		tSegment &seg2           // 線分2
	);

	// 2Dベクトルの外積
	static const float D3DXVec2Cross(const D3DXVECTOR2 & v1, const D3DXVECTOR2 & v2);

	static const eHitDirection Rect(const D3DXVECTOR2 & aPos, const D3DXVECTOR2 & aMoveVector, const D3DXVECTOR3 & aCenterPos
		, const D3DXVECTOR2 & aTargetPos, const D3DXVECTOR3 & aTargetCenterPos);

}; 

