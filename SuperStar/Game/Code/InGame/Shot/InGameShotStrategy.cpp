#include "InGameShotStrategy.h"

#include "Engine/Engine.h"
#include "InGame/Event/InGameEventShot.h"

// モジュール利用
#include "EventModule.h"

namespace InGame
{
    void InGameShotStrategyNormalBullet::VConfigure(ShotConfig* out)
    {
    }

    void InGameShotStrategyNormalBullet::VShot(const ShotConfig& in_rConfig)
    {
        // 弾発生イベント通知
        auto pEventModule = HE_ENGINE.ModuleManager().Get<Event::EventModule>();

        auto spEvent = HE_MAKE_CUSTOM_SHARED_PTR((EventShotNormalBullet), 0, in_rConfig.pos,
                                                 in_rConfig.dir, in_rConfig.uCollisionHashCoee);
        pEventModule->QueueEvent(std::move(spEvent), EVENT_TYPE_INGAME_SHOT);
    }

}  // namespace InGame
