#include "GameScene.h"
#include <DxLib.h>
GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::Update()
{

}

void GameScene::Draw()
{
	DrawString(0, 0, "Game Scene", GetColor(255, 255, 255), 0);
}
