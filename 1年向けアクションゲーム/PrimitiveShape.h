#pragma once
#include<d3dx9.h>
#include<vector>
#include<functional>

namespace Color
{
	static const DWORD White		= D3DCOLOR_ARGB(255, 255, 255, 255);
	static const DWORD Black		= D3DCOLOR_ARGB(255, 0, 0, 0);
	static const DWORD Red			= D3DCOLOR_ARGB(255, 255, 0, 0);
	static const DWORD Green		= D3DCOLOR_ARGB(255, 0, 255, 0);
	static const DWORD Blue			= D3DCOLOR_ARGB(255, 0, 0, 255);
	static const DWORD Perple		= D3DCOLOR_ARGB(255,127,17,132);
}


namespace Primitive
{
	const int TriangleNum = 1;
	
	struct tVertex 
	{
		D3DXVECTOR3 mPos;
		D3DCOLOR	mColor;
	};

	struct Vertex2D//struct Vertex2D型 
	{
		//          ↓常に0
		float x, y, z, rhw;//rhw常に１
		D3DCOLOR color;
	};

	namespace Draw
	{
		namespace D2
		{
			void Line(const LPDIRECT3DDEVICE9 apD3dDevice , const D3DXVECTOR2 & aBegin , const D3DXVECTOR2 & aEnd , const D3DCOLOR & aColor = D3DCOLOR_ARGB(255 ,0, 0, 0))
			{
				struct Vertex2D vertex[] = {
					{ aBegin.x,aBegin.y,0.0f,1.0f,aColor },
					{ aEnd.x,aEnd.y,0.0f,1.0f,aColor }
				};
				apD3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

				apD3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, vertex, sizeof(Vertex2D));

				apD3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
			}

			void Box(const LPDIRECT3DDEVICE9 apD3dDevice, const D3DXVECTOR2 & aPos, const D3DXVECTOR2 & aLength, const bool aPaintFlg, const D3DCOLOR & aColor)
			{
				apD3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

				if (aPaintFlg == true) {
					struct Vertex2D Vertex[] = {
						{ aPos.x,aPos.y,0.0f,1.0f,aColor },
					{ aPos.x,aLength.y,0.0f,1.0f,aColor },
					{ aLength.x,aPos.y,0.0f,1.0f,aColor },
					{ aLength.x,aLength.y,0.0f,1.0f,aColor },
					};

					apD3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertex, sizeof(Vertex2D));

				}
				else {
					struct Vertex2D Vertex[] = {
						{ aPos.x,aPos.y,0.0f,1.0f,aColor },
					{ aPos.x,aLength.y,0.0f,1.0f,aColor },
					{ aLength.x,aLength.y,0.0f,1.0f,aColor },
					{ aLength.x,aPos.y,0.0f,1.0f,aColor },
					{ aPos.x,aPos.y,0.0f,1.0f,aColor }
					};
					apD3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, Vertex, sizeof(Vertex2D));
				}

				apD3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
			}
		}
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

		//未完成（角度をつけるバグる）
		void Rect(const LPDIRECT3DDEVICE9 apD3dDevice, const float aX , const float aY , const float aWidth , const float aHeight , const float aDegree, const D3DCOLOR & aColor)
		{
			apD3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
			const float x = aX - aWidth / 2;
			const float y = aY - aHeight / 2;
			tVertex vertex[] = {
				{ D3DXVECTOR3(x,y,0.0f),aColor },
				{ D3DXVECTOR3(x,(y+aHeight),0.0f),aColor },
				{ D3DXVECTOR3((x+aWidth),y,0.0f),aColor },
				{ D3DXVECTOR3((x+aWidth),(y+aHeight),0.0f),aColor },
			};
			D3DXMATRIX mRot;
			D3DXMatrixRotationZ(&mRot, aDegree);
			apD3dDevice->SetTransform(D3DTS_WORLD,&mRot );
			apD3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP
				, 2
				, vertex
				, sizeof(tVertex));
			//使った後は元通りに.
			apD3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
			D3DXMatrixIdentity(&mRot);
			apD3dDevice->SetTransform(D3DTS_WORLD, &mRot);
		}

		void Line(const LPDIRECT3DDEVICE9 apD3dDevice, const D3DXVECTOR3 & aBeginPos, const D3DXVECTOR3 & aEndPos)
		{
			apD3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
			tVertex vertex[] = {
				{ aBeginPos,Color::Black },
				{ aEndPos,Color::Black },
			};
			apD3dDevice->DrawPrimitiveUP(D3DPT_LINELIST
				, 1
				, vertex
				, sizeof(tVertex));
			//使った後は元通りに.
			apD3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		}

		const std::vector<D3DXVECTOR3> & Polygon(const LPDIRECT3DDEVICE9 apD3dDevice, const int aPolygonNum, const D3DXVECTOR3 & aPosition, const float aLength, const int aDegree = 0 , bool aWireFrame = false, const D3DCOLOR & aColor = Color::Perple)
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