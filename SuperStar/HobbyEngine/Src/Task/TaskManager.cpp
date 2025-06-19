#include "Engine/Task/TaskManager.h"

#include "Engine/Memory/Memory.h"
#include "Engine/Task/Task.h"

namespace Core
{
    TaskManager::~TaskManager()
    {
        this->End();
    }

#ifdef HE_ENGINE_DEBUG
    HE::Bool TaskManager::Init(const HE::Uint32 in_uTaskMax, const HE::Uint32 in_uGroupNum,
                               const char* in_szFileName, const HE::Uint32 in_uFileLine)
#else
    HE::Bool TaskManager::Init(const HE::Uint32 in_uTaskMax, const HE::Uint32 in_iGroupNum)
#endif
    {
        // プールのバッファ予約
#ifdef HE_ENGINE_DEBUG
        this->ReservePool(in_uTaskMax, in_szFileName, in_uFileLine);
#else
        this->ReservePool(in_uTaskMax);
#endif

        // タスクグループを確保
        this->_pTasks =
            reinterpret_cast<TaskGroup*>(HE_ALLOC_MEM(sizeof(TaskGroup) * in_uGroupNum, 0));

        for (HE::Uint32 i = 0; i < in_uGroupNum; ++i)
        {
            this->_pTasks[i]._root._Clear();

            // ダミータスクを終端タスクとして設定
            this->_pTasks[i]._pTailTask = &this->_pTasks[i]._root;
            this->_pTasks[i]._uFlags    = 0;
            this->_pTasks[i]._uCount    = 0;
        }
        HE_ASSERT_RETURN_VALUE(FALSE, this->_pTasks && "タスクグループのメモリ確保に失敗");

        this->_uGroupNum = in_uGroupNum;

        return TRUE;
    }

    void TaskManager::End()
    {
        if (this->_pTasks == NULL) return;

        // タスクを全て削除
        this->RemoveAll();

        // グループを削除
        HE_SAFE_DELETE_MEM(this->_pTasks);

        this->ReleasePool();
    }

    void TaskManager::ForeachByGroup(const HE::Uint32 in_uGroupId,
                                     std::function<void(Task*)> in_func)
    {
        HE_ASSERT(in_uGroupId < this->_uGroupNum);

        // 登録タスクを全更新
        TaskGroup* pGroup = &this->_pTasks[in_uGroupId];
        Task* pTask       = &pGroup->_root;

        // グループにポーズフラグが付いてるなら何もしない
        if ((pGroup->_uFlags & uFlagPause) == FALSE)
        {
            while (pTask->_pNext)
            {
                pTask = pTask->_pNext;

                // 死亡フラグが付いてるのは更新しない
                if (pTask->_bKill) continue;
                if (pTask->_bStart == FALSE) continue;

                in_func(pTask);
            }
        }
    }

    void TaskManager::UpdateAll(const HE::Float32 in_fDt)
    {
        for (HE::Sint32 i = 0; i < this->_uGroupNum; ++i) this->UpdateByGroup(i, in_fDt);
    }

    void TaskManager::UpdateByGroup(const HE::Uint32 in_uGroupId, const HE::Float32 in_fDt)
    {
        HE_ASSERT(in_uGroupId < this->_uGroupNum);

        // 登録タスクを全更新
        TaskGroup* pGroup = &this->_pTasks[in_uGroupId];
        Task* pTask       = &pGroup->_root;

        // グループにポーズフラグが付いてるなら何もしない
        if ((pGroup->_uFlags & uFlagPause) == FALSE)
        {
            while (pTask->_pNext)
            {
                pTask = pTask->_pNext;

                // 死亡フラグが付いてるのは更新しない
                if (pTask->_bKill) continue;

                // タスク実行開始
                // 一度しか呼ばれない
                if (pTask->_bStart == FALSE)
                {
                    if (pTask->VBegin())
                    {
                        pTask->_bStart = TRUE;
                    }
                }

                // タスク更新
                pTask->VUpdate(in_fDt);
            }
        }

        // 更新が終わったタイミングで死亡フラグが付いたタスクを殺す
        pTask = pGroup->_root._pNext;
        while (pTask)
        {
            // 次のタスクポインタを先に取得しておく
            Task* pNextTask = pTask->_pNext;

            // 死亡フラグが付いてたら消す
            if (pTask->_bKill)
            {
                this->RemoveTask(pTask->_handle);
            }

            // 逃がしておいた次のポインタを得る
            pTask = pNextTask;
        }
    }

    /*
        void TaskManager::EventAll(const TaskData& in_rTaskData)
        {
            for (HE::Uint32 i = 0; i < this->_uGroupNum; ++i) this->EventByGroup(i, in_rTaskData);
        }
    */

    /*
        void TaskManager::EventByGroup(const HE::Uint32 in_uGroupId, const TaskData& in_rTaskData)
        {
            HE_ASSERT(in_uGroupId < this->_uGroupNum);

            TaskGroup* pGroup = &this->_pTasks[in_uGroupId];
            Task* pTask       = pGroup->_pRootTask;

            // グループにポーズフラグが付いてるなら何もしない
            if ((pGroup->_uFlags & uFlagPause) == FALSE)
            {
                while (pTask->_pNext)
                {
                    pTask = pTask->_pNext;

                    if (pTask->_bKill) continue;
                    if (pTask->_bStart == FALSE) continue;

                    pTask->VEvent(in_rTaskData);
                }
            }
        }
        */

    void TaskManager::RemoveTask(Common::Handle in_handle)
    {
        // タスクのインスタンスを取得
        // 既に削除済みのハンドルだったらNULLが返される
        Task* pTask = this->GetTask(in_handle);
        if (pTask == NULL) return;

        // 終了を呼ぶ
        if (pTask->_bStart) pTask->VEnd();

        // タスクを破棄
        pTask->_VDestory();

        // タスクの連結を解除
        this->_Dettach(pTask);

        pTask->_pTaskManager = NULL;

        // ハンドルを返却する + メモリから削除するかしないか
        const HE::Bool bCache = pTask->_bReleaseMem == FALSE;
        this->Free(in_handle, bCache);
    }

    void TaskManager::RemoveGroup(HE::Uint32 in_uGroupId)
    {
        HE_ASSERT(in_uGroupId < this->_uGroupNum);

        // 登録されているタスクを全て更新
        TaskGroup* pGroup = &this->_pTasks[in_uGroupId];
        Task* pTask       = &pGroup->_root;
        pTask             = pTask->_pNext;

        while (pTask)
        {
            // RemoveでタスクのpPrev/pNextの入れ替えが起きてしまうので,
            // 次に実行するタスクを事前に取得しておく
            Task* pNextTask = pTask->_pNext;
            this->RemoveTask(pTask->_handle);

            // 逃がしておいた次のポインタを得る
            pTask = pNextTask;
        }
    }

    void TaskManager::RemoveAll()
    {
        if (this->_pTasks == NULL) return;

        // タスクを全て削除
        for (HE::Sint32 i = 0; i < this->_uGroupNum; ++i) this->RemoveGroup(i);
    }

    HE::Bool TaskManager::MoveGroupAll(const HE::Uint32 in_uOrgGroupId,
                                       const HE::Uint32 in_uTargetGroupId)
    {
        HE_ASSERT(in_uOrgGroupId < this->_uGroupNum && "グループ元のグループID値が間違っている");
        HE_ASSERT(in_uTargetGroupId < this->_uGroupNum &&
                  "グループ移動させるグループID値が間違っている");

        // 付け替え元と付け替え先のグループIDが同じなので何もしない
        if (in_uOrgGroupId == in_uTargetGroupId) return FALSE;

        // 登録されているグループタスクを全て更新
        TaskGroup* pGroup = &this->_pTasks[in_uOrgGroupId];
        Task* pTask       = &pGroup->_root;
        pTask             = pTask->_pNext;
        while (pTask)
        {
            // RemoveでタスクのpPrev/pNextの入れ替えが起きてしまうので,
            // 次に実行するタスクを事前に取得しておく
            Task* pNextTask = pTask->_pNext;
            {
                // 一旦グループから削除する
                this->_Dettach(pTask);

                // 別のグループに付け替える
                this->_Attach(pTask, in_uTargetGroupId);
            }

            // 逃がしておいた次のポインタを得る
            pTask = pNextTask;
        }

        return TRUE;
    }

    HE::Bool TaskManager::MoveGropuTask(const Common::Handle& in_hTask,
                                        const HE::Uint32 in_uGroupId)
    {
        HE_ASSERT(in_hTask.Null() == FALSE);

        Task* pTask = this->GetTask(in_hTask);
        HE_ASSERT(pTask);

        if (this->_Dettach(pTask) == Task::uNoneGroupId) return FALSE;
        if (this->_Attach(pTask, in_uGroupId) == FALSE) return FALSE;

        return TRUE;
    }

    Task* TaskManager::GetTask(const Common::Handle& in_hTask)
    {
        if (in_hTask.Null()) return NULL;

        // ハンドルからタスクインスタンスを引き出す
        Task* pTask = this->Ref(in_hTask);
        if (pTask == NULL) return NULL;

        return pTask;
    }

    void TaskManager::EnableFlag(const HE::Uint32 in_uGroupId, const HE::Uint32 in_uFlags)
    {
        HE_ASSERT(in_uGroupId < this->_uGroupNum);

        TaskGroup* pGroup = &this->_pTasks[in_uGroupId];
        pGroup->_uFlags |= in_uFlags;
    }

    void TaskManager::DisableFlag(const HE::Uint32 in_uGroupId, const HE::Uint32 in_uFlags)
    {
        HE_ASSERT(in_uGroupId < this->_uGroupNum);
        TaskGroup* pGroup = &this->_pTasks[in_uGroupId];
        pGroup->_uFlags &= ~in_uFlags;
    }

    HE::Uint32 TaskManager::Flag(const HE::Uint32 in_uGroupId) const
    {
        HE_ASSERT(in_uGroupId < this->_uGroupNum);
        TaskGroup* pGroup = &this->_pTasks[in_uGroupId];
        return pGroup->_uFlags;
    }

    HE::Uint32 TaskManager::Count(const HE::Uint32 in_uGroupId) const
    {
        HE_ASSERT(in_uGroupId < this->_uGroupNum);
        TaskGroup* pGroup = &this->_pTasks[in_uGroupId];
        return pGroup->_uCount;
    }

    void TaskManager::_SetupTask(const Core::Common::Handle in_handle,
                                 const HE::Bool in_bReleaseMem)
    {
        auto pTask = this->GetTask(in_handle);

        pTask->VSetup(in_bReleaseMem);
        pTask->_handle = in_handle;
    }

    /// <summary>
    /// タスク追加する
    /// </summary>
    HE::Bool TaskManager::_Attach(Task* in_pTask, const HE::Uint32 in_uGroupId)
    {
        HE_ASSERT(in_uGroupId < this->_uGroupNum);
        HE_ASSERT((in_pTask != NULL) && "タスクに割り当ている数がいっぱいで割り当てに失敗");

        // アタッチ
        Task* pTask      = in_pTask;
        pTask->_uGroupId = in_uGroupId;

        // 終端タスクの後ろに登録
        Task* pTailTask   = this->_pTasks[in_uGroupId]._pTailTask;
        pTailTask->_pNext = in_pTask;

        // タスクの初期化
        pTask->_pPrev        = pTailTask;
        pTask->_pNext        = NULL;
        pTask->_uGroupId     = in_uGroupId;
        pTask->_bStart       = FALSE;
        pTask->_bKill        = FALSE;
        pTask->_pTaskManager = this;

        // 追加したタスクを終端タスクとする
        this->_pTasks[in_uGroupId]._pTailTask = in_pTask;

        ++this->_pTasks[in_uGroupId]._uCount;

        return TRUE;
    }

    /// <summary>
    /// タスクをグループから外す
    /// </summary>
    HE::Uint32 TaskManager::_Dettach(Task* in_pTask)
    {
        HE::Uint32 uGroupId = in_pTask->_uGroupId;
        HE_ASSERT_RETURN_VALUE(Task::uNoneGroupId, uGroupId != Task::uNoneGroupId);

        // タスクの前後付け替え
        Task* pPrevTask = in_pTask->_pPrev;
        Task* pNextTask = in_pTask->_pNext;

        // タスクの前後連結情報がない場合はタスクのつけ外しはできないので終了
        if ((pPrevTask == NULL) && (pNextTask == NULL)) return Task::uNoneGroupId;

        if (pPrevTask != NULL) pPrevTask->_pNext = pNextTask;
        if (pNextTask != NULL) pNextTask->_pPrev = pPrevTask;

        // 今回削除するのが終端タスクならつなぎなおしをする
        {
            TaskGroup* pGroup = &this->_pTasks[uGroupId];
            if (pGroup->_pTailTask == in_pTask) pGroup->_pTailTask = pPrevTask;
        }

        --this->_pTasks[uGroupId]._uCount;

        return uGroupId;
    }
}  // namespace Core
