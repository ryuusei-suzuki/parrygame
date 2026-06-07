#include "TitleScene.h"
#include "GameScene.h"
TitleScene::TitleScene(ISceneChanger& changer)
	: changer_(changer)  //changer_に保存
{

}

TitleScene::~TitleScene()
{
}

void TitleScene::Update()
{
	//マウス座標を取得
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	bool isHover = (mouseX >= buttonLeft_ && mouseX <= buttonRight_ &&
					mouseY >= buttonTop_ && mouseY <= buttonBottom_);

	if (isHover && GetMouseInput() & MOUSE_INPUT_LEFT)
	{
		changer_.ChangeScene(std::make_unique<GameScene>());
	}
	if (isHover)
	{
		buttonColor_ = GetColor(0, 255, 255);//水色
	}
	else {
		buttonColor_ = GetColor(255, 255, 255);//白
	}
}

void TitleScene::Draw()
{
	DrawString(0, 0, "見切り侍", GetColor(255, 255, 255), 0);
	DrawBox(buttonLeft_, buttonTop_, buttonRight_, buttonBottom_, buttonColor_, TRUE);
	DrawString(buttonLeft_ + 50, buttonTop_ + 15, "START", GetColor(0, 0, 0), 0);
}
