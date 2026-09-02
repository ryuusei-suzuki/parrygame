#include "GameScene.h"
#include "TitleScene.h"
#include <DxLib.h>
#include <cstdio>

namespace
{
	// パリィ入力に使うキー。変更したい場合はここを変える
	// (例: KEY_INPUT_RETURN, KEY_INPUT_Z, ...)。
	constexpr int kParryKey = KEY_INPUT_SPACE;

	// 攻撃が実際に始まる(=Telegraphが終わる)何秒前から「!」警告を
	// 表示するか。この値がTetimer以上の場合は、Telegraph中ずっと
	// 表示され続けることになる。
	constexpr float kWarningLeadTime = 0.4f;
}

GameScene::GameScene(ISceneChanger& changer)
	: changer_(changer)
{
	// ここで一度だけ作成する(毎フレームではない) - 理由はGameScene.hの
	// 宣言部分に書いた注意事項を参照。
	warningFontHandle_ = CreateFontToHandle(NULL, 80, -1, DX_FONTTYPE_NORMAL);

	backgroundHandle_ = LoadGraph("data/Background.png");
}

GameScene::~GameScene()
{
	if (warningFontHandle_ != -1)
	{
		DeleteFontToHandle(warningFontHandle_);
		warningFontHandle_ = -1;
	}
}

void GameScene::Update(float deltaTime)
{
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
		// パリィが実際に成功したこのタイミングでだけ、プレイヤーが
		// 一瞬だけガード/パリィのポーズを見せる(Player::PlayParrySuccess参照)。
		player_.PlayParrySuccess();
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

	if (result_ == GameResult::Cleared)
	{
		DrawString(500, 300, "クリア!", GetColor(0, 255, 0), 0);
		DrawString(430, 330, "SPACEでタイトルへ", GetColor(255, 255, 255), 0);
	}
	else if (result_ == GameResult::GameOver)
	{
		DrawString(480, 300, "ゲームオーバー", GetColor(255, 0, 0), 0);
		DrawString(430, 330, "SPACEでタイトルへ", GetColor(255, 255, 255), 0);
	}
}