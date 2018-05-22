#include<d3dx9.h>
#include "DebugRectLine.h"


#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

DebugRectLine::DebugRectLine()
	: mpLine( nullptr )
	, mRectList( )
{
}


DebugRectLine::~DebugRectLine()
{
}

bool DebugRectLine::Initialize( LPDIRECT3DDEVICE9 lpD3DDevice )
{
	D3DXCreateLine( lpD3DDevice , &mpLine );
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
	for ( const auto & rect : mRectList)
	{
		auto lineDraw = [ this ](const float beginX , const float beginY , const float endX , const float endY )
		{
			D3DXVECTOR2 vec[] = {
				D3DXVECTOR2(beginX,beginY),
				D3DXVECTOR2(endX,endY),
			};
			mpLine->Draw(vec, 2, D3DCOLOR_ARGB(255, 255, 0, 255));
		};
		lineDraw(rect.left , rect.top   , rect.right, rect.top);
		lineDraw(rect.right, rect.top   , rect.right, rect.bottom);
		lineDraw(rect.right, rect.bottom, rect.left , rect.bottom);
		lineDraw(rect.left , rect.bottom, rect.left , rect.top);
	}
	mpLine->End();
}

void DebugRectLine::AddRect(const RECT aRect)
{
	mRectList.push_back(aRect);
}
