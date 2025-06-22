#pragma once

// 最小エンジンインクルード
#include "Engine/Common/CustomVector.h"
#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"
#include "Engine/Platform/PlatformInput.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "EnhancedInput/Actor/ActorManagerDecorater.h"
#include "EnhancedInput/Actor/InputComponent.h"
#include "EnhancedInput/Event.h"

namespace EnhancedInput
{
    /// <summary>
    /// 入力アクションデータ
    /// </summary>
    struct ActionData
    {
        using ActionKeyMap   = Core::Common::FixedVector<Platform::EKeyboard, 4>;
        using ActionTouchMap = Core::Common::FixedVector<Platform::EInputMouseType, 4>;

        // CustomFixMapクラスでバッファデータを作成時に使う
        ActionData() {}
        ActionData(const ActionKeyMap& in_arKeys)
        {
            this->aTouchs.Clear();
            this->aKeyboardKeys = in_arKeys;
        }
        ActionData(const ActionTouchMap& in_arTouch)
        {
            this->aKeyboardKeys.Clear();
            this->aTouchs = in_arTouch;
        }
        ActionData(const ActionKeyMap& in_arKeys, const ActionTouchMap& in_arTouchs)
        {
            this->aKeyboardKeys = in_arKeys;
            this->aTouchs       = in_arTouchs;
        }

        // CustomFixMapクラスでデータをコピーする時に使う
        void operator=(const ActionData& in_rSrc)
        {
            this->aKeyboardKeys = in_rSrc.aKeyboardKeys;
            this->aTouchs       = in_rSrc.aTouchs;
        }

        /// <summary>
        /// キーボードキーデータ
        /// </summary>
        Core::Common::FixedVector<Platform::EKeyboard, 4> aKeyboardKeys;

        /// <summary>
        /// マウスやタッチ入力データ
        /// </summary>
        Core::Common::FixedVector<Platform::EInputMouseType, 4> aTouchs;
    };

    /// <summary>
    /// アクションデータのマップ型
    /// </summary>
    using ActionMap = Core::Common::FixedMap<Core::Common::FixedString64, ActionData, 32>;

    /// <summary>
    /// インプット用の追加モジュール
    /// </summary>
    class EnhancedInputModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(EnhancedInputModule);

    public:
        using EventInputSharedPtr = Core::Memory::SharedPtr<EventInput>;

    public:
        EnhancedInputModule();

        /// <summary>
        /// 入力を扱うハンドル
        /// </summary>
        void SetInputHandle(const Core::Common::Handle in_handle)
        {
            this->_inputHandle = in_handle;
        }

        /// <summary>
        /// アクションをマッピングデータを設定
        /// </summary>
        /// <param name="in_mrTable"></param>
        void SetAction(const ActionMap&);

        /// <summary>
        /// アクションマッピングデータを追加
        /// </summary>
        /// <param name=""></param>
        void AddAction(const ActionMap&);
        void AddAction(const Core::Common::FixedString64&, const ActionData&);

        /// <summary>
        /// アクションマッピングデータを外す
        /// </summary>
        /// <param name="in_pActonName"></param>
        void RemoveAction(const ActionMap&);
        void RemoveAction(const HE::Char*);

        /// <summary>
        /// 登録アクションの入力検出・格納処理
        /// </summary>
        void DetectInputActions(Platform::InputObject* pInputObj, EventInputSharedPtr& spEventInput);

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        HE::Bool _VStart() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        HE::Bool _VRelease() override final;

        void _VBeforeUpdate(const HE::Float32) override final;
        void _VUpdate(const HE::Float32) override final;

    private:
        /// <summary>
        /// マッピングしたアクションデータ
        /// </summary>
        ActionMap _mMappingAction;

        /// <summary>
        /// マッピングしたアクションの入力リスト
        /// </summary>
        // InputMap _mInputAction;

        Core::Common::Handle _inputHandle;
        Core::Common::Handle _eventHandle;

        EventInputSharedPtr _spEventInput;
    };

}  // namespace EnhancedInput
