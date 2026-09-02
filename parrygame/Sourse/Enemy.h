#pragma once

enum class State
{
	Idle,
	Telegraph,
	Attack,
	Recovery
};

// 敵キャラクター。Idle -> Telegraph -> Attack -> Recovery -> Idle ...
// というステートマシンで動き、状態遷移とアニメーション、
// パリィ受付時間の判定を担当する。
class Enemy
{
public:
	Enemy();
	~Enemy();
	void Update(float deltaTime);
	void Draw();

	// パリィ受付時間中はtrueを返す。
	// 受付時間はTelegraphの終盤とAttackの開始直後にまたがる
	// (下のParryWindowBeforeAttack / ParryWindowAfterAttack参照)。
	bool IsInParryWindow() const;

	State state_ = State::Idle;

	float Timer = 0.0f;

	// 1回の攻撃サイクル = Idtimer + Tetimer + Attimer + Retimer。
	// 4つまとめて倍率をかければテンポの速さ/遅さを調整できる 
	
	float Idtimer = 0.2f;
	float Tetimer = 0.1f;
	float Attimer = 0.07f;
	float Retimer = 0.1f;

	// パリィ受付時間の調整。
	// Telegraphが終わる何秒前から受付が始まるか、Attackが始まって
	// 何秒間受付が続くか。
	
	float ParryWindowBeforeAttack = 0.15f;
	float ParryWindowAfterAttack = 0.12f;

private:
	// スプライトシートを読み込み、フレームごとに分割する。
	// コンストラクタから一度だけ呼ばれる。
	void LoadGraphics();

	// ある状態のTimer(0 .. duration)がどこまで進んだかと、
	// シートの総フレーム数から、現在表示すべきフレームを求める。
	// Timer=0でフレーム0、その状態が終わるまでは最後のフレームを表示する。
	static int FrameIndex(float timer, float duration, int frameCount);

	
	// IdleはTelegraphとRecoveryのアニメーションも兼ねている - 敵側は
	// 「構え」ポーズを一切使わない仕様なので、専用のTelegraph/Recovery
	// シートは存在せず、どちらもIdleを使い回しても違和感がない。
	static const int kIdleFrameCount = 6;
	static const int kAttackFrameCount = 4;     // Attack_1.png
	int idleHandles_[kIdleFrameCount] = {};
	int attackHandles_[kAttackFrameCount] = {};
};
