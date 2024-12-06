#include "AsyncTaskManager.h"

#include <thread>

#include "AsyncTask.h"
#include "Engine/Memory/Memory.h"

namespace Core
{
    AsyncTaskManager::AsyncTaskManager(const Uint32 in_uMemPage)
        : _uMemPage(in_uMemPage), _bEnd(TRUE), _apThread(NULL)
    {
    }

    AsyncTaskManager::~AsyncTaskManager()
    {
        this->End();
    }

    Bool AsyncTaskManager::Init(const Uint32 in_uThreadNum)
    {
        HE_ASSERT(this->_bEnd);
        HE_ASSERT(0 < in_uThreadNum);

        this->_bEnd       = FALSE;
        this->_uThreadNum = in_uThreadNum;

        // スレッド生成
        this->_apThread = reinterpret_cast<std::thread**>(
            HE_ALLOC_MEM(sizeof(std::thread*) * in_uThreadNum, this->_uMemPage));
        for (Uint32 i = 0; i < this->_uThreadNum; ++i)
        {
            this->_apThread[i] =
                HE_NEW_MEM(std::thread, this->_uMemPage)(&AsyncTaskManager::_WorkerThread, this);
        }

        return TRUE;
    }

    void AsyncTaskManager::End()
    {
        if (this->_bEnd) return;

        this->_bEnd = TRUE;

        // スレッドを全て起こして終了させる
        {
            this->_cv.notify_all();

            for (Uint32 i = 0; i < this->_uThreadNum; ++i)
            {
                if (this->_apThread[i]->joinable()) this->_apThread[i]->join();

                HE_SAFE_DELETE_MEM(this->_apThread[i]);
            }
        }

        HE_SAFE_DELETE_MEM(this->_apThread);

        // 固定長にしているので全要素で破棄処理が必要
        {
            for (Uint32 i = 0; i < this->_vTaskQueue.Capacity(); ++i)
            {
                this->_vTaskQueue.GetPtr(i)->reset();
            }
        }
    }

    void AsyncTaskManager::Push(Memory::SharedPtr<AsyncTask> in_pTask)
    {
        // メモリ保護
        std::unique_lock<std::mutex> lock(this->_mutex);

        this->_vTaskQueue.PushBack(in_pTask);

        // スレッドを起動
        this->_cv.notify_one();
    }

    Memory::SharedPtr<AsyncTask> AsyncTaskManager::_PopWithThread()
    {
        // メモリ保護
        std::unique_lock<std::mutex> lock(this->_mutex);

        // タスクが存在するまで待機
        // 管理システムが終了した時も起動して処理を失敗にしている
        this->_cv.wait(lock,
                       [this] { return this->_bEnd || (this->_vTaskQueue.Empty() == FALSE); });

        if (this->_bEnd) return NULL;

        // 非同期タスクがあるか
        auto pTask = this->_vTaskQueue.PopBack();
        if (*pTask)
        {
            // 初期化が失敗したらスレッド休止
            if ((*pTask)->_VBeginWithThread()) return *pTask;

            // 失敗なので即終了処理を呼び出す
            (*pTask)->_VEndWithThread();
        }

        return NULL;
    }

    void AsyncTaskManager::_WorkerThread()
    {
        while (true)
        {
            // 非同期タスクがあるか
            auto pTask = this->_PopWithThread();
            if (pTask)
            {
                // 非同期タスクを実行
                while (pTask->_VUpdateWithThread())
                {
                    // 一定時間待機
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }

                pTask->_VEndWithThread();
            }
            else if (this->_bEnd)
            {
                break;
            }
        }
    }

}  // namespace Core
