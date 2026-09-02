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
	bool wasInParryWindow_ = false;   // 直前のフレームでパリィ受付中だったか
	bool parriedThisWindow_ = false;  // 今回の受付中にすでにパリィを成功させたか
	bool prevKeyDown_ = false;        // キーが「押された瞬間」を検出するための直前フレームの状態

	int parrySuccessCount_ = 0;
	int parryFailCount_ = 0;

	// 1ラウンドの長さや難易度を変えたいときはここを調整する。
	static const int kParrySuccessGoal = 5;
	static const int kParryFailLimit = 3;

	// Telegraph中の「!」警告表示用の大きいフォント。一度だけ作成して使い回す。
	
	int warningFontHandle_ = -1;

	// 対戦アリーナの背景(data/Background.png)。一度だけ読み込み、
	// 毎フレーム一番最初(他のすべての奥)に描画する - GameScene::Draw参照。
	int backgroundHandle_ = -1;
};