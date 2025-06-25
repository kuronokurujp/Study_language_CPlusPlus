#pragma once

// エンジン最小インクルード
#include "Engine/MiniEngine.h"

// 利用モジュール
#include "ActorModule.h"
#include "EnhancedInputModule.h"

namespace Level
{
    /// <summary>
    /// レベルのノード
    /// アクターなどのオブジェクトを配置
    /// </summary>
    class Node : public Actor::Object
    {
        HE_CLASS_COPY_NG(Node);
        HE_CLASS_MOVE_NG(Node);

    public:
        Node();
        virtual ~Node() = default;

        /// <summary>
        /// タスク開始
        /// </summary>
        HE::Bool VBegin() override;

        /// <summary>
        /// タスク終了
        /// </summary>
        HE::Bool VEnd() override;

        /// <summar>
        /// 更新
        /// </summary>
        void VBeginUpdate(const HE::Float32 in_fDt) override final;

        /// <summary>
        /// 更新
        /// </summary>
        void VUpdate(const HE::Float32 in_fDt) override;

        /// <summar>
        /// 更新
        /// </summary>
        void VLateUpdate(const HE::Float32 in_fDt) override final;

        virtual void VProcessInput(const EnhancedInput::InputMap&);

        /// <summary>
        /// レベルにアクターを追加
        /// </summary>
        template <class T, typename... TArgs>
        Core::Common::Handle AddActor(TArgs&&... in_args)
        {
            HE_STATIC_ASSERT(std::is_base_of<Actor::Object, T>::value,
                             "TクラスはアクターのObjectクラスを継承していない");

            Core::Common::Handle handle =
                this->_actorManager.Add<T>(std::forward<TArgs>(in_args)...);
            HE_ASSERT(handle.Null() == FALSE);

            return handle;
        }

        // レベルに追加されたアクターを削除
        void RemoveActor(const Core::Common::Handle);

        /// <summary>
        /// レベルを追加
        /// </summary>
        template <class T>
        const Core::Common::Handle AddLevel()
        {
            HE_STATIC_ASSERT(std::is_base_of<Node, T>::value,
                             "Tクラスはレベルのノードクラスを継承していない");

            // ノード管理にレベルを追加
            auto pNodeManager = this->_pOwner;
            HE_ASSERT(pNodeManager);
            if (pNodeManager == NULL) return NullHandle;

            Core::Common::Handle handle = pNodeManager->Add<T>();
            if (handle.Null()) return NullHandle;

            return handle;
        }

        /// <summary>
        /// レベルのアクターを取得
        /// </summary>
        template <class T>
        T* GetActor(const Core::Common::Handle& in_rHandle)
        {
            HE_STATIC_ASSERT(std::is_base_of<Actor::Object, T>::value,
                             "Tクラスはアクターのクラスを継承していない");

            HE_ASSERT(in_rHandle.Null() == FALSE);
            return reinterpret_cast<T*>(this->_actorManager.Get(in_rHandle));
        }

        /// <summary>
        /// アクター同士の関連付け設定
        /// </summary>
        HE::Bool ChainActor(const Core::Common::Handle& in_rActor,
                            const Core::Common::Handle& in_rParentActor);

    protected:
        /// <summary>
        /// タスク破棄
        /// </summary>
        void _VDestory() override final;

    private:
        /// <summary>
        /// レベルに紐づけるアクター管理
        /// </summary>
        Actor::ActorManager _actorManager;
    };
}  // namespace Level
