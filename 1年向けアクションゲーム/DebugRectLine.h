#pragma once

#include<vector>

typedef struct ID3DXLine ID3DXLine;
typedef struct ID3DXLine *LPD3DXLINE;
struct IDirect3DDevice9;
typedef struct IDirect3DDevice9 *LPDIRECT3DDEVICE9;

class DebugRectLine
{
public:
	DebugRectLine();
	~DebugRectLine();

	bool Initialize( LPDIRECT3DDEVICE9 lpD3DDevice );
	void Update();
	void Draw();
	void SetRect( const RECT & aRect );
	const RECT & GetRect();
private:
	LPD3DXLINE			mpLine;
	RECT				mRect;
};

