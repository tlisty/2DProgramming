#include<d3dx9.h>
#include "DebugRectLine.h"


#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

DebugRectLine::DebugRectLine()
	: mpLine(nullptr)
	, mRect()
{
}


DebugRectLine::~DebugRectLine()
{
}

bool DebugRectLine::Initialize(LPDIRECT3DDEVICE9 lpD3DDevice)
{
	D3DXCreateLine(lpD3DDevice, &mpLine);
	const bool result = mpLine != nullptr;
	return result;
}

void DebugRectLine::Update()
{

}

void DebugRectLine::Draw()
{
	mpLine->SetAntialias(TRUE);
	mpLine->SetWidth(1);
	mpLine->Begin();
	auto lineDraw = [this](const float beginX, const float beginY, const float endX, const float endY)
	{
		D3DXVECTOR2 vec[] = {
			D3DXVECTOR2(beginX,beginY),
			D3DXVECTOR2(endX,endY),
		};
		mpLine->Draw(vec, 2, D3DCOLOR_ARGB(255, 255, 0, 255));
	};
	lineDraw(mRect.left, mRect.top, mRect.right, mRect.top);
	lineDraw(mRect.right, mRect.top, mRect.right, mRect.bottom);
	lineDraw(mRect.right, mRect.bottom, mRect.left, mRect.bottom);
	lineDraw(mRect.left, mRect.bottom, mRect.left, mRect.top);
	mpLine->End();
}

void DebugRectLine::SetRect(const RECT & aRect)
{
	mRect = aRect;
}

const RECT & DebugRectLine::GetRect()
{
	return mRect;
}
