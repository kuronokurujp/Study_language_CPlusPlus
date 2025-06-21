#pragma once

#include <functional>

#include "Task.h"

// エンジン最小インクルード
#include "Engine/MiniEngine.h"

namespace Core
{
    /// <summary>
    /// 子タスクを持たせるタスクツリーのタスク
    /// </summary>
    class TaskTree : public Task
    {
        HE_CLASS_COPY_NG(TaskTree);
        HE_CLASS_MOVE_NG(TaskTree);
        // HE_GENERATED_CLASS_BASE_BODY_HEADER(TaskTree);

    public:
        // 子タスクのリストノード
        class ChildTaskNode final : public Core::Common::LinkedListNode<ChildTaskNode>
        {
        public:
            ChildTaskNode() { this->Clear(); }
            ChildTaskNode(const Core::Common::Handle& in_rParentHandle,
                          const Core::Common::Handle& in_rHandle)
                : _parentHandle(in_rParentHandle), _handle(in_rHandle)
            {
            }

            inline const Core::Common::Handle& GetParentHandle() const
            {
                return this->_parentHandle;
            }
            inline Core::Common::Handle& GetHandle() { return this->_handle; }

            void Clear()
            {
                this->_handle.Clear();
                this->_parentHandle.Clear();
            }

            inline HE::Bool Empty() const
            {
                if (this->_handle.Null() == FALSE) return FALSE;
                if (this->_parentHandle.Null() == FALSE) return FALSE;

                return TRUE;
            }

        private:
            Core::Common::Handle _parentHandle;
            Core::Common::Handle _handle;
        };

        using ChildTaskNodeIterator =
            Core::Common::CustomList<TaskTree::ChildTaskNode>::IteratorChar;

    public:
        TaskTree();

        virtual void VSetup(const HE::Bool in_bAutoDelete);
        virtual void VUpdate(const HE::Float32) override;

        // 親タスクのハンドル
        Core::Common::Handle GetParentHandle() const { return this->_chainNode.GetParentHandle(); }

        inline HE::Bool IsChild() const { return this->_bChild; }

        /// <summary>
        /// 子タスクの追加
        /// 子タスクに追加できるのはTaskTreeかTaskTreeの派生クラスのみ
        /// Taskが追加できると管理が複雑になるため
        /// </summary>
        HE::Bool AddChildTask(const Core::Common::Handle&);

        /// <summary>
        /// 子タスクを外す
        /// </summary>
        const ChildTaskNodeIterator RemoveChildTask(Core::Common::Handle);

        /// <summary>
        /// 子タスクのループ
        /// </summary>
        void ForeachChildTask(std::function<void(TaskTree*)>);

        /// <summary>
        /// 親タスクからつながっているタスクへループ
        /// </summary>
        /// <param name=""></param>
        void ForeachParentTask(std::function<HE::Bool(TaskTree*)>);

    protected:
        /// <summary>
        /// タスク破棄
        /// </summary>
        virtual void _VDestory() override;

    private:
        TaskTree* _GetTaskTree(const Core::Common::Handle&);

        void _Clear()
        {
            this->_lstChildTask.Clear();
            this->_chainNode.Clear();
            this->_bChild = FALSE;
        }

    protected:
        Core::Common::CustomList<ChildTaskNode> _lstChildTask;
        ChildTaskNode _chainNode;

    private:
        HE::Bool _bChild = FALSE;
    };
}  // namespace Core
