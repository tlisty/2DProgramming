#pragma once
#include<string>

class GameBase
{
public:
	GameBase();
	~GameBase();
	
	bool Init();
	void Update();
	void Draw();

private:
	/*
	std::string mText;
	LPD3DXFONT	mpFont;
	*/
};

