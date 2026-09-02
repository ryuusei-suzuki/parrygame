#include "Player.h"
#include <DxLib.h>

namespace
{
	// このパックのシートはすべて128x128のフレームで構成されている。
	constexpr int kFrameSize = 128;

	// 画面上の描画位置とサイズ(1280x720キャンバス)。
	// 敵側の位置/サイズと左右対称にすることで、画面中央から同じ距離に
	// 立ち、きちんと向き合う「vs」の構図になるようにしている
	// (プレイヤー: kDrawX 188..572、敵: Enemy.cppのkDrawX 708..1092、
	// どちらも384px四方、同じkDrawY - 大きさも立ち位置の高さも
	// 左右で揃えている)。
	// kDrawYは、足元(kDrawY + kDrawSize)がdata/Background.pngの
	// 地面のラインにちょうど着地するように調整してある(浮かないように)。
	constexpr int kDrawX = 188;
	constexpr int kDrawY = 224;
	constexpr int kDrawSize = 384;

	// ループする待機アニメーションの1フレームあたりの秒数。
	constexpr float kIdleFrameDuration = 0.12f;
}

Player::Player()
{
	LoadGraphics();
}

Player::~Player()
{
}

void Player::LoadGraphics()
{
	LoadDivGraph("data/Player/Idle.png", kIdleFrameCount, kIdleFrameCount, 1,
		kFrameSize, kFrameSize, idleHandles_);
	LoadDivGraph("data/Player/Protect.png", kParryFrameCount, kParryFrameCount, 1,
		kFrameSize, kFrameSize, parryHandles_);
	LoadDivGraph("data/Player/Hurt.png", kHurtFrameCount, kHurtFrameCount, 1,
		kFrameSize, kFrameSize, hurtHandles_);
}

int Player::FrameIndex(float timer, float duration, int frameCount)
{
	if (duration <= 0.0f || frameCount <= 1)
	{
		return 0;
	}
	int index = static_cast<int>((timer / duration) * frameCount);
	if (index < 0)
	{
		index = 0;
	}
	if (index > frameCount - 1)
	{
		index = frameCount - 1;
	}
	return index;
}

void Player::PlayParrySuccess()
{
	successTimer_ = kParrySuccessDuration;
}

void Player::PlayParryFail()
{
	hurtTimer_ = kHurtDuration;
}

void Player::Update(float deltaTime)
{
	idleTimer_ += deltaTime;

	if (successTimer_ > 0.0f)
	{
		successTimer_ -= deltaTime;
		if (successTimer_ < 0.0f)
		{
			successTimer_ = 0.0f;
		}
	}

	if (hurtTimer_ > 0.0f)
	{
		hurtTimer_ -= deltaTime;
		if (hurtTimer_ < 0.0f)
		{
			hurtTimer_ = 0.0f;
		}
	}
}

void Player::Draw()
{
	int handle;

	if (hurtTimer_ > 0.0f)
	{
		// hurtTimer_がkHurtDurationから0へ減っていくのに合わせて、
		// 最初のフレームから最後のフレームへ再生する。
		float elapsed = kHurtDuration - hurtTimer_;
		handle = hurtHandles_[FrameIndex(elapsed, kHurtDuration, kHurtFrameCount)];
	}
	else if (successTimer_ > 0.0f)
	{
		// successTimer_がkParrySuccessDurationから0へ減っていくのに
		// 合わせて、最初のフレームから最後のフレームへ再生する -
		// パリィ成功の瞬間にだけ出る、一瞬のガード/パリィフラッシュ。
		// 敵の攻撃タイミングには同期していない。
		float elapsed = kParrySuccessDuration - successTimer_;
		handle = parryHandles_[FrameIndex(elapsed, kParrySuccessDuration, kParryFrameCount)];
	}
	else
	{
		int frame = static_cast<int>(idleTimer_ / kIdleFrameDuration) % kIdleFrameCount;
		handle = idleHandles_[frame];
	}

	// Samurai_Commanderのシートは素材パックの時点ですでに右向きで、
	// これはプレイヤー(画面左側に立ち、右側の敵と向き合う)に
	// ちょうど必要な向きなので、反転せずそのまま描画している。
	// (敵側のシートは逆に、画面右側に立って左を向く必要があるため
	// 事前に左右反転してある - Enemy.cpp参照。)
	DrawExtendGraph(kDrawX, kDrawY, kDrawX + kDrawSize, kDrawY + kDrawSize, handle, TRUE);
}
