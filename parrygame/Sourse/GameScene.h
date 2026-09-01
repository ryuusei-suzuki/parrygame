#pragma once
#include "Scene.h"
#include "Enemy.h"
class GameScene : public Scene
{
public:
	GameScene();
	~GameScene() override;
	void Update(float deltaTime) override;
	void Draw() override;

private:
	Enemy enemy_;
};
