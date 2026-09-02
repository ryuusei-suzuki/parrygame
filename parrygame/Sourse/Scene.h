#pragma once

// すべてのシーン(タイトル画面、ゲーム画面など)の基底クラス。
// SceneManagerがこれを介して現在のシーンのUpdate/Drawを呼び出す。
class Scene
{
public:
	virtual ~Scene() = default;

	// 毎フレーム呼ばれる更新処理。
	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;
};
