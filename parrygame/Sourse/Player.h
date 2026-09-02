#pragma once

// プレイヤー側のキャラクター。純粋に見た目だけの存在で、画面左側に
// 立って敵と向き合う。パリィが実際に成功した瞬間だけガード/パリィの
// ポーズを一瞬見せ(フラッシュ的な演出)、パリィに失敗すると
// 「被弾」リアクションを再生する。それ以外は待機(Idle)しているだけ。
class Player
{
public:
	Player();
	~Player();
	void Update(float deltaTime);
	void Draw();

	// パリィが成功したときに呼ぶ。プレイヤーが一瞬だけガード/パリィの
	// ポーズを見せてから待機状態に戻る。
	void PlayParrySuccess();

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

	float idleTimer_ = 0.0f;    // ループする待機アニメーションの経過時間
	float successTimer_ = 0.0f; // パリィ成功ポーズの残り表示時間。0なら非表示
	float hurtTimer_ = 0.0f;    // 被弾リアクションの残り表示時間。0なら非表示

	// 「被弾」リアクションポーズを待機状態に戻すまで表示し続ける時間。
	static constexpr float kHurtDuration = 0.35f;
	// パリィ成功ポーズを待機状態に戻すまで表示し続ける時間。
	static constexpr float kParrySuccessDuration = 0.25f;

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
