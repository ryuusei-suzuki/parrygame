#pragma once
#include <DxLib.h>
#include "Scene.h"
#include "ISceneChanger.h"

// タイトル画面。「プレイ」ボタンをクリックするとGameSceneへ遷移する。
class TitleScene : public Scene
{
public:
	TitleScene(ISceneChanger& changer);
	~TitleScene() override;
	void Update(float deltaTime) override;
	void Draw()   override;

private:
	ISceneChanger& changer_;

	// 「プレイ」ボタン - ホバー中にクリックするとゲームを開始する。
	// 水平方向は中央揃え(キャンバス幅1280)、垂直方向は中央より下。
	int buttonLeft_ = 475;
	int buttonRight_ = 805;
	int buttonTop_ = 560;
	int buttonBottom_ = 630;
	unsigned int buttonColor_;

	// タイトルテキストボックス(「見切り侍」)。画面上部に固定表示する。
	int titleBoxLeft_ = 390;
	int titleBoxRight_ = 890;
	int titleBoxTop_ = 40;
	int titleBoxBottom_ = 120;

	int titleHandle_;

	// 「見切り侍」タイトルテキスト用の大きいフォント。一度だけ作成して
	// 使い回す - 理由はGameScene.hのwarningFontHandle_の注意事項を参照
	// (毎フレームSetFontSize()+DrawString()をしないこと)。
	int titleFontHandle_ = -1;

	// 「プレイ」ボタンの文字用のフォント。デフォルトフォントより
	// 大きく表示するために用意している。
	int playFontHandle_ = -1;
};
