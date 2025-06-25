#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// 依存モジュール
#include "ActorModule.h"
#include "Engine/Platform/PlatformModule.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Level/LevelNode.h"

namespace Level
{
    // class Manager;
    //  前方宣言
    class Node;

    /// <summary>
    /// レベル用の追加モジュール
    /// </summary>
    class LevelModule final : public Module::ModuleBase
    {
        HE_CLASS_COPY_NG(LevelModule);
        HE_CLASS_MOVE_NG(LevelModule);
        HE_MODULE_GENRATE_DECLARATION(LevelModule);

    public:
        LevelModule();

        /// <summary>
        /// メインレベルの切り替え
        /// </summary>
        template <class T>
        HE::Bool ChangeMainLevel()
        {
            HE_STATIC_ASSERT(std::is_base_of<Node, T>::value,
                             "Tクラスはレベルのノードクラスを継承していない");

            // レベルのノードは使いまわさない
            Core::Common::Handle handle = this->_upNodeManager->Add<T>();
            if (handle.Null()) return FALSE;

            // メインレベルの初期化
            return this->_InitMainLevel(handle);
        }

        Node& GetLevel(const Core::Common::Handle& in_rHandle) const;

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        HE::Bool _VStart() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        HE::Bool _VRelease() override final;

        /// <summary>
        /// 前更新
        /// </summary>
        void _VBeforeUpdate(const HE::Float32) override final;

        /// <summary>
        /// 更新
        /// </summary>
        void _VUpdate(const HE::Float32) override final;

        /// <summary>
        /// 後更新
        /// </summary>
        void _VLateUpdate(const HE::Float32) override final;

    private:
        /// <summary>
        /// ユーザー入力処理
        /// </summary>
        // void _ProcessInput(void*);

        HE::Bool _InitMainLevel(const Core::Common::Handle&);

    private:
        Core::Common::Handle _inputEventListenerHandle;

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
