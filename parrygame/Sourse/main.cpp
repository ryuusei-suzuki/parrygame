#include "DxLib.h"
#include "SceneManager.h"
#include "TitleScene.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(1280, 720, 32);
	ChangeWindowMode(TRUE);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	int nowTime = GetNowCount();
	SetMouseDispFlag(TRUE);//マウスポインター表示
	SceneManager sceneManager;
	sceneManager.ChangeScene(std::make_unique<TitleScene>(sceneManager));

	while (ProcessMessage() == 0)
	{
		int nowCount = GetNowCount();//現在の経過時間
		float elapsedTime = nowCount - nowTime;//経過みり秒
		float deltaTime = elapsedTime / 1000.0f;//deltaTime setting

		ClearDrawScreen();

		sceneManager.Update(deltaTime);     // 更新
		sceneManager.Draw();       // 描画

		ScreenFlip();              // 画面を更新

		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			return 0;				// ソフトの終了
		}
		nowTime = GetNowCount();
	}
}
