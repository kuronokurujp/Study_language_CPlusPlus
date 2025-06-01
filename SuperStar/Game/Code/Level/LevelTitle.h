#pragma once

#include "Engine/MiniEngine.h"

// 外部モジュール
#include "LevelModule.h"

namespace Level
{
    class LevelTitle final : public Level::Node
    {
        HE_CLASS_COPY_NG(LevelTitle);
        HE_CLASS_MOVE_NG(LevelTitle);

    public:
        LevelTitle() = default;

        HE::Bool VBegin() override final;
        HE::Bool VEnd() override final;

        void VProcessInput(const EnhancedInput::InputMap&) override final;

    private:
        Core::Common::Handle _layoutAssetHandle;
        Core::Common::Handle _widget;
    };
}  // namespace Level
