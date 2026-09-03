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
	Timer += deltaTime;

	// whileループにしているのは、フレームレートが極端に低いPCなどで
	// 1回のUpdate呼び出しに渡されるdeltaTimeが大きくなった場合に
	// 対応するため。以前はif文1つだけだったので、1回のUpdateで
	// 複数の状態をまたぐほど時間が経過していても遷移が1回しか
	// 起こらず、その分Timerが0にリセットされて余りの時間が
	// 消えてしまっていた。フレームがまばらにしか来ない環境だと、
	// これが積み重なって「ステートが一周するのに何十秒もかかる」
	// ように見える原因になる。ここでは超過分(Timer -= 各durationの
	// 余り)を次の状態に持ち越しながら、必要な回数だけ遷移させている。
	bool transitioned = true;
	while (transitioned)
	{
		transitioned = false;
		switch (state_)
		{
		case State::Idle:
			if (Timer >= Idtimer)
			{
				Timer -= Idtimer;
				state_ = State::Telegraph;
				transitioned = true;
			}
			break;
		case State::Telegraph:
			if (Timer >= Tetimer)
			{
				Timer -= Tetimer;
				state_ = State::Attack;
				transitioned = true;
			}
			break;
		case State::Attack:
			if (Timer >= Attimer)
			{
				Timer -= Attimer;
				state_ = State::Recovery;
				transitioned = true;
			}
			break;
		case State::Recovery:
			if (Timer >= Retimer)
			{
				Timer -= Retimer;
				state_ = State::Idle;
				transitioned = true;
			}
			break;
		}
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
	// 攻撃の「当たり」の瞬間(kAttackImpactFrameIndex枚目のフレームが
	// 始まる瞬間)までの残り時間を、Telegraph/Attackどちらの状態にいても
	// 同じ基準で計算する。受付時間はこの瞬間の直前ParryWindowBeforeAttack秒
	// 〜直後ParryWindowAfterAttack秒。
	float impactTimeInAttack = Attimer * kAttackImpactFrameIndex / kAttackFrameCount;

	float timeUntilImpact;
	if (state_ == State::Telegraph)
	{
		timeUntilImpact = (Tetimer - Timer) + impactTimeInAttack;
	}
	else if (state_ == State::Attack)
	{
		timeUntilImpact = impactTimeInAttack - Timer;
	}
	else
	{
		return false;
	}

	return timeUntilImpact <= ParryWindowBeforeAttack && timeUntilImpact >= -ParryWindowAfterAttack;
}

bool Enemy::IsPastAttackImpact() const
{
	if (state_ != State::Attack)
	{
		return false;
	}
	float impactTimeInAttack = Attimer * kAttackImpactFrameIndex / kAttackFrameCount;
	return Timer >= impactTimeInAttack;
}