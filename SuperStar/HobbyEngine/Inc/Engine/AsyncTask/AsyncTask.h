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
        HE::Bool Contiune(const HE::Uint32);

        /// <summary>
        /// タスクが終わっているか
        /// </summary>
        inline HE::Bool IsEnd() const { return this->_bEnd; }

        /// <summary>
        /// タスクが成功しているか
        /// </summary>
        HE::Bool IsSuccess() const;

    protected:
        virtual HE::Bool _VBeginWithThread();
        virtual HE::Bool _VUpdateWithThread() = 0;
        virtual HE::Bool _VEndWithThread();

    protected:
        HE::Bool _bSuccess = FALSE;

    private:
        std::atomic<HE::Bool> _bEnd{FALSE};
        std::promise<HE::Bool> _promise;
        std::future<HE::Bool> _future;
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
        virtual HE::Bool _VUpdateWithThread() override = 0;

    protected:
        TResult _result;
    };
}  // namespace Core
