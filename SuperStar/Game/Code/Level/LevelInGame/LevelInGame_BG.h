#pragma once

#include "Engine/Common/CustomVector.h"
#include "Engine/Math/Vector2.h"
#include "Engine/MiniEngine.h"

// 外部モジュール
#include "LevelModule.h"

namespace Render
{
    struct Point2D;
};

namespace Level
{
    class LevelInGame_BG : public Level::Node
    {
        HE_CLASS_COPY_NG(LevelInGame_BG);
        HE_CLASS_MOVE_NG(LevelInGame_BG);

    public:
        LevelInGame_BG() : Level::Node() {}

        Bool VBegin() override final;
        Bool VEnd() override final;

        /// <summary>
        /// 更新
        /// </summary>
        void VUpdate(const Float32 in_fDt) override final;

    private:
        Render::Point2D* _aPoint  = NULL;
        const Uint32 _uPointCount = 64;

        Core::Common::Handle _viewHandle;
    };
}  // namespace Level
