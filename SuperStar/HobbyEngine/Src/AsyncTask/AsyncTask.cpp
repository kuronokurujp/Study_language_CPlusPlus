#include "Engine/AsyncTask/AsyncTask.h"

namespace Core
{
    AsyncTask::AsyncTask() : _bEnd(FALSE), _bSuccess(FALSE)
    {
        this->_future = this->_promise.get_future();
    }

    void AsyncTask::Wait()
    {
        this->_future.wait();
    }

    /// <summary>
    /// 非同期処理が終わるまで一定時間メインスレッド待機する
    /// </summary>
    HE::Bool AsyncTask::Contiune(const HE::Uint32 in_uMsSec)
    {
        // タスクが終了しているかどうか
        auto result = this->_future.wait_for(std::chrono::milliseconds(in_uMsSec));
        if (result == std::future_status::timeout)
            return TRUE;
        else if (result == std::future_status::deferred)
            return FALSE;

        return FALSE;
    }

    /// <summary>
    /// タスクが成功しているか
    /// </summary>
    HE::Bool AsyncTask::IsSuccess() const
    {
        if (this->_bEnd) return this->_bSuccess;
        return FALSE;
    }

    HE::Bool AsyncTask::_VBeginWithThread()
    {
        return TRUE;
    }

    HE::Bool AsyncTask::_VEndWithThread()
    {
        // 非同期タスク終了
        this->_bEnd = TRUE;
        this->_promise.set_value(TRUE);

        return TRUE;
    }

}  // namespace Core
