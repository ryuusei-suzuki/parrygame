#include "DxLib.h"
#include "SceneManager.h"
#include "TitleScene.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(1280, 720, 32);
	ChangeWindowMode(TRUE);

	// これを設定しないと、ゲームウィンドウが非アクティブな間はDxLibが
	// Update/Drawの実行をほぼ止めてしまい、ウィンドウをクリックしたり
	// キーを押したりするまでゲームが「フリーズ」しているように見える。
	SetAlwaysRunFlag(TRUE);

	// ScreenFlip()は通常、垂直同期を待つ。ウィンドウが非アクティブ/非表示の
	// 間は、この待ち時間が1フレーム分よりはるかに長くなることがある
	// (Windowsが非アクティブ/隠れたウィンドウへの垂直同期の配信を止める
	// ことがあるため)。これをオフにすることでScreenFlip()が待たずに
	// 即座に返るようになる。これが「ウィンドウをクリックするまでフリーズ
	// する」症状のもう半分の原因で、SetAlwaysRunFlagだけではカバーできない。
	SetWaitVSyncFlag(FALSE);

	// DxLibの初期化
	if (DxLib_Init() == -1)
	{
		return -1;
	}

	int nowTime = GetNowCount();
	SetMouseDispFlag(TRUE);

	SceneManager sceneManager;
	sceneManager.ChangeScene(std::make_unique<TitleScene>(sceneManager));

	while (ProcessMessage() == 0)
	{
		int nowCount = GetNowCount();
		float elapsedTime = nowCount - nowTime;
		float deltaTime = elapsedTime / 1000.0f; // ミリ秒 -> 秒

		ClearDrawScreen();

		sceneManager.Update(deltaTime); // 現在のシーンを更新
		sceneManager.Draw();            // 現在のシーンを描画

		ScreenFlip();

		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			return 0;
		}
		nowTime = GetNowCount();
	}

	// 上のループはProcessMessage()がウィンドウを閉じる合図(WM_QUIT)を
	// 返したときだけ抜ける。ここで明示的にreturnしないと、WinMainが
	// 戻り値なしで終端まで落ちてしまい未定義動作になる
	// (MSVCの警告: C4715 - 全ての制御パスが値を返すわけではない)。
	return 0;
}