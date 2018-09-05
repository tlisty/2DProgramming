#include<d3dx9.h>
#include<string>

class CFlyEnemy
{
public:

	CFlyEnemy();
	~CFlyEnemy();

	static CFlyEnemy & Create();

	void Update();
	void LateUpdate();
	void Draw(LPD3DXSPRITE apSprite);
	CFlyEnemy & Texture(const LPDIRECT3DTEXTURE9 apTexture);
	CFlyEnemy & StartPosition(const D3DXVECTOR2 & aPosition);
	CFlyEnemy & MovePoint(const float aDistance);
	CFlyEnemy & MoveVector(const D3DXVECTOR2 & aMoveVector);
	void AddPosition(const D3DXVECTOR2 & aPosition);
	const D3DXVECTOR2 & GetPosition() const;

private:

	LPDIRECT3DTEXTURE9	mpTexture;
	D3DXVECTOR2			mPosition;
	D3DXVECTOR2			mStartPosition;
	D3DXVECTOR2			mMoveVector;
	D3DXVECTOR3			mCenterPoint;
	D3DXMATRIX			mMatrix;
	RECT				mRect;
	D3DCOLOR            mColor;
	float				mDegree;
};

