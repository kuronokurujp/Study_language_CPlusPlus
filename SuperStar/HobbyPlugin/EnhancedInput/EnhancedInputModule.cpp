#include "EnhancedInputModule.h"

#include "Engine/Common/CustomMap.h"
#include "Engine/Memory/Memory.h"

// 依存モジュール
#include "Engine/Platform/PlatformModule.h"
#include "EventModule.h"

namespace EnhancedInput
{
    EnhancedInputModule::EnhancedInputModule() : ModuleBase(ModuleName(), Module::ELayer_Logic, -10)
    {
        // 依存しているモジュールを設定
        // モジュールの依存設定していないと依存したモジュールが使えない
        this->_AppendDependenceModule<Platform::PlatformModule>();
        this->_AppendDependenceModule<Event::EventModule>();
    }

    void EnhancedInputModule::SetAction(const ActionMap& in_mrAction)
    {
        this->_mMappingAction = in_mrAction;
    }

    void EnhancedInputModule::AddAction(const ActionMap& in_mrAction)
    {
        for (auto it = in_mrAction.Begin(); it != in_mrAction.End(); ++it)
        {
            this->_mMappingAction.Add(it->_key, it->_data);
        }
    }

    void EnhancedInputModule::AddAction(const Core::Common::FixedString64& in_rKey,
                                        const ActionData& in_rAction)
    {
        this->_mMappingAction.Add(in_rKey, in_rAction);
    }

    void EnhancedInputModule::RemoveAction(const ActionMap& in_mrAction)
    {
        for (auto it = in_mrAction.Begin(); it != in_mrAction.End(); ++it)
        {
            this->_mMappingAction.Erase(it->_key);
        }
    }

    void EnhancedInputModule::RemoveAction(const HE::Char* in_pActonName)
    {
        HE_ASSERT(in_pActonName);
        this->_mMappingAction.Erase(in_pActonName);
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    /// <returns></returns>
    HE::Bool EnhancedInputModule::_VStart()
    {
        this->_spEventInput = HE_MAKE_CUSTOM_SHARED_PTR((EventInput));

        return TRUE;
    }

    HE::Bool EnhancedInputModule::_VRelease()
    {
        this->_spEventInput.reset();

        return TRUE;
    }

    void EnhancedInputModule::_VBeforeUpdate(const HE::Float32 in_fDeltaTime)
    {
    }

    void EnhancedInputModule::_VUpdate(const HE::Float32 in_fDeltaTime)
    {
        this->_spEventInput->_mInputAction.Clear();

        // this->_mInputAction.Clear();

        auto pPlatformModule = this->GetDependenceModule<Platform::PlatformModule>();
        HE_ASSERT(pPlatformModule);

        auto pInput = pPlatformModule->VInput();
        HE_ASSERT(pInput);

        // 入力処理をする
        if (this->_mMappingAction.Empty()) return;

        if (this->_inputHandle.Null()) return;

        // 扱う入力オブジェクトを取得
        auto pInputObj = pInput->GetObj(this->_inputHandle);
        HE_ASSERT_RETURN(pInputObj);

        // 登録した入力アクションが発生しているかチェック
        for (auto it = this->_mMappingAction.Begin(); it != this->_mMappingAction.End(); ++it)
        {
            InputData inputData;
            // 発生している場合は発生リストに登録
            for (HE::Uint32 i = 0; i < it->_data.aKeyboardKeys.Size(); ++i)
            {
                const auto eKey = it->_data.aKeyboardKeys[i];
                if (pInputObj->Keyboard().GetKeyState(eKey))
                {
                    HE_CLS_MEM(&inputData, sizeof(inputData));
                    inputData.eType              = EInputType_Keyboard;
                    inputData.item.keyboard.eKey = eKey;

                    // this->_mInputAction[it->_key].PushBack(inputData);
                    this->_spEventInput->_mInputAction[it->_key].PushBack(inputData);
                }
            }

            for (HE::Uint32 i = 0; i < it->_data.aTouchs.Size(); ++i)
            {
                const auto eTouch      = it->_data.aTouchs[i];
                const auto eTouchState = pInputObj->Touch().GetTouchState(it->_data.aTouchs[i]);
                if (eTouchState == Platform::EInputState::EInputState_PRESSED)
                {
                    const auto pos = pInputObj->Touch().GetWorldPos();

                    HE_CLS_MEM(&inputData, sizeof(inputData));
                    inputData.eType            = EInputType_Touch;
                    inputData.item.touch._fX   = pos._fX;
                    inputData.item.touch._fY   = pos._fY;
                    inputData.item.touch.eType = eTouch;

                    // auto& r = this->_mInputAction[it->_key];
                    auto& r = this->_spEventInput->_mInputAction[it->_key];
                    r.PushBack(inputData);
                }
            }
        }

        // TODO: 入力結果をイベント送信
        if (this->_spEventInput->Empty() == FALSE)
        {
            auto* pEventModule = this->GetDependenceModule<Event::EventModule>();
            pEventModule->QueueEvent(this->_spEventInput, EVENT_TYPE_ENHANCEDINPUT);
        }
    }

}  // namespace EnhancedInput
