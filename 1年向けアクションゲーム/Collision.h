#include<d3dx9.h>

enum eHitDirection
{
	eUp,
	eDown,
	eLeft,
	eRight,
	eNone,
};

static const eHitDirection RectCollision( const D3DXVECTOR2 & aPos , const D3DXVECTOR2 & aMoveVector , const D3DXVECTOR3 & aCenterPos
                                        , const D3DXVECTOR2 & aTargetPos , const D3DXVECTOR3 & aTargetCenterPos )
{
	//TODO:Ç‰Ç≠Ç‰Ç≠ÇÕäOïîÇ…ì¶Ç™Ç∑.
	const int length = 64;

	const auto nextPos = aPos + aMoveVector;
	const RECT rect = { aPos.x - aCenterPos.x , aPos.y - aCenterPos.y , aPos.x + ( length - aCenterPos.x ) , aPos.y + (length - aCenterPos.y) };
	const RECT targetRect = { aTargetPos.x - aTargetCenterPos.x , aTargetPos.y - aTargetCenterPos.y , aTargetPos.x + (length - aTargetCenterPos.x) , aTargetPos.y + (length - aTargetCenterPos.y) };
	const RECT nextRect = { nextPos.x - aCenterPos.x , nextPos.y - aCenterPos.y , nextPos.x + (length - aCenterPos.x) , nextPos.y + (length - aCenterPos.y) };

	//è„â∫îªíË.
	if (targetRect.left < rect.right
		&& targetRect.right > rect.left)
	{
		//è„îªíË.
		if (targetRect.top < nextRect.bottom
			&& targetRect.top > nextRect.top)
		{
			return eHitDirection::eUp;
		}
		//â∫îªíË.
		else if (targetRect.bottom < nextRect.bottom
			&& targetRect.bottom > nextRect.top)
		{
			return eHitDirection::eDown;
		}
	}
	//ç∂âEîªíË.
	if (targetRect.top < rect.bottom
		&& targetRect.bottom > rect.top)
	{
		//ç∂îªíË.
		if (targetRect.left < nextRect.right
			&& targetRect.left > nextRect.left)
		{
			return eHitDirection::eLeft;
		}
		//âEîªíË.
		else if (targetRect.right < nextRect.right
			&& targetRect.right > nextRect.left)
		{
			return eHitDirection::eRight;
		}
	}

	return eHitDirection::eNone;
}