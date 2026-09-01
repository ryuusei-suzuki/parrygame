#include "TitleScene.h"
#include "GameScene.h"

TitleScene::TitleScene(ISceneChanger& changer)
	: changer_(changer)  //changer_に保存
{
	titleHandle_ = LoadGraph("data/Title.png");
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update(float deltaTime)
{
	//マウス座標を取得
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	bool isHover = (mouseX >= buttonLeft_ && mouseX <= buttonRight_ &&
					mouseY >= buttonTop_ && mouseY <= buttonBottom_);//マウスがボックスに触れているか

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
	DrawExtendGraph(0, 0, 1280, 720, titleHandle_, TRUE);
	DrawBox(buttonLeft_, buttonTop_, buttonRight_, buttonBottom_, buttonColor_, TRUE);
	DrawString(buttonLeft_ + 50, buttonTop_ + 15, "hajimeru", GetColor(0, 0, 0), 0);
}
