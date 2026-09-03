#include "GameScene.h"
#include "TitleScene.h"
#include <DxLib.h>
#include <cstdio>
#include <cstring>

namespace
{
	// パリィ入力に使うキー。変更したい場合はここを変える
	// (例: KEY_INPUT_RETURN, KEY_INPUT_Z, ...)。
	constexpr int kParryKey = KEY_INPUT_SPACE;

	// 攻撃が実際に始まる(=Telegraphが終わる)何秒前から「!」警告を
	// 表示するか。この値がTetimer以上の場合は、Telegraph中ずっと
	// 表示され続けることになる。
	constexpr float kWarningLeadTime = 0.4f;

	// クリア画面の「クリア!」ボックスの位置とサイズ。
	// TitleSceneのタイトルボックスと同じ考え方(黒背景・白文字・中央揃え)。
	constexpr int kClearBoxLeft = 440;
	constexpr int kClearBoxRight = 840;
	constexpr int kClearBoxTop = 250;
	constexpr int kClearBoxBottom = 340;
	constexpr int kClearFontSize = 64;

	// その下に出す「SPACEでタイトルへ」ボックス。
	constexpr int kClearSubBoxLeft = 390;
	constexpr int kClearSubBoxRight = 890;
	constexpr int kClearSubBoxTop = 360;
	constexpr int kClearSubBoxBottom = 400;
}

GameScene::GameScene(ISceneChanger& changer)
	: changer_(changer)
{
	// ここで一度だけ作成する(毎フレームではない) - 理由はGameScene.hの
	// 宣言部分に書いた注意事項を参照。
	warningFontHandle_ = CreateFontToHandle(NULL, 80, -1, DX_FONTTYPE_NORMAL);
	clearFontHandle_ = CreateFontToHandle(NULL, kClearFontSize, -1, DX_FONTTYPE_NORMAL);

	backgroundHandle_ = LoadGraph("data/Background.png");
	clearHandle_ = LoadGraph("data/Clear.png");
}

GameScene::~GameScene()
{
	if (warningFontHandle_ != -1)
	{
		DeleteFontToHandle(warningFontHandle_);
		warningFontHandle_ = -1;
	}
	if (clearFontHandle_ != -1)
	{
		DeleteFontToHandle(clearFontHandle_);
		clearFontHandle_ = -1;
	}
}

void GameScene::Update(float deltaTime)
{
	lastDeltaTime_ = deltaTime;

	// キーが「押された瞬間」だけを検出する(押しっぱなしは含めない)。
	// こうしないと、SPACEを押し続けているだけで何度もパリィしたり
	// 何度もリスタートしたりしてしまう。
	bool keyDown = (CheckHitKey(kParryKey) != 0);
	bool keyJustPressed = keyDown && !prevKeyDown_;
	prevKeyDown_ = keyDown;

	if (result_ != GameResult::Playing)
	{
		// ラウンド終了後: 何かキーが押されたらタイトル画面に戻る。
		if (keyJustPressed)
		{
			changer_.ChangeScene(std::make_unique<TitleScene>(changer_));
		}
		// 重要: 上のChangeScene()でthisがすでに破棄されている可能性がある
		// (SceneManagerが現在のシーンを差し替えて削除するため)。
		// そのため、この呼び出しの後はGameSceneのメンバに触れてはいけない。
		return;
	}

	enemy_.Update(deltaTime);

	// パリィ成功後にプロテクトポーズを表示し続けている場合、敵が刀を
	// 振り終えてRecoveryに入ったタイミングで、プレイヤーも待機ポーズに
	// 戻す。こうすることで「敵が刀を振っている間ずっと受け止め続ける」
	// という見た目になる(Player::PlayParrySuccess参照)。
	if (isGuardingAfterParry_ && enemy_.state_ == State::Recovery)
	{
		player_.EndParrySuccess();
		isGuardingAfterParry_ = false;
	}

	bool isInWindow = enemy_.IsInParryWindow();

	if (isInWindow && !wasInParryWindow_)
	{
		// 今回の攻撃サイクルでパリィ受付が始まった瞬間。
		parriedThisWindow_ = false;
	}

	if (isInWindow && keyJustPressed && !parriedThisWindow_)
	{
		parriedThisWindow_ = true;
		parrySuccessCount_++;
		// パリィが実際に成功した瞬間にプロテクトのポーズを開始する。
		// 敵がまだ刀を振っている(Attack)間はこのまま表示し続け、
		// 上のRecoveryチェックで振り終わったタイミングに合わせて消す。
		player_.PlayParrySuccess();
		isGuardingAfterParry_ = true;
	}

	if (!isInWindow && wasInParryWindow_ && !parriedThisWindow_)
	{
		// 受付時間が終わったのに、間に合わなかった場合。
		parryFailCount_++;
		player_.PlayParryFail();
	}

	wasInParryWindow_ = isInWindow;

	player_.Update(deltaTime);

	// 勝敗判定。
	if (parrySuccessCount_ >= kParrySuccessGoal)
	{
		result_ = GameResult::Cleared;
	}
	else if (parryFailCount_ >= kParryFailLimit)
	{
		result_ = GameResult::GameOver;
	}
}

void GameScene::Draw()
{
	if (result_ == GameResult::Cleared)
	{
		// クリア画面は通常のアリーナ背景ではなく、専用の祝勝演出
		// (data/Clear.png)を表示する。プレイ中のHUDやデバッグ表示は
		// もう不要なので、ここで完結させて早めにreturnしている。
		DrawExtendGraph(0, 0, 1280, 720, clearHandle_, TRUE);

		DrawBox(kClearBoxLeft, kClearBoxTop, kClearBoxRight, kClearBoxBottom, GetColor(0, 0, 0), TRUE);
		{
			const char* clearText = "クリア!";
			int textWidth = GetDrawStringWidthToHandle(clearText, (int)strlen(clearText), clearFontHandle_);
			int textX = kClearBoxLeft + ((kClearBoxRight - kClearBoxLeft) - textWidth) / 2;
			int textY = kClearBoxTop + ((kClearBoxBottom - kClearBoxTop) - kClearFontSize) / 2;
			DrawStringToHandle(textX, textY, clearText, GetColor(255, 255, 255), clearFontHandle_);
		}

		DrawBox(kClearSubBoxLeft, kClearSubBoxTop, kClearSubBoxRight, kClearSubBoxBottom, GetColor(0, 0, 0), TRUE);
		{
			const char* subText = "SPACEでタイトルへ";
			int textWidth = GetDrawStringWidth(subText, (int)strlen(subText));
			int textX = kClearSubBoxLeft + ((kClearSubBoxRight - kClearSubBoxLeft) - textWidth) / 2;
			int textY = kClearSubBoxTop + ((kClearSubBoxBottom - kClearSubBoxTop) - 16) / 2;
			DrawString(textX, textY, subText, GetColor(255, 255, 255), 0);
		}
		return;
	}

	// 背景を最初に描画し、他の要素はすべてその上に重ねる。
	DrawExtendGraph(0, 0, 1280, 720, backgroundHandle_, TRUE);

	player_.Draw();
	enemy_.Draw();

	// Telegraph警告: 剣を構えるポーズを見て察してもらうだけでなく、
	// 敵の下にはっきり「!」を出すことで「攻撃が来る」ことを
	// 誰にでも分かるようにしている。攻撃が実際に始まる
	// (=Telegraphが終わる)kWarningLeadTime秒前から表示され、
	// Telegraphが始まった瞬間からではない。
	if (enemy_.state_ == State::Telegraph &&
		(enemy_.Tetimer - enemy_.Timer) <= kWarningLeadTime)
	{
		DrawStringToHandle(620, 600, "!", GetColor(255, 220, 0), warningFontHandle_);
	}

	DrawString(0, 20, "パリィ: SPACE", GetColor(255, 255, 255), 0);

	char successBuf[64];
	sprintf_s(successBuf, "成功: %d / %d", parrySuccessCount_, kParrySuccessGoal);
	DrawString(0, 40, successBuf, GetColor(255, 255, 255), 0);

	char failBuf[64];
	sprintf_s(failBuf, "失敗: %d / %d", parryFailCount_, kParryFailLimit);
	DrawString(0, 60, failBuf, GetColor(255, 255, 255), 0);

	// 動作が重い環境の切り分け用の簡易デバッグ表示。
	// 1フレームの実時間(ミリ秒)とおおよそのFPSを表示する。
	// この値が異常に大きい/小さいままなら、ステートの進みが遅い原因は
	// ロジック側ではなく、そもそも1フレームの実行に時間がかかっている
	// (=処理が重い)ことになる。
	char frameBuf[64];
	float fps = (lastDeltaTime_ > 0.0f) ? (1.0f / lastDeltaTime_) : 0.0f;
	sprintf_s(frameBuf, "Frame: %.1fms (%.1fFPS)", lastDeltaTime_ * 1000.0f, fps);
	DrawString(0, 80, frameBuf, GetColor(255, 255, 0), 0);

	// Cleared以外でこのDraw()に来るのはPlaying(通常プレイ中)と
	// GameOverの2パターンがあるので、ここは必ずGameOverかどうかを
	// 確認してから表示する(確認を忘れると、プレイ中もずっと
	// 「ゲームオーバー」が表示されたままになってしまう)。
	if (result_ == GameResult::GameOver)
	{
		DrawString(480, 300, "ゲームオーバー", GetColor(255, 0, 0), 0);
		DrawString(430, 330, "SPACEでタイトルへ", GetColor(255, 255, 255), 0);
	}
}