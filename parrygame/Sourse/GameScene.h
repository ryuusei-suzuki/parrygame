#pragma once
#include "Scene.h"
#include "Enemy.h"
#include "Player.h"
#include "ISceneChanger.h"

// メインのゲームプレイシーン。敵とプレイヤーを保持し、
// パリィの成否判定、勝敗判定、画面描画をすべて担当する。
class GameScene : public Scene
{
public:
	GameScene(ISceneChanger& changer);
	~GameScene() override;
	void Update(float deltaTime) override;
	void Draw() override;

private:
	enum class GameResult
	{
		Playing,
		Cleared,
		GameOver
	};

	ISceneChanger& changer_;

	Enemy enemy_;
	Player player_;

	GameResult result_ = GameResult::Playing;

	// --- パリィ判定用の状態 ---
	bool wasInParryWindow_ = false;      // 直前のフレームでパリィ受付中だったか
	bool parriedThisWindow_ = false;     // 今回の受付中にすでにパリィを成功させたか
	bool prevKeyDown_ = false;           // キーが「押された瞬間」を検出するための直前フレームの状態
	bool isGuardingAfterParry_ = false;  // パリィ成功後、まだプレイヤーがプロテクトポーズ中か
	// (敵がRecoveryに入ったらEndParrySuccess()で解除する)

	int parrySuccessCount_ = 0;
	int parryFailCount_ = 0;

	// 1ラウンドの長さや難易度を変えたいときはここを調整する。
	static const int kParrySuccessGoal = 5;
	static const int kParryFailLimit = 3;

	// Telegraph中の「!」警告表示用の大きいフォント。一度だけ作成して使い回す。
	// 重要: 毎フレームSetFontSize()/DrawString()を呼ばないこと -
	// SetFontSizeは呼ぶたびにデフォルトフォントのGDIリソースを作り直すため、
	// 1秒間に60回もそれをやるとハンドルがリークし、数秒で処理が重くなる。
	int warningFontHandle_ = -1;

	// クリア画面の「クリア!」表示用の大きいフォント。理由は
	// warningFontHandle_と同じくここで一度だけ作成して使い回す。
	int clearFontHandle_ = -1;

	// 対戦アリーナの背景(data/Background.png)。一度だけ読み込み、
	// 毎フレーム一番最初(他のすべての奥)に描画する - GameScene::Draw参照。
	int backgroundHandle_ = -1;

	// クリア画面専用の背景(data/Clear.png)。同じ城を、花吹雪と花火で
	// 祝勝ムードに演出したもの - result_がClearedのときだけ、
	// backgroundHandle_の代わりにこちらを描画する。
	int clearHandle_ = -1;

	// 直近フレームのdeltaTime(秒)。動作が重い環境で本当に処理が
	// 遅いのか(1フレームの実時間が長い)を確認するための簡易的な
	// デバッグ表示用 - GameScene::Draw参照。
	float lastDeltaTime_ = 0.0f;
};