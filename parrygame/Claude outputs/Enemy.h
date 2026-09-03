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
	// 受付時間は、攻撃アニメーションの中で実際に刀が「当たり」の絵になる
	// 瞬間(下のkAttackImpactFrameIndex参照)を基準に、その前後
	// ParryWindowBeforeAttack秒〜ParryWindowAfterAttack秒。
	// Telegraphの終盤からAttackの前半にまたがる。
	bool IsInParryWindow() const;

	// 攻撃アニメーションが「当たり」のフレームに到達した後
	// (Attack状態でTimerがその時点を過ぎている間)trueを返す。
	// GameScene側でfalse->trueに変わった瞬間を検出し、刀を振り抜く
	// 効果音を鳴らすのに使う想定(IsInParryWindow()と同じ使い方)。
	bool IsPastAttackImpact() const;

	State state_ = State::Idle;

	float Timer = 0.0f;

	// 1回の攻撃サイクル = Idtimer + Tetimer + Attimer + Retimer。
	// 4つまとめて倍率をかければテンポの速さ/遅さを調整できる -
	// Telegraphが一番長く、一番「見て反応しやすい」フェーズであるという
	// 比率はできるだけ保つこと。
	float Idtimer = 0.2f;
	float Tetimer = 0.1f;
	float Attimer = 0.07f;
	float Retimer = 0.1f;

	// パリィ受付時間の調整。
	// 刀が「当たり」の絵になる瞬間(kAttackImpactFrameIndex参照)の
	// 何秒前から受付が始まるか、その何秒後まで受付が続くか。
	// 重要: ParryWindowBeforeAttackは「当たりの瞬間までの残り時間」の
	// 最大値、ParryWindowAfterAttackは「当たりの瞬間を過ぎてからAttack
	// 状態が終わるまでの残り時間」を超えないようにすること。超えても
	// 動作はするが、その分は実質的に切り詰められる(前者はTelegraph開始
	// より前には遡れず、後者はRecoveryに入った瞬間に強制的に受付終了
	// するため)。
	float ParryWindowBeforeAttack = 0.06f;
	float ParryWindowAfterAttack = 0.04f;

private:
	// スプライトシートを読み込み、フレームごとに分割する。
	// コンストラクタから一度だけ呼ばれる。
	void LoadGraphics();

	// ある状態のTimer(0 .. duration)がどこまで進んだかと、
	// シートの総フレーム数から、現在表示すべきフレームを求める。
	// Timer=0でフレーム0、その状態が終わるまでは最後のフレームを表示する。
	static int FrameIndex(float timer, float duration, int frameCount);

	// スプライトシート。LoadDivGraphで1フレームごとのグラフィック
	// ハンドルに分割している。各フレームは128x128(data/Samurai/*.png参照)。
	// IdleはTelegraphとRecoveryのアニメーションも兼ねている - 敵側は
	// 「構え」ポーズを一切使わない仕様なので、専用のTelegraph/Recovery
	// シートは存在せず、どちらもIdleを使い回しても違和感がない。
	static const int kIdleFrameCount = 6;
	static const int kAttackFrameCount = 4;     // Attack_1.png
	int idleHandles_[kIdleFrameCount] = {};
	int attackHandles_[kAttackFrameCount] = {};

	// Attack_1.png(全kAttackFrameCount枚)のうち、実際に刀が伸びきって
	// 軌跡エフェクトが出る「当たり」の絵になるのは3枚目(インデックス2)。
	// IsInParryWindow()とIsPastAttackImpact()は、Attack状態の経過時間
	// そのものではなく、このフレームが始まる瞬間を基準に判定している。
	static const int kAttackImpactFrameIndex = 2;
};
