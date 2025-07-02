#include <gtest/gtest.h>

#include <chrono>
#include <ctime>

#include "Engine/Task/Task.h"
#include "Engine/Task/TaskManager.h"
#include "Engine/Task/TaskTree.h"

namespace Core
{
    namespace Local
    {
        /// <summary>
        /// タスクグループ一覧
        /// </summary>
        enum EGroup
        {
            EGroup_System,
            EGroup_Player,
            EGroup_Enemy,
            EGroup_Effect,
            EGroup_Post,

            EGroup_Num
        };

        // テスト用のタスクたち
        class EffectTask : public Task
        {
        public:
            EffectTask() : Task() {}

            HE::Bool VBegin() override final { return TRUE; }
            HE::Bool VEnd() override final { return TRUE; }

            void VUpdate(const HE::Float32 in_fDt) override final
            {
                this->_time += in_fDt;
                HE_LOG(HE_STR_TEXT("F"));

                // 3秒経過したら自動的に死ぬ
                if (this->_time >= 3000.0f) this->Kill();
            }

        private:
            HE::Float32 _time = 0.0f;
        };

        class ObjectTask : public Task
        {
        public:
            ObjectTask() : Task() {}

            HE::Bool VBegin() override final { return TRUE; }
            HE::Bool VEnd() override final { return TRUE; }

            void VUpdate(const HE::Float32 in_fDt) override
            {
                this->_time += in_fDt;
                HE_LOG(HE_STR_TEXT("P"));

                // 1/30の確率でエフェクトを発生させる
                if (rand() % 30 == 0)
                    this->_pTaskManager->CreateAndAdd<EffectTask>(EGroup_System, TRUE);
            }

        private:
            HE::Float32 _time = 0.0f;
        };

        class EnemyTask : public ObjectTask
        {
        public:
            EnemyTask() : ObjectTask() {}

            void VUpdate(const HE::Float32 in_fDt) override final
            {
                this->_time += in_fDt;
                HE_LOG(HE_STR_TEXT("E"));
            }

        private:
            HE::Float32 _time = 0.0f;
        };

        class DummySystemTask : public Task
        {
        public:
            DummySystemTask() {}

            HE::Bool VBegin() override final { return TRUE; }
            HE::Bool VEnd() override final { return TRUE; }

            void VUpdate(const HE::Float32 in_fDt) override final
            {
                this->_time += in_fDt;
                HE_LOG(HE_STR_TEXT("S"));
            }

        private:
            HE::Float32 _time = 0.0f;
        };

    }  // namespace Local

    TEST(HobbyEngine_Task, TaskInitAndEnd)
    {
        static TaskManager manager;
        static const HE::Uint32 taskNum = 256;

        // タスクシステムの初期化
        EXPECT_EQ(TASK_MANAGER_INIT(manager, taskNum, 3), TRUE);

        // タスクの確保数が意図通りか
        EXPECT_EQ(manager.Max(), taskNum);

        // タスクの利用数が意図通りか
        EXPECT_EQ(manager.UseCount(), 0);

        // タスクシステムの終了
        manager.End();
    }

    TEST(HobbyEngine_Task, TaskAddAndRemove)
    {
        static TaskManager manager;
        static const HE::Uint32 taskNum = 256;

        // タスクシステムの初期化
        EXPECT_EQ(TASK_MANAGER_INIT(manager, taskNum, Local::EGroup_Num), TRUE);
        // タスクの確保数が意図通りか
        EXPECT_EQ(manager.Max(), taskNum);
        // タスクの利用数が意図通りか
        EXPECT_EQ(manager.UseCount(), 0);

        // システムタスクを起動する
        {
            auto h01 = manager.CreateAndAdd<Local::DummySystemTask>(Local::EGroup_System, TRUE);
            auto h02 = manager.CreateAndAdd<Local::DummySystemTask>(Local::EGroup_System, TRUE);
            EXPECT_NE(manager.GetTask(h01), manager.GetTask(h02));
        }

        // プレイヤータスクを起動する
        Common::Handle&& hPlayer =
            manager.CreateAndAdd<Local::ObjectTask>(Local::EGroup_Player, TRUE);

        manager.CreateAndAdd<Local::ObjectTask>(Local::EGroup_Player, TRUE);

        // エネミータスクを起動する
        manager.CreateAndAdd<Local::EnemyTask>(Local::EGroup_Enemy, TRUE);
        manager.CreateAndAdd<Local::EnemyTask>(Local::EGroup_Enemy, TRUE);
        manager.CreateAndAdd<Local::EnemyTask>(Local::EGroup_Enemy, TRUE);

        // エフェクトタスクを起動
        manager.CreateAndAdd<Local::EffectTask>(Local::EGroup_Effect, TRUE);

        // タスクの利用数が意図通りか
        EXPECT_EQ(manager.UseCount(), 8);
        {
            // 擬似ゲームループ
            const HE::Sint32 FPS       = 60;
            HE::Float32 msec_one_frame = 1000.0f / FPS;
            HE::Float32 end_time       = 10 * 1000.0f;  // 10秒
            HE::Float32 delta_time = 0.0f, boot_time = 0.0f;

            while (boot_time < end_time)
            {
                HE::Sint64 beginTimeMSec = std::chrono::duration_cast<std::chrono::milliseconds>(
                                               std::chrono::system_clock::now().time_since_epoch())
                                               .count();

                boot_time += delta_time;

                // タスクの更新
                manager.UpdateAll(delta_time);

                HE_LOG(HE_STR_TEXT(" (%d / %d)\n"), manager.UseCount(), manager.Max());

                // フレーム更新間隔まで待つ
                do
                {
                    HE::Sint64 nowTimeMSec =
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count();

                    delta_time = static_cast<HE::Float32>(nowTimeMSec - beginTimeMSec);
                } while (delta_time < msec_one_frame);

                // 設定したタスクが取得できるか
                {
                    Local::ObjectTask* pPlayer =
                        dynamic_cast<Local::ObjectTask*>(manager.GetTask(hPlayer));
                    EXPECT_EQ(pPlayer->GetType(), Task::EType::EType_Base);
                    EXPECT_NE(pPlayer, NULL);
                }
            }
        }

        // タスクシステムの終了
        manager.End();

        // タスクシステムが終了しているので解放されているか
        EXPECT_EQ(manager.UseCount(), 0);

        // タスクシステムが終了しているので取得が失敗しているか
        {
            Local::ObjectTask* pPlayer = dynamic_cast<Local::ObjectTask*>(manager.GetTask(hPlayer));
            EXPECT_EQ(pPlayer, NULL);
        }
    }

    TEST(HobbyEngine_Task, TaskMoveTask)
    {
        static TaskManager manager;
        static const HE::Uint32 taskNum = 256;

        // タスクシステムの初期化
        EXPECT_EQ(TASK_MANAGER_INIT(manager, taskNum, Local::EGroup_Num), TRUE);
        // タスクの確保数が意図通りか
        EXPECT_EQ(manager.Max(), taskNum);
        // タスクの利用数が意図通りか
        EXPECT_EQ(manager.UseCount(), 0);

        // タスクを起動する
        auto h1 = manager.CreateAndAdd<Local::ObjectTask>(Local::EGroup_Player, TRUE);
        auto h2 = manager.CreateAndAdd<Local::ObjectTask>(Local::EGroup_Player, TRUE);
        auto h3 = manager.CreateAndAdd<Local::ObjectTask>(Local::EGroup_Player, TRUE);

        // タスクの利用数が意図通りか
        EXPECT_EQ(manager.UseCount(), 3);

        // グループ設定したタスクの総数が意図通りか
        EXPECT_EQ(manager.Count(Local::EGroup_Player), 3);
        EXPECT_EQ(manager.Count(Local::EGroup_System), 0);
        EXPECT_EQ(manager.Count(Local::EGroup_Enemy), 0);

        // グループ移動
        manager.MoveGroupAll(Local::EGroup_Player, Local::EGroup_System);

        // タスクの利用数が意図通りか
        EXPECT_EQ(manager.UseCount(), 3);

        // グループ設定したタスクの総数が意図通りか
        EXPECT_EQ(manager.Count(Local::EGroup_Player), 0);
        EXPECT_EQ(manager.Count(Local::EGroup_System), 3);
        EXPECT_EQ(manager.Count(Local::EGroup_Enemy), 0);

        manager.MoveGropuTask(h1, Local::EGroup_Player);
        EXPECT_EQ(manager.Count(Local::EGroup_Player), 1);
        EXPECT_EQ(manager.Count(Local::EGroup_System), 2);
        EXPECT_EQ(manager.Count(Local::EGroup_Enemy), 0);

        manager.MoveGropuTask(h2, Local::EGroup_Player);
        EXPECT_EQ(manager.Count(Local::EGroup_Player), 2);
        EXPECT_EQ(manager.Count(Local::EGroup_System), 1);
        EXPECT_EQ(manager.Count(Local::EGroup_Enemy), 0);

        manager.MoveGropuTask(h1, Local::EGroup_Enemy);
        manager.MoveGropuTask(h2, Local::EGroup_Player);
        manager.MoveGropuTask(h3, Local::EGroup_Player);
        EXPECT_EQ(manager.Count(Local::EGroup_Player), 2);
        EXPECT_EQ(manager.Count(Local::EGroup_System), 0);
        EXPECT_EQ(manager.Count(Local::EGroup_Enemy), 1);

        // タスクシステムの終了
        manager.End();

        // タスクシステムが終了しているので解放されているか
        EXPECT_EQ(manager.UseCount(), 0);
    }

    TEST(HobbyEngine_Task, TaskCreateOrDelete)
    {
        static TaskManager manager;
        static const HE::Uint32 taskNum = 256;

        // タスクシステムの初期化
        EXPECT_EQ(TASK_MANAGER_INIT(manager, taskNum, Local::EGroup_Num), TRUE);
        // タスクの確保数が意図通りか
        EXPECT_EQ(manager.Max(), taskNum);
        // タスクの利用数が意図通りか
        EXPECT_EQ(manager.UseCount(), 0);

        // タスクを起動する
        // 削除した時にメモリから解放
        auto h1 = manager.CreateAndAdd<Local::EffectTask>(Local::EGroup_Player, TRUE);
        EXPECT_EQ(h1.Null(), FALSE);
        auto h2 = manager.CreateAndAdd<Local::EffectTask>(Local::EGroup_Player, TRUE);
        EXPECT_EQ(h2.Null(), FALSE);
        auto h3 = manager.CreateAndAdd<Local::EffectTask>(Local::EGroup_Player, FALSE);
        EXPECT_EQ(h3.Null(), FALSE);

        // タスクの利用数が意図通りか
        EXPECT_EQ(manager.UseCount(), 3);

        manager.RemoveTask(h1);
        EXPECT_EQ(manager.UseCount(), 2);
        EXPECT_EQ(manager.GetTask(h1), NULL);
        EXPECT_EQ(manager.CacheCount(), 0);

        manager.RemoveTask(h3);
        EXPECT_EQ(manager.CacheCount(), 1);
        EXPECT_EQ(manager.UseCount(), 1);

        auto h4 = manager.CreateAndAdd<Local::EffectTask>(Local::EGroup_Player, TRUE);
        EXPECT_EQ(h4.Null(), FALSE);
        EXPECT_EQ(manager.CacheCount(), 0);
        // タスクの利用数が意図通りか
        EXPECT_EQ(manager.UseCount(), 2);

        // タスクシステムの終了
        manager.End();

        // タスクシステムが終了しているので解放されているか
        EXPECT_EQ(manager.UseCount(), 0);
    }

    // テスト用のタスクたち
    class EffectTaskTree final : public TaskTree
    {
    public:
        EffectTaskTree() : TaskTree() {}

        void VUpdate(const HE::Float32 in_fDt) override
        {
            TaskTree::VUpdate(in_fDt);

            this->_time += in_fDt;
            HE_LOG(HE_STR_TEXT("F"));

            // 3秒経過したら自動的に死ぬ
            if (this->_time >= 3000.0f) this->Kill();
        }

    private:
        HE::Float32 _time = 0.0f;
    };

    class ObjectTaskTree : public TaskTree
    {
    public:
        ObjectTaskTree() : TaskTree() {}

        HE::Bool VBegin() override final { return TRUE; }
        HE::Bool VEnd() override final { return TRUE; }

        void VUpdate(const HE::Float32 in_fDt) override
        {
            this->_time += in_fDt;
            HE_LOG(HE_STR_TEXT("P"));
        }

    private:
        HE::Float32 _time = 0.0f;
    };

    TEST(HobbyEngine_Task, TestTaskTree)
    {
        HE_LOG_LINE(HE_STR_TEXT("Test Task Tree: start"));

        /// <summary>
        /// タスクグループ一覧
        /// </summary>
        enum
        {
            EGroup_System,
            EGroup_Player,
            EGroup_Num
        };

        static TaskManager manager;
        static const HE::Uint32 taskNum = 256;

        // タスクシステムの初期化
        EXPECT_EQ(TASK_MANAGER_INIT(manager, taskNum, EGroup_Num), TRUE);
        // タスクの確保数が意図通りか
        EXPECT_EQ(manager.Max(), taskNum);
        // タスクの利用数が意図通りか
        EXPECT_EQ(manager.UseCount(), 0);

        // タスクを起動する
        // 削除した時にメモリから解放
        auto h1 = manager.CreateAndAdd<EffectTaskTree>(EGroup_Player, TRUE);
        EXPECT_EQ(h1.Null(), FALSE);
        auto h2 = manager.CreateAndAdd<ObjectTaskTree>(EGroup_Player, TRUE);
        EXPECT_EQ(h2.Null(), FALSE);
        auto h3 = manager.CreateAndAdd<EffectTaskTree>(EGroup_Player, FALSE);
        EXPECT_EQ(h3.Null(), FALSE);

        // タスクの利用数が意図通りか
        EXPECT_EQ(manager.UseCount(), 3);
        auto pTask = manager.GetTask(h1);

        auto pTaskTree = reinterpret_cast<TaskTree*>(pTask);
        EXPECT_EQ(pTaskTree->GetType(), Task::EType::EType_Tree);
        EXPECT_EQ(pTaskTree->AddChildTask(h1), FALSE);
        EXPECT_EQ(pTaskTree->AddChildTask(h2), TRUE);
        EXPECT_EQ(manager.UseCount(), 3);
        manager.UpdateAll(0);
        HE_LOG(HE_STR_TEXT(" (%d / %d)\n"), manager.UseCount(), manager.Max());

        // 親タスクが外したのだから子タスクも同時に外れているはず
        manager.RemoveTask(h1);
        EXPECT_EQ(manager.UseCount(), 1);
        EXPECT_EQ(manager.GetTask(h1), NULL);
        EXPECT_EQ(manager.GetTask(h2), NULL);
        EXPECT_NE(manager.GetTask(h3), NULL);
        EXPECT_EQ(manager.CacheCount(), 0);

        auto h4 = manager.CreateAndAdd<EffectTaskTree>(EGroup_Player, FALSE);
        EXPECT_EQ(h4.Null(), FALSE);

        auto h5 = manager.CreateAndAdd<EffectTaskTree>(EGroup_Player, FALSE);
        EXPECT_EQ(h5.Null(), FALSE);

        pTask = manager.GetTask(h3);

        pTaskTree = reinterpret_cast<TaskTree*>(pTask);
        pTaskTree->AddChildTask(h4);

        pTask = manager.GetTask(h4);

        pTaskTree = reinterpret_cast<TaskTree*>(pTask);
        pTaskTree->AddChildTask(h5);

        manager.UpdateAll(0);
        HE_LOG(HE_STR_TEXT(" (%d / %d)\n"), manager.UseCount(), manager.Max());

        // タスクシステムの終了
        manager.End();

        // タスクシステムが終了したのだから今まで確保したタスクがないとおかしい
        EXPECT_EQ(manager.GetTask(h1), NULL);
        EXPECT_EQ(manager.GetTask(h2), NULL);
        EXPECT_EQ(manager.GetTask(h3), NULL);

        // タスクシステムが終了しているので解放されているか
        EXPECT_EQ(manager.UseCount(), 0);

        HE_LOG_LINE(HE_STR_TEXT("Test Task Tree: end"));
    }

}  // namespace Core
