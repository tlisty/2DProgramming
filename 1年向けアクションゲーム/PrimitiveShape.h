#pragma once
#include<d3dx9.h>
#include<vector>
#include<functional>

namespace Color
{
	DWORD White		= D3DCOLOR_ARGB(255, 255, 255, 255);
	DWORD Black		= D3DCOLOR_ARGB(255, 0, 0, 0);
	DWORD Red		= D3DCOLOR_ARGB(255, 0, 0, 0);
	DWORD Green		= D3DCOLOR_ARGB(255, 0, 0, 0);
	DWORD Blue		= D3DCOLOR_ARGB(255, 0, 0, 0);
	DWORD Perple	= D3DCOLOR_ARGB(255,127,17,132);
}

namespace Primitive
{
	const int TriangleNum = 1;
	
	struct tVertex 
	{
		D3DXVECTOR3 mPos;
		D3DCOLOR	mColor;
	};

	namespace Draw
	{
		void Triangle( const LPDIRECT3DDEVICE9 apD3dDevice ,const D3DXVECTOR3 & aPos1 , const D3DXVECTOR3 & aPos2 , const D3DXVECTOR3 & aPos3 , const D3DCOLOR & aColor)
		{
			apD3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
			tVertex vertex[] = { 
				{ aPos1,aColor},
				{ aPos2,aColor },
				{ aPos3,aColor },
			};
			apD3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST
				, TriangleNum
				, vertex
				, sizeof(tVertex));
			apD3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		}

		const std::vector<D3DXVECTOR3> & Polygon(const LPDIRECT3DDEVICE9 apD3dDevice, const int aPolygonNum, const D3DXVECTOR3 & aPosition, const int aLength, const int aDegree = 0 , bool aWireFrame = false, const D3DCOLOR & aColor = Color::Perple)
		{
			if (aWireFrame)
			{
				apD3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			}


			std::vector<D3DXVECTOR3> result;
			const int vertexNum = aPolygonNum * 3;
			std::vector<tVertex> v;
			v.resize(vertexNum);
			for (int polygonIndex = 0; polygonIndex < aPolygonNum; polygonIndex++)
			{
				const int vertexIndex = polygonIndex * 3;
				auto& vertex1 = v[vertexIndex];
				vertex1.mPos = aPosition;
				vertex1.mColor = aColor;

				auto& vetrex2 = v[vertexIndex + 1];
				float x = sin(D3DXToRadian(polygonIndex * (360 / aPolygonNum) + aDegree)) * aLength;
				float y = cos(D3DXToRadian(polygonIndex * (360 / aPolygonNum) + aDegree)) * aLength;
				result.push_back( D3DXVECTOR3(x,y,0.0f) );
				vetrex2.mPos = aPosition + D3DXVECTOR3(x, y, 0.0f);
				vetrex2.mColor = aColor;

				auto& vetrex3 = v[vertexIndex + 2];
				x = sin(D3DXToRadian((polygonIndex + 1) * (360 / aPolygonNum) + aDegree)) * aLength;
				y = cos(D3DXToRadian((polygonIndex + 1) * (360 / aPolygonNum) + aDegree)) * aLength;
				vetrex3.mPos = aPosition + D3DXVECTOR3(x, y, 0.0f);
				vetrex3.mColor = aColor;
			}
			apD3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST
				, aPolygonNum
				, &v.at(0)
				, sizeof(tVertex));
			apD3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			return result;
		}
		
	}
}