#pragma once
#include <DxLib.h>
#include "Scene.h"
#include "ISceneChanger.h"

// タイトルの「プレイ」を押した直後に一瞬挟む、対戦前の合図画面。
// 背景画像などは出さず、黒地に「スペースキーで勝負」という文字を
// 画面中央に表示する。あわせて、「!」が出たらSPACEでパリィする
// という操作方法と、何回成功したらクリア/何回失敗したらゲームオーバーに
// なるかという勝敗条件もここでまとめて説明する(以前はGameScene側の
// プレイ中HUDに出していたが、プレイ中の画面をシンプルにするため
// こちらに移した)。SPACEキーを押すとGameSceneへ遷移する。
class ReadyScene : public Scene
{
public:
	ReadyScene(ISceneChanger& changer);
	~ReadyScene() override;
	void Update(float deltaTime) override;
	void Draw()   override;

private:
	ISceneChanger& changer_;

	// キーが「押された瞬間」を検出するための直前フレームの状態。
	// GameScene::Updateと同じ理由(押しっぱなしで何度も反応しないように)。
	bool prevKeyDown_ = false;

	// 「スペースキーで勝負」テキスト用の大きいフォント。一度だけ作成して
	// 使い回す - 理由はGameScene.hのwarningFontHandle_の注意事項を参照
	// (毎フレームSetFontSize()+DrawString()をしないこと)。
	int fontHandle_ = -1;
};
