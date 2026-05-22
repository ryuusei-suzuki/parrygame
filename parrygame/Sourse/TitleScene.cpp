#include "TitleScene.h"

TitleScene::TitleScene()
{

}

TitleScene::~TitleScene()
{
}

void TitleScene::Update()
{
	if (CheckHitKey(KEY_INPUT_SPACE))
	{
		
	}
}

void TitleScene::Draw()
{
	DrawString(0, 0, "Title Scene", GetColor(255, 255, 255), 0);
}
