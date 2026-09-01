#pragma once
#include <DxLib.h>
#include "Scene.h"
#include "ISceneChanger.h"
class TitleScene : public Scene
{
public:
	TitleScene(ISceneChanger& changer);
	~TitleScene() override;
	void Update(float deltaTime) override;
	void Draw()   override;

private:
	ISceneChanger& changer_;

	int buttonLeft_   = 580;
	int buttonRight_  = 910;
	int buttonTop_    = 560;
	int buttonBottom_ = 630;
	unsigned int buttonColor_;
	int titleHandle_;
};
