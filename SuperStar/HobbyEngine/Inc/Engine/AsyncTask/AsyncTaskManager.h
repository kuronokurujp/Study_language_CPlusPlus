#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "Engine/Common/CustomVector.h"
#include "Engine/Core.h"
#include "Engine/Memory/Memory.h"

namespace Core
{
    // 前方宣言
    class AsyncTask;

    /// <summary>
    /// 非同期タスク管理
    /// </summary>
    class AsyncTaskManager final
    {
        HE_CLASS_DEFAULT_CONSTRUCT_NG(AsyncTaskManager);
        HE_CLASS_COPY_NG(AsyncTaskManager);
        HE_CLASS_MOVE_NG(AsyncTaskManager);

    public:
        AsyncTaskManager(const HE::Uint32 in_uMemPage = 0);
        ~AsyncTaskManager();

        /// <summary>
        /// 初期化
        /// 利用前に必ず呼び出す
        /// </summary>
        HE::Bool Init(const HE::Uint32 in_uThreadNum);

        /// <summary>
        /// 終了時に呼び出す
        /// </summary>
        void End();

        /// <summary>
        /// 非同期タスクを設定
        /// </summary>
        void Push(Memory::SharedPtr<AsyncTask>);

    private:
        Memory::SharedPtr<AsyncTask> _PopWithThread();

        /// <summary>
        /// スレッド呼び出し用
        /// </summary>
        void _WorkerThread();

    private:
        std::atomic<HE::Bool> _bEnd{FALSE};
        std::condition_variable _cv;
        std::mutex _mutex;

        // メモリ確保に利用するページ
        HE::Uint32 _uMemPage = 0;

        // 用意するスレッド数
        HE::Uint32 _uThreadNum = 0;

        // スレッド配列
        class std::thread** _apThread = NULL;

        // 非同期タスクのキュー
        // タスクに数は固定にした
        Common::FixedVector<Memory::SharedPtr<AsyncTask>, 256> _vTaskQueue;
    };
}  // namespace Core
