#include"Collision.h"

// 2D�x�N�g���̊O��
const float Collision::D3DXVec2Cross(const D3DXVECTOR2 & v1, const D3DXVECTOR2 & v2) {
	return v1.x * v2.y - v1.y * v2.x;
}

// �����̏Փ�
 const Collision::SegmentsResult Collision::Segments(
	tSegment &seg1,          // ����1
	tSegment &seg2           // ����2
) {
	SegmentsResult result(false,D3DXVECTOR2(0.0f,0.0f));

	D3DXVECTOR2 v = seg2.mPosition - seg1.mPosition;
	float Crs_v1_v2 = D3DXVec2Cross(seg1.mVector, seg2.mVector);
	if (Crs_v1_v2 == 0.0f) {
		// ���s���
		return result;
	}

	float Crs_v_v1 = D3DXVec2Cross(v, seg1.mVector);
	float Crs_v_v2 = D3DXVec2Cross(v, seg2.mVector);

	float t1 = Crs_v_v2 / Crs_v1_v2;
	float t2 = Crs_v_v1 / Crs_v1_v2;

	/*
	if (outT1)
		*outT1 = Crs_v_v2 / Crs_v1_v2;
	if (outT2)
		*outT2 = Crs_v_v1 / Crs_v1_v2;
	*/

	const float eps = 0.00000f;
	if (t1 < 0 || t1 > 1 || t2 < 0 || t2 > 1) {
		// �������Ă��Ȃ�
		return result;
	}

	result.first  = true;
	result.second = seg1.mPosition + seg1.mVector * t1;

	return result;
}

const eHitDirection Collision::Rect( const D3DXVECTOR2 & aPos , const D3DXVECTOR2 & aMoveVector , const D3DXVECTOR3 & aCenterPos
                                        , const D3DXVECTOR2 & aTargetPos , const D3DXVECTOR3 & aTargetCenterPos )
{
	//TODO:�䂭�䂭�͊O���ɓ�����.
	const int length = 64;

	const auto nextPos = aPos + aMoveVector;
	const RECT rect = { aPos.x - aCenterPos.x , aPos.y - aCenterPos.y , aPos.x + ( length - aCenterPos.x ) , aPos.y + (length - aCenterPos.y) };
	const RECT targetRect = { aTargetPos.x - aTargetCenterPos.x , aTargetPos.y - aTargetCenterPos.y , aTargetPos.x + (length - aTargetCenterPos.x) , aTargetPos.y + (length - aTargetCenterPos.y) };
	const RECT nextRect = { nextPos.x - aCenterPos.x , nextPos.y - aCenterPos.y , nextPos.x + (length - aCenterPos.x) , nextPos.y + (length - aCenterPos.y) };

	//�㉺����.
	if (targetRect.left < rect.right
		&& targetRect.right > rect.left)
	{
		//�㔻��.
		if (targetRect.top < nextRect.bottom
			&& targetRect.top > nextRect.top)
		{
			return eHitDirection::eUp;
		}
		//������.
		else if (targetRect.bottom < nextRect.bottom
			&& targetRect.bottom > nextRect.top)
		{
			return eHitDirection::eDown;
		}
	}
	//���E����.
	if (targetRect.top < rect.bottom
		&& targetRect.bottom > rect.top)
	{
		//������.
		if (targetRect.left < nextRect.right
			&& targetRect.left > nextRect.left)
		{
			return eHitDirection::eLeft;
		}
		//�E����.
		else if (targetRect.right < nextRect.right
			&& targetRect.right > nextRect.left)
		{
			return eHitDirection::eRight;
		}
	}

	return eHitDirection::eNone;
}