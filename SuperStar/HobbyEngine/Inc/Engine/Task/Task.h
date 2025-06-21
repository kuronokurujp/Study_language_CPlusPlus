#pragma once

#include "Engine/Common/CustomList.h"
#include "Engine/Common/Handle.h"
#include "Engine/Core.h"
#include "Engine/Generated.h"

namespace Core
{
    // 前方宣言
    class TaskManager;

    /// <summary>
    /// オブジェクトのタスクを作る時の継承用のクラス
    /// </summary>
    class Task
    {
        // タスクマネージャには非公開メンバを操作させる
        friend class TaskManager;
        friend class TaskTree;

        HE_CLASS_COPY_NG(Task);
        HE_CLASS_MOVE_NG(Task);

    public:
        static const HE::Uint32 uNoneId      = 0;
        static const HE::Uint32 uNoneGroupId = 0xffffffff;

        /// <summary>
        /// タスクのタイプ
        /// タスクの派生クラスが出来たら追加してGetType()で取得できるようにする
        /// </summary>
        enum class EType
        {
            EType_Base = 0,
            EType_Tree
        };

        // タスクは生成したインスタンスが再利用されるのでコンストラクタで初期化はできない
        Task();
        virtual ~Task() = default;

        /// <summary>
        /// タスク実行前の初期セットアップ
        /// </summary>
        virtual void VSetup(const HE::Bool in_bAutoDelete);

        /// <summary>
        /// タスク開始
        /// </summary>
        virtual HE::Bool VBegin() { return TRUE; }

        /// <summary>
        /// タスク終了
        /// </summary>
        virtual HE::Bool VEnd() { return TRUE; }

        /// <summary>
        /// 更新
        /// </summary>
        virtual void VUpdate(const HE::Float32) {}

        /// <summary>
        /// 破棄指示
        /// 呼び出してもすぐには破棄されない
        /// </summary>
        void Kill();

        /// <summary>
        /// タスクのオブジェクトハンドル
        /// </summary>
        inline const Common::Handle& Handle() const { return this->_handle; }

        /// <summary>
        /// グループID
        /// </summary>
        inline HE::Sint32 GetGropuId() const { return this->_uGroupId; }

        /// <summary>
        /// タスクのタイプ
        /// </summary>
        inline EType GetType() const { return this->_eType; }

    protected:
        /// <summary>
        /// タスク破棄
        /// </summary>
        virtual void _VDestory() {}

    private:
        void _Clear()
        {
            this->_bStart = FALSE;
            this->_bKill  = FALSE;

            this->_uGroupId    = Task::uNoneGroupId;
            this->_pPrev       = NULL;
            this->_pNext       = NULL;
            this->_bReleaseMem = FALSE;
            this->_handle.Clear();
            this->_pTaskManager = NULL;
            this->_eType        = EType::EType_Base;
        }

    protected:
        TaskManager* _pTaskManager = NULL;

        EType _eType = EType::EType_Base;

    private:
        HE::Uint32 _uGroupId = Task::uNoneGroupId;
        HE::Bool _bStart     = FALSE;
        HE::Bool _bKill      = FALSE;
        Common::Handle _handle;

        // タスクを解放した時にメモリからも解放するか
        // 解放しない場合はキャッシュして使いまわすことになる
        HE::Bool _bReleaseMem = FALSE;

        Task* _pPrev = NULL;
        Task* _pNext = NULL;
    };

};  // namespace Core
