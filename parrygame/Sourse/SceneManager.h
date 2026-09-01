#pragma once
#include "Scene.h"
#include "ISceneChanger.h"
#include <memory>
class SceneManager : public ISceneChanger
{
public:
    /// @brief シーンを切り替える
    /// @param next 次のシーン
    void ChangeScene(std::unique_ptr<Scene> next) override;

    /// @brief 現在のシーンのUpdateを呼ぶ 毎フレーム呼ぶこと
    void Update(float deltaTime);

    /// @brief 現在のシーンのDrawを呼ぶ 毎フレーム呼ぶこと
    void Draw();

private:
    std::unique_ptr<Scene> currentScene_; ///< 現在のシーン
};
