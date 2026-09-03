#include "TitleScene.h"
#include "ReadyScene.h"
#include <cstring>

// 以下の日本語の文字列リテラル("見切り侍"、"プレイ")についての補足:
// このファイルはUTF-8(BOMあり)で保存してあり、Visual Studioが
// システムのコードページ(932/Shift-JIS)と誤解して文字を壊さないように
// している。ただし、コンパイラに/utf-8(または/execution-charset:utf-8)
// オプションを設定していないため、このマシンでのMSVCのデフォルトの
// 実行時文字コードは依然として932であり、コンパイル後のバイナリでは
// これらの文字列はShift-JISのバイト列に変換される。DxLibの文字列関数も
// デフォルトではShift-JISを前提としているため、
// SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8)を呼ばない限り
// (/utf-8も同時に設定しない限り復活させないこと)、両者の解釈が一致し
// 正しく表示される。
TitleScene::TitleScene(ISceneChanger& changer)
	: changer_(changer)
{
	titleHandle_ = LoadGraph("data/Title.png");

	// ここで一度だけ作成する(毎フレームではない) - 理由は
	// TitleScene.hの宣言部分に書いた注意事項を参照。
	titleFontHandle_ = CreateFontToHandle(NULL, 56, -1, DX_FONTTYPE_NORMAL);
	playFontHandle_ = CreateFontToHandle(NULL, 32, -1, DX_FONTTYPE_NORMAL);
}

TitleScene::~TitleScene()
{
	if (titleFontHandle_ != -1)
	{
		DeleteFontToHandle(titleFontHandle_);
		titleFontHandle_ = -1;
	}
	if (playFontHandle_ != -1)
	{
		DeleteFontToHandle(playFontHandle_);
		playFontHandle_ = -1;
	}
}

void TitleScene::Update(float deltaTime)
{
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	bool isHover = (mouseX >= buttonLeft_ && mouseX <= buttonRight_ &&
		mouseY >= buttonTop_ && mouseY <= buttonBottom_);

	if (isHover && GetMouseInput() & MOUSE_INPUT_LEFT)
	{
		// GameSceneへ直接ではなく、まずReadySceneを挟む
		// (「スペースキーで勝負」の合図画面 - ReadyScene.h参照)。
		changer_.ChangeScene(std::make_unique<ReadyScene>(changer_));
		// 重要: 上のChangeScene()でthisがすでに破棄されている可能性がある
		// (SceneManagerが現在のシーンを差し替えて削除するため)。
		// そのため、この呼び出しの後はTitleSceneのメンバに触れてはいけない。
		return;
	}

	if (isHover)
	{
		// ホバー中であることが分かるように、通常時の黒より少し明るくする。
		buttonColor_ = GetColor(60, 60, 60);
	}
	else {
		buttonColor_ = GetColor(0, 0, 0);
	}
}

void TitleScene::Draw()
{
	DrawExtendGraph(0, 0, 1280, 720, titleHandle_, TRUE);

	// タイトルテキストボックス: 「見切り侍」を画面上部に、黒背景・白文字で
	// 表示する。テキスト幅をフォントハンドルから測って中央に配置している
	// ので、フォントや文言を変えても中央揃えのままになる。
	DrawBox(titleBoxLeft_, titleBoxTop_, titleBoxRight_, titleBoxBottom_, GetColor(0, 0, 0), TRUE);
	{
		const char* titleText = "見切り侍";
		int textWidth = GetDrawStringWidthToHandle(titleText, (int)strlen(titleText), titleFontHandle_);
		int textX = titleBoxLeft_ + ((titleBoxRight_ - titleBoxLeft_) - textWidth) / 2;
		int textY = titleBoxTop_ + ((titleBoxBottom_ - titleBoxTop_) - 56) / 2;
		DrawStringToHandle(textX, textY, titleText, GetColor(255, 255, 255), titleFontHandle_);
	}

	// 「プレイ」ボタン: 画面中央より下に、黒背景(ホバー時は少し明るく)・
	// 白文字で表示する。こちらもテキストを中央揃えにしている。
	DrawBox(buttonLeft_, buttonTop_, buttonRight_, buttonBottom_, buttonColor_, TRUE);
	{
		const char* playText = "プレイ";
		int textWidth = GetDrawStringWidthToHandle(playText, (int)strlen(playText), playFontHandle_);
		int textX = buttonLeft_ + ((buttonRight_ - buttonLeft_) - textWidth) / 2;
		int textY = buttonTop_ + ((buttonBottom_ - buttonTop_) - 32) / 2;
		DrawStringToHandle(textX, textY, playText, GetColor(255, 255, 255), playFontHandle_);
	}
}
