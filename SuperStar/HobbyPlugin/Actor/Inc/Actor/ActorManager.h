#pragma once

#include "Actor/Actor.h"
#include "Actor/ActorInterface.h"
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
        /// 解放処理
        /// これを実行するとインスタンスが残っていても利用できない
        /// タスク破棄などメモリ解放される
        /// </summary>
        void Release();

        /// <summary>
        /// 起動する
        /// 必ず最初に呼び出す
        /// グループ最大数は2以上にする
        /// TODO: 使用者が保留アクターを考慮した設定を考えるのはおかしい
        /// TODO:
        /// 内部で保留アクター用のグループを作るようにして設定側ではグループ最大数を1にしても問題ないようにする
        /// 保留のアクターを管理する専用グループを作るため
        /// </summary>
#ifdef HE_ENGINE_DEBUG
        HE::Bool Start(const HE::Uint32 in_uActorCapacity, const HE::Uint32 in_uActorGroupMax,
                       const char* in_szFileName, const HE::Uint32 in_uFileLine);
#else
        HE::Bool Start(const HE::Uint32 in_uActorCapacity, const HE::Uint32 in_uActorGroupMax);
#endif

        /// <summary>
        /// 起動終了
        /// これを読んだ後にStartメソッドを再度実行すれば使える
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
        // void Event(const Core::TaskData&);

        void ForeachActor(std::function<void(Object*)> in_func);

        /// <summary>
        /// アクター追加
        /// </summary>
        template <class T, typename... TArgs>
        Core::Common::Handle Add(const HE::Uint32 in_uGroupId, TArgs&&... in_args)
        {
            HE_STATIC_ASSERT(std::is_base_of<Object, T>::value,
                             "TクラスはアクターのObjectクラスを継承していない");

            // Actorが更新中の場合は保留グループIDに一旦登録
            // Actorは確保したメモリを使いまわさない
            HE::Uint32 uGroupId = in_uGroupId;
            if (this->_bUpdatingActors) uGroupId = this->_GetPendingGroupId();

            auto handle = this->_taskManager.CreateAndAdd<T>(uGroupId, TRUE, in_uGroupId,
                                                             std::forward<TArgs>(in_args)...);

            // 作成したActorのセットアップ
            if (this->_SetupObject(handle) == FALSE)
            {
                this->_taskManager.RemoveTask(handle);
                return NullHandle;
            }

            return handle;
        }

        /// <summary>
        /// Removes the actor.
        /// </summary>
        void Remove(Core::Common::Handle);

        /// <summary>
        /// アクター取得
        /// </summary>
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
            return this->_taskManager.GetGroupNum() - 1;
        }

        /// <summary>
        /// 最後のグループが保留グループなので更新グループ数は最大グループ数-1になる
        /// </summary>
        inline HE::Sint32 _GetUpdateGroupMax() const
        {
            return this->_taskManager.GetGroupNum() - 1;
        }

    private:
        HE::Bool _SetupObject(const Core::Common::Handle);

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
        // Core::Common::FixedMap<HE::Uint64, PendingData, 256> _mPendingActor;
    };

// Startメソッドの実行箇所を特定するマクロ
#ifdef HE_ENGINE_DEBUG
#define ACTOR_MANAGER_START(OBJ, CAPACITY, GROUP_MAX) \
    (OBJ).Start(CAPACITY, GROUP_MAX, __FILE__, __LINE__)
#else
#define ACTOR_MANAGER_START(OBJ, CAPACITY, GROUP_MAX) (OBJ).Start(CAPACITY, GROUP_MAX)
#endif
}  // namespace Actor
