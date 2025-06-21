#include "Engine/Task/TaskTree.h"

#include "Engine/Task/TaskManager.h"

namespace Core
{
    void TaskTree::_VDestory()
    {
        // 子タスクを全て破棄
        while (this->_lstChildTask.BeginItr() != this->_lstChildTask.EndItr())
        {
            auto it = this->_lstChildTask.BeginItr();

            auto pTask = this->_pTaskManager->GetTask(it->GetHandle());

            // 親から外すのと同時にタスク破棄
            pTask->Kill();
            this->RemoveChildTask(it->GetHandle());
        }
    }

    HE::Bool TaskTree::AddChildTask(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT_RETURN_VALUE(FALSE, in_rHandle.Null() == FALSE);
        // 自分自身を子タスクとして設定できない
        if (in_rHandle == this->_handle) return FALSE;

        auto pTaskTree = this->_GetTaskTree(in_rHandle);
        HE_ASSERT_RETURN_VALUE(FALSE, pTaskTree != NULL);

        // グループIDが違うタスクを子タスクにできないようにする
        HE_ASSERT_RETURN_VALUE(FALSE, pTaskTree->GetGropuId() == this->GetGropuId());

        // 管理側でつながっているのであれば外す
        if (pTaskTree->_bChild == FALSE)
        {
            // タスクが子タスクを持っていると対応しないようにしている
            if (pTaskTree->_lstChildTask.Empty() == FALSE)
            {
                HE_ASSERT(FALSE && "タスクに子タスクがあるのこのタスクは子タスクにはできない");
                return FALSE;
            }

            this->_pTaskManager->_Dettach(pTaskTree);
            // 子のタスクに変わった
            pTaskTree->_bChild = TRUE;
        }
        else
        {
            // すでに子タスクとなっているのであれば親タスクから外す
            auto pParentTaskTree = this->_GetTaskTree(pTaskTree->_chainNode.GetParentHandle());
            pParentTaskTree->RemoveChildTask(pTaskTree->_chainNode.GetHandle());
        }

        // 子のタスク情報を作成してリストに追加
        pTaskTree->_chainNode = ChildTaskNode(this->_handle, in_rHandle);

        // つける対象のタスクハンドルとつけるタスクのハンドルをリストに追加
        this->_lstChildTask.PushBack(pTaskTree->_chainNode);

        return TRUE;
    }

    const TaskTree::ChildTaskNodeIterator TaskTree::RemoveChildTask(Core::Common::Handle in_handle)
    {
        HE_ASSERT(in_handle.Null() == FALSE);

        auto pTaskTree = this->_GetTaskTree(in_handle);
        if (pTaskTree->_chainNode.Empty()) return ChildTaskNodeIterator();

        HE_ASSERT(pTaskTree->_chainNode.GetParentHandle() == this->_handle);

        // 子のタスクではなくなった
        pTaskTree->_bChild = FALSE;
        // 親のリストから子のタスクを外す
        auto nextItr = this->_lstChildTask.Erase(&pTaskTree->_chainNode);

        // 破棄フラグがあればタスク管理で消去
        if (pTaskTree->_bKill)
        {
            this->_pTaskManager->RemoveTask(in_handle);
        }
        else
        {
            // タスク管理につけなおす
            this->_pTaskManager->_Attach(pTaskTree, pTaskTree->GetGropuId());
        }

        return nextItr;
    }

    TaskTree::TaskTree() : Task()
    {
        this->_Clear();
        this->_eType = EType::EType_Tree;
    }

    void TaskTree::VSetup(const HE::Bool in_bAutoDelete)
    {
        Task::VSetup(in_bAutoDelete);
        this->_Clear();
    }

    void TaskTree::VUpdate(const HE::Float32 in_fDt)
    {
        // 子タスクの実行
        this->ForeachChildTask(
            [this, in_fDt](Task* in_pTask)
            {
                // 子タスクがまだ開始していない可能性がある
                if (in_pTask->_bStart == FALSE)
                {
                    if (in_pTask->VBegin())
                    {
                        in_pTask->_bStart = TRUE;
                    }
                }

                in_pTask->VUpdate(in_fDt);
            });
    }

    TaskTree* TaskTree::_GetTaskTree(const Core::Common::Handle& in_rHandle)
    {
        auto pTask = this->_pTaskManager->GetTask(in_rHandle);
        HE_ASSERT_RETURN_VALUE(NULL, pTask);
        HE_ASSERT_RETURN_VALUE(NULL, pTask->GetType() == Task::EType::EType_Tree);

        auto pTaskTree = reinterpret_cast<TaskTree*>(pTask);
        return pTaskTree;
    }

    void TaskTree::ForeachChildTask(std::function<void(TaskTree*)> in_action)
    {
        for (auto it = this->_lstChildTask.BeginItr(); it != this->_lstChildTask.EndItr(); ++it)
        {
            auto pTask = this->_pTaskManager->GetTask(it->GetHandle());
            if (pTask == NULL)
            {
                this->_lstChildTask.Erase(it);
                continue;
            }

            HE_ASSERT_RETURN(pTask->GetType() == Task::EType::EType_Tree);
            in_action(reinterpret_cast<TaskTree*>(pTask));
        }
    }

    void TaskTree::ForeachParentTask(std::function<HE::Bool(TaskTree*)> in_action)
    {
        Core::Common::Handle handle = this->GetParentHandle();
        while (handle.Null() == FALSE)
        {
            auto pTaskTree = this->_GetTaskTree(handle);
            if (pTaskTree == NULL) break;

            HE_ASSERT_RETURN(pTaskTree->GetType() == Task::EType::EType_Tree);
            if (in_action(reinterpret_cast<TaskTree*>(pTaskTree)) == FALSE) break;

            if (pTaskTree->IsChild() == FALSE) break;

            handle = pTaskTree->GetParentHandle();
        }
    }
}  // namespace Core
