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

    protected:
        void _VProcessInput(const EnhancedInput::InputMap* in_pInputMap) override final;

    private:
        Core::Common::Handle _layoutAssetHandle;
    };
}  // namespace Level
