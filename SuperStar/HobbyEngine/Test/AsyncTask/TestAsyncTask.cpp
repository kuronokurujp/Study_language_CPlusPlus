#include <gtest/gtest.h>

#include "Engine/AsyncTask/AsyncTask.h"
#include "Engine/AsyncTask/AsyncTaskManager.h"
#include "Engine/Memory/Memory.h"

namespace Core
{
    TEST(HobbyEngine_Async, AsyncTask)
    {
        class AsyncTest : public Core::AsyncTaskResult<HE::Uint32>
        {
        public:
            AsyncTest(const HE::Uint32 in_count) : _count(in_count) {}

        protected:
            virtual HE::Bool _VBeginWithThread() override final
            {
                HE_LOG_LINE(HE_STR_TEXT("%d"), this->_count);
                if (Core::AsyncTaskResult<HE::Uint32>::_VBeginWithThread() == FALSE)
                {
                    return FALSE;
                }

                this->_result = 0;

                return TRUE;
            }

            virtual HE::Bool _VUpdateWithThread() override final
            {
                ++this->_result;

                // 成功している間はループされる
                return (this->_result < this->_count);
            }

            virtual HE::Bool _VEndWithThread() override final
            {
                if (Core::AsyncTaskResult<HE::Uint32>::_VEndWithThread() == FALSE) return FALSE;

                this->_bSuccess = TRUE;

                return TRUE;
            }

        private:
            HE::Uint32 _count = 0;
        };

        Core::AsyncTaskManager manager(0);
        EXPECT_EQ(manager.Init(32), TRUE);
        {
            Core::Memory::SharedPtr<AsyncTest> asyncTask =
                HE_MAKE_CUSTOM_SHARED_PTR((AsyncTest), 100);
            Core::Memory::SharedPtr<AsyncTest> asyncTask2 =
                HE_MAKE_CUSTOM_SHARED_PTR((AsyncTest), 2100);
            Core::Memory::SharedPtr<AsyncTest> asyncTask3 =
                HE_MAKE_CUSTOM_SHARED_PTR((AsyncTest), 300);
            manager.Push(asyncTask);
            manager.Push(asyncTask2);
            manager.Push(asyncTask3);

            // メインスレッド待機
            asyncTask->Wait();
            {
                HE_LOG_LINE(HE_STR_TEXT("End AsyncTask"));

                EXPECT_EQ(asyncTask->IsSuccess(), TRUE);
                if (asyncTask->IsSuccess())
                {
                    auto pResult = asyncTask->GetResult();
                    EXPECT_EQ(*pResult, 100);
                }
            }

            asyncTask2->Wait();
            {
                HE_LOG_LINE(HE_STR_TEXT("End AsyncTask2"));

                EXPECT_EQ(asyncTask2->IsSuccess(), TRUE);
                if (asyncTask2->IsSuccess())
                {
                    auto pResult = asyncTask2->GetResult();
                    EXPECT_EQ(*pResult, 2100);
                }
            }

            asyncTask3->Wait();
            {
                HE_LOG_LINE(HE_STR_TEXT("End AsyncTask3"));

                EXPECT_EQ(asyncTask3->IsSuccess(), TRUE);
                if (asyncTask3->IsSuccess())
                {
                    auto pResult = asyncTask3->GetResult();
                    EXPECT_EQ(*pResult, 300);
                }
            }
        }

        // 一定時間を得てtask破棄されても問題なく動作するか
        {
            Core::Memory::SharedPtr<AsyncTest> asyncTask4 =
                HE_MAKE_CUSTOM_SHARED_PTR((AsyncTest), 1200);
            manager.Push(asyncTask4);
            HE::Uint32 count = 10;
            while (0 < count)
            {
                --count;
            }
        }

        // 一定時間毎にメインスレッドを動くか
        {
            Core::Memory::SharedPtr<AsyncTest> asyncTask4 =
                HE_MAKE_CUSTOM_SHARED_PTR((AsyncTest), 1000);
            manager.Push(asyncTask4);
            while (asyncTask4->Contiune(10))
            {
                HE_LOG_LINE(HE_STR_TEXT("contiune"));
            }
            HE::Uint32 uNum = *asyncTask4->GetResult();
            EXPECT_EQ(uNum, 1000);
        }

        manager.End();
    }
}  // namespace Core
