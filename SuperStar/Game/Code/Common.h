#pragma once

#include "Engine/Common/Handle.h"
#include "Engine/Common/Hash.h"
#include "Engine/MiniEngine.h"

// ゲーム全体で参照できるグローバルデータを置く
namespace Game
{
    // シーン描画するUI/2Dのハンドル
    extern Core::Common::Handle g_sceneUIHandle;
    extern Core::Common::Handle g_scene2DHandle;

}  // namespace Game
