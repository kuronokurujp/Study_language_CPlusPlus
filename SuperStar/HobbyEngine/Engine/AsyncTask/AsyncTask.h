#pragma once

#include <atomic>
#include <future>

#include "Engine/Core.h"

namespace Core
{
    /// <summary>
    /// 非同期タスク
    /// </summary>
    class AsyncTask
    {
        // 管理クラスからは自由にアクセスできるようにする
        friend class AsyncTaskManager;

        HE_CLASS_COPY_NG(AsyncTask);
        HE_CLASS_MOVE_NG(AsyncTask);

    public:
        AsyncTask();

        /// <summary>
        /// 非同期処理が終わるまでメインスレッド待機するので注意
        /// </summary>
        void Wait();

        /// <summary>
        /// 非同期処理が終わるまで一定時間メインスレッド待機する
        /// </summary>
        /// <returns>TRUE: タスク継続 / FALSE: タスク終了</returns>
        Bool Contiune(const Uint32 in_uMsSec);

        /// <summary>
        /// タスクが終わっているか
        /// </summary>
        inline Bool IsEnd() const { return this->_bEnd; }

        /// <summary>
        /// タスクが成功しているか
        /// </summary>
        Bool IsSuccess() const;

    protected:
        virtual Bool _VBeginWithThread();
        virtual Bool _VUpdateWithThread() = 0;
        virtual Bool _VEndWithThread();

    protected:
        Bool _bSuccess = FALSE;

    private:
        std::atomic<Bool> _bEnd{FALSE};
        std::promise<Bool> _promise;
        std::future<Bool> _future;
    };

    /// <summary>
    /// 非同期処理した結果を返すタスク
    /// </summary>
    template <typename TResult>
    class AsyncTaskResult : public AsyncTask
    {
    public:
        /// <summary>
        /// タスク結果を返す
        /// </summary>
        TResult* GetResult()
        {
            if (this->IsSuccess() == FALSE) return NULL;
            return &this->_result;
        }

    protected:
        virtual Bool _VUpdateWithThread() override = 0;

    protected:
        TResult _result;
    };
}  // namespace Core
