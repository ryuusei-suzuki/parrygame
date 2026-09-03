#include "ReadyScene.h"
#include "GameScene.h"
#include <cstring>
#include <cstdio>

// 日本語文字列リテラルのエンコーディングについては、TitleScene.cppの
// 冒頭コメントを参照。このファイルも同じくUTF-8(BOMあり)で保存し、
// SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8)は呼ばないこと。

namespace
{
	// GameScene::kParrySuccessGoal / kParryFailLimit と同じ値。
	// この画面で勝敗条件を説明するために使う。GameScene側の対応する
	// 定数はprivateで直接参照できないので、ここに同じ値を持たせている。
	// 注意: GameScene側の閾値を変更したときは、こちらも忘れずに
	// 合わせて変更すること。
	constexpr int kParrySuccessGoal = 5;
	constexpr int kParryFailLimit = 3;
}

ReadyScene::ReadyScene(ISceneChanger& changer)
	: changer_(changer)
{
	fontHandle_ = CreateFontToHandle(NULL, 48, -1, DX_FONTTYPE_NORMAL);
}

ReadyScene::~ReadyScene()
{
	if (fontHandle_ != -1)
	{
		DeleteFontToHandle(fontHandle_);
		fontHandle_ = -1;
	}
}

void ReadyScene::Update(float deltaTime)
{
	// キーが「押された瞬間」だけを検出する(押しっぱなしは含めない)。
	bool keyDown = (CheckHitKey(KEY_INPUT_SPACE) != 0);
	bool keyJustPressed = keyDown && !prevKeyDown_;
	prevKeyDown_ = keyDown;

	if (keyJustPressed)
	{
		changer_.ChangeScene(std::make_unique<GameScene>(changer_));
		// 重要: 上のChangeScene()でthisがすでに破棄されている可能性がある
		// (SceneManagerが現在のシーンを差し替えて削除するため)。
		// そのため、この呼び出しの後はReadySceneのメンバに触れてはいけない。
		return;
	}
}

void ReadyScene::Draw()
{
	// 背景画像は使わず、黒地に文字だけを画面中央に表示する。
	// main.cppのループが毎フレームClearDrawScreen()で画面を黒に
	// クリアしているので、ここでは文字を描くだけでよい。
	const char* text = "スペースキーで勝負";
	int textWidth = GetDrawStringWidthToHandle(text, (int)strlen(text), fontHandle_);
	int textX = (1280 - textWidth) / 2;
	int textY = (720 - 48) / 2;
	DrawStringToHandle(textX, textY, text, GetColor(255, 255, 255), fontHandle_);

	// 操作方法(「!」が出たらSPACEでパリィ)と勝敗条件を、
	// メインの見出しの下にまとめて表示する。すべて中央揃えにするため、
	// 各行ごとに幅を測り直している。
	{
		const char* howToText = "「!」が出たらSPACEでパリィ!";
		int w = GetDrawStringWidth(howToText, (int)strlen(howToText));
		DrawString((1280 - w) / 2, textY + 80, howToText, GetColor(255, 255, 255), 0);
	}

	{
		char successBuf[64];
		sprintf_s(successBuf, "%d回成功でクリア", kParrySuccessGoal);
		int w = GetDrawStringWidth(successBuf, (int)strlen(successBuf));
		DrawString((1280 - w) / 2, textY + 110, successBuf, GetColor(255, 255, 255), 0);
	}

	{
		char failBuf[64];
		sprintf_s(failBuf, "%d回失敗でゲームオーバー", kParryFailLimit);
		int w = GetDrawStringWidth(failBuf, (int)strlen(failBuf));
		DrawString((1280 - w) / 2, textY + 140, failBuf, GetColor(255, 255, 255), 0);
	}
}
