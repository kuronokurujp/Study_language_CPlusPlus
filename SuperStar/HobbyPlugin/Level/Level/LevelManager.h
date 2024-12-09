#pragma once

#include "ActorModule.h"
#include "Engine/Common/CustomArray.h"
#include "Engine/MiniEngine.h"
#include "Engine/Task/Task.h"
#include "Engine/Task/TaskManager.h"
#include "EnhancedInputModule.h"

/// <summary>
/// レベルの管理
/// ゲーム中のアクターなどのオブジェクトを配置するレベルを管理
/// </summary>
namespace Level
{
    // 前方宣言
    class Node;

    /// <summary>
    /// レベルを管理するクラス
    /// 個々のレベルはタスク形式で管理する
    /// </summary>
    class Manager
    {
        HE_CLASS_COPY_NG(Manager);
        HE_CLASS_MOVE_NG(Manager);

    public:
        Manager() {}

        /// <summary>
        /// 必ず最初に呼び出す
        /// </summary>
        HE::Bool Init();

        /// <summary>
        /// 使い終わったら実行
        /// そのあとに利用したらエラーになる
        /// </summary>
        HE::Bool Release();

        /// <summary>
        /// ユーザー入力処理
        /// </summary>
        void ProcessInput(const HE::Float32 in_fDt, const EnhancedInput::InputMap&);

        /// <summary>
        /// 前更新
        /// </summary>
        void BeforeUpdate(const HE::Float32);

        /// <summary>
        /// 更新
        /// </summary>
        void Update(const HE::Float32);

        /// <summary>
        /// 後更新
        /// </summary>
        void LateUpdate(const HE::Float32);

        /// <summary>
        /// 起動するレベルを設定
        /// </summary>
        template <class T>
        HE::Bool StartLevel()
        {
            HE_STATIC_ASSERT(std::is_base_of<Node, T>::value,
                             "Tクラスはレベルのノードクラスを継承していない");

            // レベルのノードは使いまわさない
            Core::Common::Handle handle = this->_upNodeManager->Add<T>();
            if (handle.Null()) return FALSE;

            return this->_StartLevel(handle);
        }

        /// <summary>
        /// レベルを取得
        /// </summary>
        Node* GetLevel(const Core::Common::Handle&);

    private:
        HE::Bool _StartLevel(const Core::Common::Handle&);

    private:
        // レベルのノードをアクターとして管理
        Core::Memory::UniquePtr<Actor::ActorManager> _upNodeManager;

        /// <summary>
        /// カレントレベルのハンドル
        /// </summary>
        Core::Common::Handle _currentLevelHandle;

        /// <summary>
        ///  切り替え先のレベルのハンドル
        /// </summary>
        Core::Common::Handle _nextLevelHandle;
    };

}  // namespace Level
