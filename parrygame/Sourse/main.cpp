#include "DxLib.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include <Windows.h> // Sleep()

namespace
{
	// ゲーム全体の上限フレームレート。
	// SetWaitVSyncFlag(FALSE)で垂直同期待ちを切っているため、
	// 何も制限しないとPCによってはCPUを無駄に全力で回し続けてしまう
	// (ウィンドウが非アクティブな間のフリーズ対策のためVSync待ちには
	// 戻せないので、代わりにここで明示的に上限を設けている)。
	constexpr int kTargetFPS = 30;
	constexpr int kFrameIntervalMs = 1000 / kTargetFPS;
}

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
	// その代わり、フレームレートの上限は下のSleep()で自前にかけている。
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

		// 次のフレームのdeltaTime計算の基準点は、ここ(まだ何も処理していない
		// 「このフレームの開始時刻」)にしておく必要がある。以前はこの
		// 代入をSleep()の後(ループ最後)でやっていたため、「今フレームの
		// 開始」から「Sleepが終わった瞬間」までの時間(=処理時間+Sleep時間、
		// つまりだいたい1フレーム分まるまる)がnowTimeに含まれてしまい、
		// 次のフレーム冒頭でのnowCount(ほぼ間を置かずGetNowCountした値)との
		// 差がほぼ0になっていた。結果、deltaTimeが常にほぼ0秒になり、
		// 「Frame: 0.0ms (0.0FPS)」のままステートが全く進まなくなって
		// いた(FPS上限を付ける前は問題なかった箇所)。
		nowTime = nowCount;

		ClearDrawScreen();

		sceneManager.Update(deltaTime); // 現在のシーンを更新
		sceneManager.Draw();            // 現在のシーンを描画

		ScreenFlip();

		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			return 0;
		}

		// フレームレート上限(kTargetFPS)。この1フレームの処理に
		// かかった時間が目標フレーム時間より短ければ、残りの時間だけ
		// 待つ。ウィンドウの状態に関係なく一定時間待つだけなので、
		// SetAlwaysRunFlagの効果を打ち消すことはない。
		// (DxLib独自のWaitTimer()ではなく、標準のWinAPI Sleep()を
		// 使っている - 挙動がはっきりしていて確実なため。)
		int afterFlip = GetNowCount();
		int frameElapsed = afterFlip - nowCount;
		if (frameElapsed < kFrameIntervalMs)
		{
			Sleep(kFrameIntervalMs - frameElapsed);
		}
	}

	// 上のループはProcessMessage()がウィンドウを閉じる合図(WM_QUIT)を
	// 返したときだけ抜ける。ここで明示的にreturnしないと、WinMainが
	// 戻り値なしで終端まで落ちてしまい未定義動作になる
	// (MSVCの警告: C4715 - 全ての制御パスが値を返すわけではない)。
	return 0;
}