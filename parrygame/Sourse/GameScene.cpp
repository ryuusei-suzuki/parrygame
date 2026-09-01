#include "GameScene.h"
#include <DxLib.h>

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::Update(float deltaTime)
{
	enemy_.Update(deltaTime);
}

void GameScene::Draw()
{
	enemy_.Draw();
}
