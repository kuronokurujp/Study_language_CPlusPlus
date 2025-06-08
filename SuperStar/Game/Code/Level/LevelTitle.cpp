#include "LevelTitle.h"

#include "Common.h"
#include "Engine/Engine.h"
#include "LevelInGame.h"
#include "UIModule.h"

namespace Level
{
    namespace Local
    {
        constexpr HE::Char* szInputActionNameByGameStart = HE_STR_TEXT("TitleLevel_GameStart");
        constexpr HE::Char* szInputActionNameByGameEnd   = HE_STR_TEXT("TitleLevel_GameEnd");

        static const EnhancedInput::ActionMap mInputAction =
            {{szInputActionNameByGameStart,
              EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_SPACE})},
             {szInputActionNameByGameEnd,
              EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_A})}};
    };  // namespace Local

    HE::Bool LevelTitle::VBegin()
    {
        const HE::Bool bRet = Level::Node::VBegin();
        HE_ASSERT(bRet);

        // ユーザー共通入力割り当て設定
        {
            auto pInputModule = HE_ENGINE.ModuleManager().Get<EnhancedInput::EnhancedInputModule>();
            pInputModule->AddAction(Local::mInputAction);
        }

        /*
                // UIイベントをキャッチするコンポーネントを追加
                {
                    auto [compHandle, pComp] =
                        this->AddComponentByHandleAndComp<Level::LevelUserInputReceiveComponent>(
                            0, Actor::Component::EPriorty::EPriorty_Main);

                    auto handler = HE_MAKE_CUSTOM_UNIQUE_PTR((Level::LevelUserInputMessage),
                                                             [this](const HE::Char* in_pMsg)
                                                             { HE_LOG_LINE(in_pMsg); });
                    pComp->SetReceiver(std::move(handler));
                }
        */
        // TODO: UIのイベントリスナー登録
        {
        }

        // UIのBuilderファイルからレイアウト作成
        {
            auto pUIModule = HE_ENGINE.ModuleManager().Get<UI::UIModule>();

            this->_layoutAssetHandle = pUIModule->LoadAssetWithLayoutBuild(
                Core::File::Path(HE_STR_TEXT("UI"), HE_STR_TEXT("Builder"), HE_STR_TEXT("Game"),
                                 HE_STR_TEXT("Title.xml")));

            // widgetを作成
            // レベルが切り替わると自動的にwidgetは破棄される
            this->_widget = pUIModule->NewLayoutByLayotuAsset(this->_layoutAssetHandle, 0,
                                                              Game::g_sceneUIHandle);
        }

        return bRet;
    }

    HE::Bool LevelTitle::VEnd()
    {
        {
            auto pUIModule = HE_ENGINE.ModuleManager().Get<UI::UIModule>();
            pUIModule->DeleteWidget(this->_widget);
        }

        // 専用の入力アクションを外す
        {
            auto pInputModule = HE_ENGINE.ModuleManager().Get<EnhancedInput::EnhancedInputModule>();
            pInputModule->RemoveAction(Local::mInputAction);
        }

        // ロードしたアセットを破棄
        {
            auto pUIModule = HE_ENGINE.ModuleManager().Get<UI::UIModule>();
            pUIModule->UnloadAssetWithLayoutBuild(this->_layoutAssetHandle);
        }

        const HE::Bool bRet = Level::Node::VEnd();
        HE_ASSERT(bRet);

        return bRet;
    }

    void LevelTitle::VProcessInput(const EnhancedInput::InputMap& in_rInputMap)
    {
        Level::Node::VProcessInput(in_rInputMap);

        // メインゲーム開始入力があるか
        {
            if (in_rInputMap.Contains(Local::szInputActionNameByGameStart))
            {
                // メインゲームに遷移
                auto pLevelModule = HE_ENGINE.ModuleManager().Get<Level::LevelModule>();
                pLevelModule->ChangeMainLevel<Level::LevelInGame>();
                return;
            }
        }

        // ゲーム終了入力があるか
        {
            if (in_rInputMap.Contains(Local::szInputActionNameByGameEnd))
            {
                // TODO: ゲーム終了
                return;
            }
        }
    }
}  // namespace Level
