#pragma once

#include "Common.h"

// エンジン
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

        HE::Bool VBegin() override final;
        HE::Bool VEnd() override final;

        /// <summary>
        /// 更新
        /// </summary>
        void VUpdate(const HE::Float32 in_fDt) override final;

        void VProcessInput(const EnhancedInput::InputMap&) override final;

    private:
        Core::Common::Handle _systemComponentHandle;
        Core::Common::Handle _stageManagerComponentHandle;

        Core::Memory::SharedPtr<Game::GameAssetMap> _spGameAsset;
    };
}  // namespace Level
