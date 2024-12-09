#pragma once

#include "Engine/MiniEngine.h"

// 外部モジュール
#include "LevelModule.h"

namespace Level
{
    /// <summary>
    /// デバッグメインレベル
    /// </summary>
    class LevelLauncher final : public Level::Node
    {
        HE_CLASS_COPY_NG(LevelLauncher);
        HE_CLASS_MOVE_NG(LevelLauncher);

    public:
        LevelLauncher() = default;

        HE::Bool VBegin() override final;
        HE::Bool VEnd() override final;

    private:
        Core::Common::Handle _layoutAssetHandle;
    };
}  // namespace Level
