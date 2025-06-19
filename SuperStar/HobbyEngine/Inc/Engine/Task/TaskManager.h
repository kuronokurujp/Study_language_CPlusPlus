#pragma once

#include <functional>

#include "Engine/Common/CustomArray.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/Core.h"
#include "Engine/Task/Task.h"

namespace Core
{
    // 前方宣言
    //class Task;
    //struct TaskData;

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
#ifdef HE_ENGINE_DEBUG
        HE::Bool Init(const HE::Uint32 in_uTaskMax, const HE::Uint32 in_uGroupNum,
                      const char* in_szFileName, const HE::Uint32 in_uFileLine);
#else
        HE::Bool Init(const HE::Uint32 in_uTaskMax, const HE::Sint32 in_iGroupNum);
#endif

        void End();

        /// <summary>
        /// 指定グループの全タスクのループ
        /// </summary>
        void ForeachByGroup(const HE::Uint32 in_sGroupId, std::function<void(Task*)>);

        /// <summary>
        /// 全タスク更新
        /// </summary>
        void UpdateAll(const HE::Float32);

        /// <summary>
        /// 指定グループを更新
        /// </summary>
        void UpdateByGroup(const HE::Uint32 in_uGroupId, const HE::Float32 in_fDt);

        /// <summary>
        /// 全タスクに流すイベント
        /// </summary>
        // void EventAll(const TaskData&);

        /// <summary>
        /// 指定グループのみイベントを流す
        /// </summary>
        // void EventByGroup(const HE::Uint32 in_uGroupId, const TaskData&);

        /// <summary>
        /// タスク作成して追加する
        /// 結果はハンドルで返す
        /// </summary>
        template <class T, typename... TArgs>
        Common::Handle CreateAndAdd(const HE::Uint32 in_uGroupId, const HE::Bool in_bReleaseMem,
                                    TArgs&&... in_args)
        {
            HE_STATIC_ASSERT(std::is_base_of<Task, T>::value,
                             "TクラスはTaskクラスを継承していない");

            HE_ASSERT(in_uGroupId < this->_uGroupNum);

            // 利用するタスクを割り当て
            auto [handle, pTask] = this->Alloc<T>(std::forward<TArgs>(in_args)...);

            // 作成したタスクのセットアップ
            this->_SetupTask(handle, in_bReleaseMem);
            // 作成したタスクのアタッチ
            this->_Attach(pTask, in_uGroupId);

            return handle;
        }

        /// <summary>
        /// タスクの削除
        /// </summary>
        void RemoveTask(Common::Handle);

        /// <summary>
        /// グループ丸ごとタスクを削除
        /// </summary>
        void RemoveGroup(const HE::Uint32);

        /// <summary>
        /// すべてのタスクを削除
        /// </summary>
        void RemoveAll();

        /// <summary>
        /// 指定グループの全タスクをターゲットグループへ移動
        /// </summary>
        HE::Bool MoveGroupAll(const HE::Uint32 in_uOrgGroupId, const HE::Uint32 in_uTargetGroupId);

        /// <summary>
        /// タスクを指定したグループ移動
        /// </summary>
        HE::Bool MoveGropuTask(const Common::Handle& in_rTask, const HE::Uint32 in_uGroupId);

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
        void EnableFlag(const HE::Uint32 in_uGroupId, const HE::Uint32 in_uFlags);

        /// <summary>
        /// 論理和を使ったフラグの消去
        /// </summary>
        void DisableFlag(const HE::Uint32 in_uGroupId, const HE::Uint32 in_uFlags);

        /// <summary>
        /// グループのフラグを取得
        /// </summary>
        HE::Uint32 Flag(const HE::Uint32) const;

        /// <summary>
        /// グループに設定しているタスクの総数
        /// </summary>
        HE::Uint32 Count(const HE::Uint32) const;

        /// <summary>
        /// タスクグループの数
        /// </summary>
        inline HE::Uint32 GetGroupNum() const { return this->_uGroupNum; }

    private:
        // タスクグループ管理
        struct TaskGroup
        {
            // グループの先頭タスクでダミーとなる
            //Task* _pRootTask;
            Task _root;
            // グループのタスク終端
            Task* _pTailTask;
            // グループのフラグ
            HE::Uint32 _uFlags;

            /// <summary>
            /// 設定しているタスク数
            /// </summary>
            HE::Uint32 _uCount;
        };

        void _SetupTask(const Core::Common::Handle in_handle, const HE::Bool in_bReleaseMem);

        /// <summary>
        /// タスク追加する
        /// </summary>
        HE::Bool _Attach(Task* in_pTask, const HE::Uint32 in_uGroupId);

        /// <summary>
        /// タスクをグループから外す
        /// グループから外しただけでタスクは消えていない
        /// GetTaskメソッドで取得は可能
        /// </summary>
        HE::Uint32 _Dettach(Task* in_pTask);

    private:
        TaskGroup* _pTasks    = NULL;
        HE::Uint32 _uGroupNum = 0;
    };

// 各メソッドを呼び出している箇所を特定するマクロ
#ifdef HE_ENGINE_DEBUG
#define TASK_MANAGER_INIT(OBJ, TASK_VAL, GROUP_VAL) \
    (OBJ).Init(TASK_VAL, GROUP_VAL, __FILE__, __LINE__)
#else
#define TASK_MANAGER_INIT(OBJ, TASK_VAL, GROUP_VAL) (OBJ).Init(TASK_VAL, GROUP_VAL)
#endif
};  // namespace Core
