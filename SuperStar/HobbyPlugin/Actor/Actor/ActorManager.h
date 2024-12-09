#pragma once

#include "Actor.h"
#include "ActorInterface.h"
#include "Component/InputComponent.h"
#include "Engine/Common/CustomMap.h"
#include "Engine/Common/Handle.h"
#include "Engine/MiniEngine.h"
#include "Engine/Task/TaskManager.h"

// 前方宣言
namespace Platform
{
    class InputInterface;
}

namespace Actor
{
    /// <summary>
    /// アクターの管理
    /// アクター登録 / 削除 / 更新などアクター個々の挙動を制御
    /// デコレーターパターンのみで拡張
    /// </summary>
    class ActorManager final
    {
    private:
        /// <summary>
        /// 更新中に作成した保留アクター情報
        /// </summary>
        struct PendingData
        {
            Core::Common::Handle handle;
            HE::Sint32 sMoveGroupId = 0;
        };

    public:
        ActorManager() = default;
        ActorManager(Core::Memory::UniquePtr<ActorManagerDecoraterlnterface>);

        /// <summary>
        /// 起動する
        /// 必ず最初に呼び出す
        /// グループ最大数は2以上にする
        /// 保留のアクターを管理する専用グループを作るため
        /// </summary>
        HE::Bool Start(const HE::Uint32 in_uActorCapacity, const HE::Uint32 in_uActorGroupMax);

        /// <summary>
        /// 終了
        /// これを呼び出した後は使えない
        /// </summary>
        HE::Bool End();

        /// <summary>
        /// 前更新
        /// </summary>
        void BeginUpdate(const HE::Float32);

        /// <summary>
        /// 更新
        /// </summary>
        void Update(const HE::Float32);

        /// <summary>
        /// 後更新
        /// </summary>
        void LateUpdate(const HE::Float32);

        /// <summary>
        /// タスクイベント
        /// </summary>
        void Event(const Core::TaskData&);

        /// <summary>
        /// アクター追加
        /// </summary>
        template <class T, typename... TArgs>
        Core::Common::Handle Add(TArgs&&... in_args)
        {
            HE_STATIC_ASSERT(std::is_base_of<Object, T>::value,
                             "TクラスはアクターのObjectクラスを継承していない");

            // Actorが更新中の場合は保留グループIDに一旦登録
            // Actorは確保したメモリを使いまわさない
            HE::Sint32 iGroupId = 0;
            if (this->_bUpdatingActors) iGroupId = this->_GetPendingGroupId();

            auto handle =
                this->_taskManager.CreateAndAdd<T>(iGroupId, TRUE, std::forward<TArgs>(in_args)...);

            Object* pObject = this->_taskManager.GetTask<Object>(handle);
            HE_ASSERT(pObject != NULL);
            pObject->SetOwner(this);

            if (this->_bUpdatingActors)
            {
                HE::Uint32 dataIdx = this->_pendingDataMap.Size();
                this->_pendingDataMap.Add(handle, PendingData{handle, 0});
            }

            return handle;
        }

        /// <summary>
        /// Removes the actor.
        /// </summary>
        void Remove(Core::Common::Handle*);

        /// <summary>
        /// アクター取得
        /// </summary>
        /// <param name=""></param>
        /// <returns></returns>
        Object* Get(const Core::Common::Handle&);

        ActorManagerDecoraterlnterface* GetDecorater() const { return this->_upDecorator.get(); }

        /// <summary>
        /// アクターのコンポーネントの登録・解除イベント
        /// </summary>
        void VOnActorRegistComponent(Component*);
        void VOnActorUnRegistComponent(Component*);

    protected:
        inline HE::Sint32 _GetPendingGroupId() const
        {
            return this->_taskManager.GetMaxGroup() - 1;
        }

        /// <summary>
        /// 最後のグループが保留グループなので更新グループ数は最大グループ数-1になる
        /// </summary>
        inline HE::Sint32 _GetUpdateGroupMax() const
        {
            return this->_taskManager.GetMaxGroup() - 1;
        }

    private:
        /// <summary>
        /// 保留アクター更新
        /// </summary>
        void _UpdatePending();

    protected:
        HE::Bool _bUpdatingActors = FALSE;

    private:
        Core::Memory::UniquePtr<ActorManagerDecoraterlnterface> _upDecorator = NULL;

        /// <summary>
        /// アクターを登録する管理
        /// </summary>
        Core::TaskManager _taskManager;

        /// <summary>
        /// 更新保留アクターのデータ
        /// </summary>
        Core::Common::FixedMap<HE::Uint64, PendingData, 256> _pendingDataMap;
    };
}  // namespace Actor
