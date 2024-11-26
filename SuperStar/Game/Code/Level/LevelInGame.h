#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/MiniEngine.h"

// 外部モジュール
#include "LevelModule.h"

namespace Level
{
    class LevelInGame final : public Level::Node
    {
        HE_CLASS_COPY_NG(LevelInGame);
        HE_CLASS_MOVE_NG(LevelInGame);

    public:
        LevelInGame() = default;

        Bool VBegin() override final;
        Bool VEnd() override final;

        /// <summary>
        /// 更新
        /// </summary>
        void VUpdate(const Float32 in_fDt) override final;

    protected:
        void _VProcessInput(const EnhancedInput::InputMap* in_pInputMap) override final;

    private:
        Core::Common::Handle _systemComponentHandle;
        Core::Common::Handle _stageManagerComponentHandle;
        Core::Common::Handle _viewHandle;

        Core::Common::FixedMap<Core::Common::FixedString128, Core::Common::Handle, 64> _mGameAsset;
    };
}  // namespace Level
