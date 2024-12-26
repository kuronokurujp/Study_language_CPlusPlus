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
        HE_ASSERT(bRet);

        // UIイベントをキャッチするコンポーネントを追加
        {
            auto [h, comp] =
                this->AddComponentByHandleAndComp<Level::LevelUserInputReceiveComponent>(
                    0, Actor::Component::EPriorty::EPriorty_Main);

            auto handler = HE_MAKE_CUSTOM_UNIQUE_PTR(
                (Level::LevelUserInputMessage),
                [this](const HE::Char* in_pMsg)
                {
                    HE_LOG_LINE(in_pMsg);
                    // 次のレベルへ遷移
                    if (HE_STR_CMP(in_pMsg, HE_STR_TEXT("C_TitleSeq")) == 0)
                    {
                        auto pLevelModule = HE_ENGINE.ModuleManager().Get<Level::LevelModule>();
                        // タイトルへ遷移する
                        pLevelModule->GetManager()->StartLevel<Level::LevelTitle>();
                    }
                });
            comp->SetReceiver(std::move(handler));
        }

        // UIのBuilderファイルからレイアウト作成
        {
            auto pUIModule           = HE_ENGINE.ModuleManager().Get<UI::UIModule>();
            this->_layoutAssetHandle = pUIModule->LoadAssetWithLayoutBuild(
                Core::File::Path(HE_STR_TEXT("UI"), HE_STR_TEXT("Builder"), HE_STR_TEXT("Test"),
                                 HE_STR_TEXT("Launcher.xml")));
            // widgetを作成
            // レベルが切り替わると自動的にwidgetは破棄される
            pUIModule->NewLayoutByLayotuAsset(this->_layoutAssetHandle, 0, Game::g_sceneUIHandle,
                                              this->Handle());
        }

        return bRet;
    }

    HE::Bool LevelLauncher::VEnd()
    {
        // ロードしたアセットを破棄
        {
            auto pUIModule = HE_ENGINE.ModuleManager().Get<UI::UIModule>();
            pUIModule->UnloadAssetWithLayoutBuild(this->_layoutAssetHandle);
        }

        const HE::Bool bRet = Level::Node::VEnd();
        HE_ASSERT(bRet);

        return TRUE;
    }

}  // namespace Level

#endif
