#pragma once
#include <DxLib.h>
#include "Scene.h"
#include "ISceneChanger.h"
class TitleScene : public Scene
{
public:
	TitleScene(ISceneChanger& changer);
	~TitleScene() override;
	void Update() override;
	void Draw()   override;

private:
	ISceneChanger& changer_;

	int buttonLeft_   = 300;
	int buttonRight_  = 500;
	int buttonTop_    = 400;
	int buttonBottom_ = 450;
	unsigned int buttonColor_;
};

