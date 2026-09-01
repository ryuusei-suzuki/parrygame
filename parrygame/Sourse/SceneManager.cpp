#include "SceneManager.h"

void SceneManager::ChangeScene(std::unique_ptr<Scene> next)
{
	currentScene_ = std::move(next);
}

void SceneManager::Update(float deltaTime)
{
	currentScene_->Update(deltaTime);
}

void SceneManager::Draw()
{
	currentScene_->Draw();
}
