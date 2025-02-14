#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/Handle.h"
#include "Engine/Common/Hash.h"
#include "Engine/MiniEngine.h"

// ゲーム全体で参照できるグローバルデータを置く
namespace Game
{
    // シーン描画するUI/2Dのハンドル
    extern Core::Common::Handle g_sceneUIHandle;
    extern Core::Common::Handle g_scene2DHandle;

    using GameAssetMap =
        Core::Common::FixedMap<Core::Common::FixedString128, Core::Common::Handle, 64>;
}  // namespace Game
