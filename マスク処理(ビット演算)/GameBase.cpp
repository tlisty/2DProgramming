#include "GameBase.h"



GameBase::GameBase()
/*
	: mText( "" )
	, mpFont( nullptr )
	*/
	{
}


GameBase::~GameBase()
{
}

//初期化関数（失敗するときはfalseを返すようにする）
bool GameBase::Init()
{
	return true;
}