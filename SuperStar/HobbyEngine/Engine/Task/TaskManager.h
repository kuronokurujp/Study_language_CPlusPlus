#pragma once

#include <functional>

#include "Engine/Common/CustomArray.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/Core.h"

namespace Core
{
    // 前方宣言
    class Task;
    struct TaskData;

    /// <summary>
    /// タスク管理
    /// プールデータクラスを継承してタスク個数を固定にしている
    /// </summary>
    class TaskManager final : public Common::RuntimePoolManager<Task>
    {
        friend class Task;
        friend class TaskTree;

        HE_CLASS_COPY_NG(TaskManager);
        HE_CLASS_MOVE_NG(TaskManager);

    public:
        // タスクマネージャが使用するフラグ
        // 処理を停止するフラグ
        static const HE::Uint32 uFlagPause = 0x00000001;

    public:
        TaskManager() = default;
        virtual ~TaskManager();

        /// <summary>
        /// 初期化
        /// 利用前に必ず呼び出す
        /// </summary>
        HE::Bool Init(const HE::Uint32 in_uTaskMax, const HE::Sint32 in_iGroupNum);

        void End();

        /// <summary>
        /// 指定グループの全タスクのループ
        /// </summary>
        void ForeachByGroup(const HE::Sint32 in_sGroupId, std::function<void(Task*)>);

        /// <summary>
        /// 全タスク更新
        /// </summary>
        void UpdateAll(const HE::Float32);

        /// <summary>
        /// 指定グループを更新
        /// </summary>
        void UpdateByGroup(const HE::Sint32 in_sGroupId, const HE::Float32 in_fDt);

        /// <summary>
        /// 全タスクに流すイベント
        /// </summary>
        void EventAll(const TaskData&);

        /// <summary>
        /// 指定グループのみイベントを流す
        /// </summary>
        void EventByGroup(const HE::Sint32 in_sGroupId, const TaskData&);

        /// <summary>
        /// タスク作成して追加する
        /// 結果はハンドルで返す
        /// </summary>
        template <class T, typename... TArgs>
        Common::Handle CreateAndAdd(const HE::Sint32 in_sGroupId, const HE::Bool in_bReleaseMem,
                                    TArgs&&... in_args)
        {
            HE_STATIC_ASSERT(std::is_base_of<Task, T>::value,
                             "TクラスはTaskクラスを継承していない");

            HE_ASSERT(in_sGroupId < this->_iGroupNum);

            // 利用するタスクを割り当て
            auto [handle, pTask] = this->_Alloc<T>(std::forward<TArgs>(in_args)...);

            pTask->VSetup(in_bReleaseMem);
            pTask->_selfHandle = handle;

            this->_Attach(pTask, in_sGroupId);

            return handle;
        }

        /// <summary>
        /// タスクの削除
        /// </summary>
        void RemoveTask(Common::Handle*);

        /// <summary>
        /// グループ丸ごとタスクを削除
        /// </summary>
        void RemoveGroup(const HE::Sint32 in_sGroupId);

        /// <summary>
        /// すべてのタスクを削除
        /// </summary>
        void RemoveAll();

        /// <summary>
        /// 指定グループの全タスクをターゲットグループへ移動
        /// </summary>
        HE::Bool MoveGroupAll(const HE::Sint32 in_sOrgGroupId, const HE::Sint32 in_sTargetGroupId);

        /// <summary>
        /// タスクを指定したグループ移動
        /// </summary>
        HE::Bool MoveGropuTask(const Common::Handle& in_rTask, const HE::Sint32 in_sGroupId);

        /// <summary>
        /// タスクアドレスをハンドルから取得
        /// </summary>
        Task* GetTask(const Common::Handle& in_rTask);

        template <class T>
        T* GetTask(const Common::Handle& in_rTask)
        {
            HE_STATIC_ASSERT(std::is_base_of<Task, T>::value,
                             "TクラスはTaskクラスを継承していない");

            Task* pTask = this->GetTask(in_rTask);
            HE_ASSERT(pTask);

            return reinterpret_cast<T*>(pTask);
        }

        /// <summary>
        /// 論理和を使ったフラグの設定
        /// </summary>
        void EnableFlag(const HE::Sint32 in_sGroupId, const HE::Uint32 in_uFlags);

        /// <summary>
        /// 論理和を使ったフラグの消去
        /// </summary>
        void DisableFlag(const HE::Sint32 in_sGroupId, const HE::Uint32 in_uFlags);

        HE::Uint32 Flag(const HE::Sint32 in_sGroupId) const;

        /// <summary>
        /// グループに設定しているタスクの総数
        /// </summary>
        HE::Uint32 Count(const HE::Sint32 in_sGroupId) const;

        /// <summary>
        /// タスクグループの最大数
        /// </summary>
        inline HE::Sint32 GetMaxGroup() const { return this->_iGroupNum; }

    private:
        // タスクグループ管理
        struct TaskGroup
        {
            // グループの先頭タスクでダミーとなる
            Task* _pRootTask;
            // グループのタスク終端
            Task* _pTailTask;
            // グループのフラグ
            HE::Uint32 _uFlags;

            /// <summary>
            /// 設定しているタスク数
            /// </summary>
            HE::Uint32 _uCount;
        };

        /// <summary>
        /// タスク追加する
        /// </summary>
        HE::Bool _Attach(Task* in_pTask, const HE::Sint32 in_sGroupId);

        /// <summary>
        /// タスクをグループから外す
        /// </summary>
        HE::Sint32 _Dettach(Task* in_pTask);

    private:
        TaskGroup* _pTasks    = NULL;
        HE::Uint32 _iGroupNum = 0;
    };
};  // namespace Core
