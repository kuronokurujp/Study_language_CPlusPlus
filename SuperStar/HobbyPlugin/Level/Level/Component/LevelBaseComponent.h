#pragma once

#include "Level/LevelNode.h"

// エンジン最小インクルード
#include "Engine/MiniEngine.h"

// 利用モジュール
#include "ActorModule.h"

namespace Level
{
    /// <summary>
    /// レベルに設定できる基本コンポーネント
    /// レベル用コンポーネントは必ずこれを継承しないといけない
    /// </summary>
    class LevelBaseComponent : public Actor::Component
    {
        HE_CLASS_COPY_NG(LevelBaseComponent);
        HE_CLASS_MOVE_NG(LevelBaseComponent);
        HE_GENERATED_CLASS_BODY_HEADER(LevelBaseComponent, Actor::Component);

    public:
        LevelBaseComponent() : Actor::Component() {}
        virtual ~LevelBaseComponent() {}

        /// <summary>
        /// レベルにアクターを追加
        /// </summary>
        template <class T, typename... TArgs>
        Core::Common::Handle AddActor(TArgs&&... in_args)
        {
            auto pNode = reinterpret_cast<Node*>(this->_pOwner);
            HE_ASSERT(pNode);
            return pNode->AddActor<T>(std::forward<TArgs>(in_args)...);
        }

        template <class T, typename... TArgs>
        std::tuple<Core::Common::Handle, T*> AddActorByHandleAndActor(TArgs&&... in_args)
        {
            auto handle = this->AddActor<T>(std::forward<TArgs>(in_args)...);
            return std::make_tuple(handle, this->GetActor<T>(handle));
        }

        // レベルに追加されたアクターを削除
        void RemoveActor(Core::Common::Handle*);

        /// <summary>
        /// レベルのアクターを取得
        /// </summary>
        template <class T>
        T* GetActor(const Core::Common::Handle& in_rHandle)
        {
            auto pNode = reinterpret_cast<Node*>(this->_pOwner);
            HE_ASSERT(pNode);

            return pNode->GetActor<T>(in_rHandle);
        }
    };

}  // namespace Level
