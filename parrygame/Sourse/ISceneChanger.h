#pragma once
#include <memory>

class Scene; // 前方宣言

/// @brief シーン切り替えの窓口となるインターフェース
/// SceneManagerがこれを継承して実際の切り替え処理を実装する
class ISceneChanger
{
public:
    virtual ~ISceneChanger() = default;

    /// @brief シーンを切り替える
    /// @param next 次のシーン
    virtual void ChangeScene(std::unique_ptr<Scene> next) = 0;
};
