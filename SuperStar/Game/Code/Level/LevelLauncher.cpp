#include "Engine/Engine.h"

// デバッグ用なのでリリース版には含めない
#ifdef HE_ENGINE_DEBUG

#include "Common.h"
#include "Engine/Common/CustomMap.h"
#include "Engine/Math/Math.h"
#include "LevelLauncher.h"
#include "LevelTitle.h"

// モジュール
#include "RenderModule.h"
#include "UIModule.h"

// デバッグ画面を表示してレベルを切り替える
namespace Level
{
    HE::Bool LevelLauncher::VBegin()
    {
        const HE::Bool bRet = Level::Node::VBegin();
        HE_ASSERT_RETURN_VALUE(FALSE, bRet);

        // TODO: UIのイベントリスナー登録
        {
            auto pEventModule      = HE_ENGINE.ModuleManager().Get<Event::EventModule>();
            auto spUIEventListener = HE_MAKE_CUSTOM_SHARED_PTR(
                (Event::EventListenerWithRegistEventFunc), HE_STR_TEXT("UI"),
                [this](Event::EventDataInterfacePtr const& in_spEventData)
                {
                    // TODO: UIのボタンクリック受信
                    if (in_spEventData->VEventHash() == UI::EventButtonClick::Hash())
                    {
                        auto pEvent = reinterpret_cast<UI::EventButtonClick*>(in_spEventData.get());

                        // 次のレベルへ遷移
                        if (pEvent->_szMsg == HE_STR_TEXT("C_TitleSeq"))
                        {
                            auto pLevelModule = HE_ENGINE.ModuleManager().Get<Level::LevelModule>();
                            // タイトルへ遷移する
                            pLevelModule->ChangeMainLevel<Level::LevelTitle>();
                        }
                    }

                    return TRUE;
                });

            this->_ulEventListenerHandle =
                pEventModule->AddListener(spUIEventListener, EVENT_TYPE_UIMODULE);
            HE_ASSERT_RETURN_VALUE(FALSE, (this->_ulEventListenerHandle.Null() == FALSE) &&
                                              "UIのイベントリスナー設定ができない");
        }

        // UIのBuilderファイルからレイアウト作成
        {
            auto pUIModule           = HE_ENGINE.ModuleManager().Get<UI::UIModule>();
            this->_layoutAssetHandle = pUIModule->LoadAssetWithLayoutBuild(
                Core::File::Path(HE_STR_TEXT("UI"), HE_STR_TEXT("Builder"), HE_STR_TEXT("Test"),
                                 HE_STR_TEXT("Launcher.xml")));
            // widgetを作成
            this->_widget = pUIModule->NewLayoutByLayotuAsset(this->_layoutAssetHandle, 0,
                                                              Game::g_sceneUIHandle);
        }

        return bRet;
    }

    HE::Bool LevelLauncher::VEnd()
    {
        // TODO: 作成したUIWidgetを破棄
        {
            auto pUIModule = HE_ENGINE.ModuleManager().Get<UI::UIModule>();
            pUIModule->DeleteWidget(this->_widget);
        }

        // ロードしたアセットを破棄
        {
            auto pUIModule = HE_ENGINE.ModuleManager().Get<UI::UIModule>();
            pUIModule->UnloadAssetWithLayoutBuild(this->_layoutAssetHandle);
        }

        // TODO: UI用のイベントリスナーを外す
        {
            auto pEventModule = HE_ENGINE.ModuleManager().Get<Event::EventModule>();
            pEventModule->RemoveListener(this->_ulEventListenerHandle);
        }

        const HE::Bool bRet = Level::Node::VEnd();
        HE_ASSERT(bRet);

        return TRUE;
    }

}  // namespace Level

#endif
