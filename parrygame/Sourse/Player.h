#pragma once

// プレイヤー側のキャラクター。純粋に見た目だけの存在で、画面左側に
// 立って敵と向き合う。パリィが成功すると、敵が実際に刀を振っている
// (Attack)間ずっとプロテクトのポーズを見せ続け(EndParrySuccess()が
// 呼ばれるまで)、パリィに失敗すると「被弾」リアクションを再生する。
// それ以外は待機(Idle)しているだけ。
class Player
{
public:
	Player();
	~Player();
	void Update(float deltaTime);
	void Draw();

	// パリィが成功した瞬間に呼ぶ。呼んだ瞬間からEndParrySuccess()が
	// 呼ばれるまでの間、Draw()はプロテクトのポーズを表示し続ける。
	// 呼び出し側(GameScene)は、敵が刀を振り終えてRecoveryに移った
	// タイミングでEndParrySuccess()を呼ぶ想定 - こうすることで
	// 「敵が刀を振っている間、プレイヤーがそれを受け止め続ける」
	// という見た目になる。
	void PlayParrySuccess();

	// 敵の攻撃(刀を振る動作)が終わったときに呼ぶ。プロテクトポーズの
	// 表示を終え、待機ポーズに戻す。
	void EndParrySuccess();

	// パリィに失敗したときに呼ぶ。プレイヤーキャラクターが反応する
	// (待機ポーズを一瞬「被弾」リアクションで上書きする)。
	void PlayParryFail();

private:
	// スプライトシートを読み込み、フレームごとに分割する。
	// コンストラクタから一度だけ呼ばれる。
	void LoadGraphics();

	// 'duration'秒のうちどこまで経過したか(0 .. duration)と、
	// シートの総フレーム数から、現在表示すべきフレームを求める。
	static int FrameIndex(float timer, float duration, int frameCount);

	float idleTimer_ = 0.0f;      // ループする待機アニメーションの経過時間
	bool isGuarding_ = false;     // trueの間、Draw()はプロテクトポーズを表示する
	float guardAnimTimer_ = 0.0f; // プロテクトポーズをループ再生するための経過時間
	float hurtTimer_ = 0.0f;      // 被弾リアクションの残り表示時間。0なら非表示

	// 「被弾」リアクションポーズを待機状態に戻すまで表示し続ける時間。
	static constexpr float kHurtDuration = 0.35f;
	// プロテクトポーズの動作を1回再生する際の1フレームあたりの秒数
	// (ループはしない - 最後のフレームまで行ったらそこで止まる)。
	static constexpr float kGuardFrameDuration = 0.1f;

	// スプライトシート。LoadDivGraphで1フレームごとのグラフィック
	// ハンドルに分割している。各フレームは128x128(data/Player/*.png参照)。
	// 敵と同じCraftPixパックのSamurai_Commanderのスプライトを使うことで、
	// 見た目上プレイヤーと敵が区別しやすいようにしている。
	static const int kIdleFrameCount = 5;
	static const int kParryFrameCount = 2;
	static const int kHurtFrameCount = 2;
	int idleHandles_[kIdleFrameCount] = {};
	int parryHandles_[kParryFrameCount] = {};
	int hurtHandles_[kHurtFrameCount] = {};
};