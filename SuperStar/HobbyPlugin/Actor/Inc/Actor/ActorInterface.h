#pragma once

#include "Engine/Common/Handle.h"
#include "Engine/MiniEngine.h"

namespace Actor
{
    class ActorManager;
    class Component;

    /// <summary>
    /// アクター管理を拡張するデコレーターインターフェイス
    /// </summary>
    class ActorManagerDecoraterlnterface
    {
    public:
        virtual HE::Bool VStart(ActorManager*) = 0;

        /// <summary>
        /// アクターのコンポーネントの登録・解除イベント
        /// </summary>
        virtual void VOnActorRegistComponent(Component*)   = 0;
        virtual void VOnActorUnRegistComponent(Component*) = 0;
    };

}  // namespace Actor
