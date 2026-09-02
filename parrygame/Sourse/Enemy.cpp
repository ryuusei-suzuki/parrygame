#include "Enemy.h"
#include <DxLib.h>

namespace
{
	// このパックのシートはすべて128x128のフレームで構成されている。
	constexpr int kFrameSize = 128;

	// 画面上の描画位置とサイズ(1280x720キャンバス)。
	// プレイヤー側の位置/サイズと左右対称にすることで、画面中央から
	// 同じ距離に立ち、きちんと向き合う「vs」の構図になるようにしている
	// (敵: kDrawX 708..1092、プレイヤー: kDrawX 188..572、どちらも
	// 384px四方、同じkDrawY - 大きさも立ち位置の高さも左右で揃えている)。
	// kDrawYは、足元(kDrawY + kDrawSize)がdata/Background.pngの
	// 地面のラインにちょうど着地するように調整してある(浮かないように)。
	constexpr int kDrawX = 708;
	constexpr int kDrawY = 224;
	constexpr int kDrawSize = 384;
}

Enemy::Enemy()
{
	LoadGraphics();
}

Enemy::~Enemy()
{
}

void Enemy::LoadGraphics()
{
	LoadDivGraph("data/Samurai/Idle.png", kIdleFrameCount, kIdleFrameCount, 1,
		kFrameSize, kFrameSize, idleHandles_);
	LoadDivGraph("data/Samurai/Attack_1.png", kAttackFrameCount, kAttackFrameCount, 1,
		kFrameSize, kFrameSize, attackHandles_);
}

int Enemy::FrameIndex(float timer, float duration, int frameCount)
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

void Enemy::Update(float deltaTime)
{
	switch (state_)
	{
	case State::Idle:
		Timer += deltaTime;
		if (Timer >= Idtimer)
		{
			state_ = State::Telegraph;
			Timer = 0.0f;
		}
		break;
	case State::Telegraph:
		Timer += deltaTime;
		if (Timer >= Tetimer)
		{
			state_ = State::Attack;
			Timer = 0.0f;
		}
		break;
	case State::Attack:
		Timer += deltaTime;
		if (Timer >= Attimer)
		{
			state_ = State::Recovery;
			Timer = 0.0f;
		}
		break;
	case State::Recovery:
		Timer += deltaTime;
		if (Timer >= Retimer)
		{
			state_ = State::Idle;
			Timer = 0.0f;
		}
		break;
	}
}

void Enemy::Draw()
{
	int handle = idleHandles_[0];

	switch (state_)
	{
	case State::Idle:
		handle = idleHandles_[FrameIndex(Timer, Idtimer, kIdleFrameCount)];
		DrawString(0, 0, "待機", GetColor(255, 255, 255), 0);
		break;
	case State::Telegraph:
		// 敵側はもう「構え」ポーズを使わない - Idleのまま待機させ、
		// 攻撃が来ることは「!」の警告表示で伝える(GameScene::Draw参照)。
		handle = idleHandles_[FrameIndex(Timer, Tetimer, kIdleFrameCount)];
		DrawString(0, 0, "予備動作", GetColor(255, 255, 255), 0);
		break;
	case State::Attack:
		handle = attackHandles_[FrameIndex(Timer, Attimer, kAttackFrameCount)];
		DrawString(0, 0, "攻撃", GetColor(255, 255, 255), 0);
		break;
	case State::Recovery:
		// パックに専用の硬直ポーズがないので、Idleの最終フレームで止める。
		handle = idleHandles_[kIdleFrameCount - 1];
		DrawString(0, 0, "硬直", GetColor(255, 255, 255), 0);
		break;
	}

	DrawExtendGraph(kDrawX, kDrawY, kDrawX + kDrawSize, kDrawY + kDrawSize, handle, TRUE);
}

bool Enemy::IsInParryWindow() const
{
	if (state_ == State::Telegraph)
	{
		return Timer >= (Tetimer - ParryWindowBeforeAttack);
	}
	if (state_ == State::Attack)
	{
		return Timer <= ParryWindowAfterAttack;
	}
	return false;
}
