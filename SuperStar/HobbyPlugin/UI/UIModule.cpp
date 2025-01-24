#include "UIModule.h"

#include "Engine/Common/CustomMap.h"
#include "Engine/Memory/Memory.h"
#include "LevelModule.h"

namespace UI
{
    // クラス内のみで利用する
    namespace Local
    {
        static Core::Common::FixedMap<Builder::EAnchor, Core::Math::EAnchor, 3>
            mPosAnthorToRect2Anchor = {{Builder::EAnchor::EAnchor_Left,
                                        Core::Math::EAnchor::EAnchor_Left},
                                       {Builder::EAnchor::EPosAnchor_Center,
                                        Core::Math::EAnchor::EAnchor_Center}};
    }

    UIModule::UIModule() : ModuleBase(ModuleName())
    {
        // 依存しているモジュールを設定
        // モジュールの依存設定していないと依存したモジュールが使えない
        this->_AppendDependenceModule<AssetManager::AssetManagerModule>();
        this->_AppendDependenceModule<Level::LevelModule>();
        this->_AppendDependenceModule<Localization::LocalizationModule>();
        this->_AppendDependenceModule<Render::RenderModule>();
        this->_AppendDependenceModule<Actor::ActorModule>();
    }

    Widget* UIModule::GetWidget(const UIWidgetHandlePack& in_rHandlePack)
    {
        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        Actor::Object* pActor = NULL;
        pActor                = pLevelModule->GetLevel(in_rHandlePack._levelHandle)
                     .GetActor<Actor::Object>(in_rHandlePack._widgetHandle);
        HE_ASSERT(pActor);

        return reinterpret_cast<Widget*>(pActor);
    }

    /// <summary>
    /// UIのモジュール初期化
    /// </summary>
    HE::Bool UIModule::_VStart()
    {
        return TRUE;
    }

    Core::Common::Handle UIModule::LoadAssetWithLayoutBuild(const Core::File::Path& in_rFilePath)
    {
        auto pAssetManagerModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pAssetManagerModule);
        Core::Common::Handle handle =

            pAssetManagerModule->Load<UI::Builder::UILayoutData>(in_rFilePath.Str(),
                                                                 Core::File::Path(
                                                                     in_rFilePath.Str()));

        HE_ASSERT(handle.Null() == FALSE);
        if (handle.Null()) return NullHandle;

        return handle;
    }

    void UIModule::UnloadAssetWithLayoutBuild(Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);

        auto pAssetManagerModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pAssetManagerModule);

        pAssetManagerModule->Unload(in_rHandle);
    }

    const UIWidgetHandlePack UIModule::NewLayer(const Core::Common::StringBase& in_szrName,
                                                const Core::Math::Vector2& in_rPos,
                                                const Core::Math::Vector2& in_rSize,
                                                const Core::Math::Color in_color,
                                                const HE::Uint32 in_uSort,
                                                const Core::Common::Handle in_viewHandle,
                                                const Core::Common::Handle in_levelHandle)
    {
        auto handlePack = this->NewWidget(in_szrName, in_uSort, in_levelHandle);

        auto [hInputRouter, pComp] = this->AddComponent<Actor::InputComponent>(handlePack, 0);
        // 入力ルーター設定
        {
            auto pInputStrategy = HE_MAKE_CUSTOM_SHARED_PTR((UI::UIInputRouterStrategy));

            auto pWidget         = this->GetWidget(handlePack);
            auto pInputComponent = pWidget->GetComponent<Actor::InputComponent>(hInputRouter);
            pInputComponent->SetStrategy(pInputStrategy);
        }

        // レイヤーコンポーネントを追加
        auto [hLayer, pLayourComp] = this->AddComponent<UI::UILayerComponent>(handlePack, 0);
        pLayourComp->SetPos(in_rPos);
        pLayourComp->SetSize(in_rSize);
        pLayourComp->SetColor(in_color.c);
        pLayourComp->SetViewHandle(in_viewHandle);

        return handlePack;
    }

    const UIWidgetHandlePack UIModule::NewLayoutByLayotuAsset(
        const Core::Common::Handle& in_rAssetHandle, const HE::Uint32 in_uSort,
        const Core::Common::Handle& in_rViewHandle, const Core::Common::Handle& in_rLevelHandle)
    {
        HE_ASSERT(in_rAssetHandle.Null() == FALSE);
        HE_ASSERT(in_rLevelHandle.Null() == FALSE);
        HE_ASSERT(in_rViewHandle.Null() == FALSE);

        auto pAssetManagerModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pAssetManagerModule);

        auto& asset = pAssetManagerModule->GetAsset<UI::Builder::UILayoutData>(in_rAssetHandle);

        // ルートノードを取得
        UI::Builder::Node node;
        auto bRet = asset.OutputNode(&node, {HE_STR_U8_TEXT("ui")});
        HE_ASSERT(bRet);

        // ルートノードからレイアウトノードを取得
        // TODO: レイヤーが複数ある場合の対応がない
        UI::Builder::Node layerNode;
        bRet = asset.OutputNode(&layerNode, {HE_STR_U8_TEXT("ui"), HE_STR_U8_TEXT("layer")});
        HE_ASSERT(bRet);
        HE_ASSERT(layerNode._data._eWidgetType == UI::Builder::EWidget_Layer &&
                  "レイヤーノードが存在しない");

        // レイアウトを作成
        UIWidgetHandlePack rootLayerHandle;
        {
            auto pLayerData = &layerNode._data._exData._layer;
            Core::Math::Color c{pLayerData->_style._uColor};

            rootLayerHandle =
                this->NewLayer(Core::Common::FixedString64(layerNode._data._szId),
                               Core::Math::Vector2(pLayerData->_fX, pLayerData->_fY),
                               Core::Math::Vector2(pLayerData->_style._fW, pLayerData->_style._fH),
                               c, in_uSort, in_rViewHandle, in_rLevelHandle);
        }

        UIWidgetHandlePack parentHandle;
        asset.RecursiveOperationByNode(
            {HE_STR_U8_TEXT("ui"), HE_STR_U8_TEXT("layer")},
            [this, in_rViewHandle, in_rLevelHandle, &parentHandle,
             rootLayerHandle](const UI::Builder::Node& in_rWidgetData, HE::Uint32 in_uSort,
                              const HE::Bool in_bRoot)
            {
                if (in_bRoot)
                {
                    parentHandle = rootLayerHandle;
                }

                auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
                HE_ASSERT(pLevelModule);

                const auto pNodeData = &in_rWidgetData._data;
                // TODO: 関数テーブルにしてswitch文を消す方向にするかも
                // 今は種類が少ないからいいが, 数が膨大になるとまずい
                switch (pNodeData->_eWidgetType)
                {
                    case UI::Builder::EWidget_None:
                        break;
                    case UI::Builder::EWidget_Widget:
                    {
                        // TODO: 未対応
                        HE_ASSERT(FALSE);
                        break;
                    }
                    case UI::Builder::EWidget_Label:
                    {
                        const UI::Builder::Node::Data::ExData::Label* pLabel =
                            &pNodeData->_exData._label;
                        const UI::Builder::Style* pStyle = &pLabel->_style;
                        Core::Math::Rect2 rect;
                        rect.SetPosition(pLabel->_fX, pLabel->_fY, pStyle->_fW, pStyle->_fH,
                                         Local::mPosAnthorToRect2Anchor[pLabel->_eAnchor]);

                        auto h = this->NewLabelWidget(Core::Common::FixedString64(pNodeData->_szId),
                                                      in_uSort, pLabel->szLoc, pLabel->szText,
                                                      pStyle->_uSize, rect, pStyle->_uColor,
                                                      in_rViewHandle, in_rLevelHandle);

                        this->AddChildWidget(parentHandle, h);
                        parentHandle = h;

                        break;
                    }
                    case UI::Builder::EWidget_Button:
                    {
                        const UI::Builder::Node::Data::ExData::Button* pButton =
                            &pNodeData->_exData.button;
                        const UI::Builder::Style* pStyle = &pButton->_style;

                        Core::Math::Rect2 rect;
                        rect.SetPosition(pButton->_fX, pButton->_fY, pStyle->_fW, pStyle->_fH,
                                         Local::mPosAnthorToRect2Anchor[pButton->_eAnchor]);
                        auto h =
                            this->NewButtonWidget(Core::Common::FixedString64(pNodeData->_szId),
                                                  in_uSort, rect, pStyle->_uColor, in_rViewHandle,
                                                  in_rLevelHandle);

                        // ボタンを押した時のイベントを設定
                        {
                            Actor::Object* pWidget = NULL;
                            pWidget = pLevelModule->GetLevel(parentHandle._levelHandle)
                                          .GetActor<Actor::Object>(h._widgetHandle);
                            HE_ASSERT(pWidget);

                            auto [btnWidgetHandle, pWidgetCmp] =
                                pWidget->GetComponentHandleAndComponent(
                                    &UI::UIButtonComponent::CLASS_RTTI);
                            HE_ASSERT(btnWidgetHandle.Null() == FALSE);

                            UI::UIButtonComponent* pBtnComp =
                                reinterpret_cast<UI::UIButtonComponent*>(pWidgetCmp);

                            auto handler = HE_MAKE_CUSTOM_UNIQUE_PTR(
                                (UI::UIButtonMessageHandlerDefault), pNodeData->_szId,
                                [this, parentHandle](Core::Common::StringBase& in_msg)
                                {
                                    auto pLevelModule =
                                        this->GetDependenceModule<Level::LevelModule>();
                                    HE_ASSERT(pLevelModule);

                                    // ボタン入力をレベルのコンポーネントに通知
                                    auto [handle, c] =
                                        pLevelModule->GetLevel(parentHandle._levelHandle)
                                            .GetComponentHandleAndComponent(
                                                &Level::LevelUserInputReceiveComponent::CLASS_RTTI);
                                    if (handle.Null() == FALSE)
                                    {
                                        Level::LevelUserInputReceiveComponent* pUserInputEventComp =
                                            reinterpret_cast<
                                                Level::LevelUserInputReceiveComponent*>(c);
                                        pUserInputEventComp->Message(in_msg.Str());
                                    }
                                });
                            pBtnComp->SetPushHandler(std::move(handler));
                        }

                        this->AddChildWidget(parentHandle, h);
                        parentHandle = h;

                        break;
                    }
                }
            });

        return rootLayerHandle;
    }  // namespace UI

    const UIWidgetHandlePack UIModule::NewLabelWidget(
        const Core::Common::StringBase& in_szrName, const HE::Uint32 in_uSort,
        const HE::Char* in_szLocGroupName, const HE::Char* in_szText, const HE::Uint32 in_uFontSize,
        const Core::Math::Rect2& in_rTextRect, const HE::Uint32 in_uTextColor,
        const Core::Common::Handle& in_rViewHandle, const Core::Common::Handle& in_rLevelHandle)
    {
        auto handlePack = this->NewWidget(in_szrName, in_uSort, in_rLevelHandle);

        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        Actor::Object* pWidget = NULL;

        pWidget = pLevelModule->GetLevel(handlePack._levelHandle)
                      .GetActor<Actor::Object>(handlePack._widgetHandle);
        if (pWidget == NULL) return handlePack;

        // ボタンの上に表示するテキストコンポーネント追加と設定
        auto [textHandle, pTextComp] =
            this->AddComponent<UI::UITextComponent>(handlePack, in_uSort + 1);
        {
            UI::UITextComponent* pText = pWidget->GetComponent<UI::UITextComponent>(textHandle);
            pText->SetPos(in_rTextRect.Pos());
            pText->SetViewHandle(in_rViewHandle);
            pText->SetText(in_szText);

            pText->SetSize(Core::Math::Vector2(in_rTextRect.Width(), in_rTextRect.Height()));
            pText->SetColor(in_uTextColor);
            pText->SetLocGroupName(in_szLocGroupName);
            pText->SetAnchor(in_rTextRect._eAnchor);
            pText->SetFontSize(in_uFontSize);
        }

        return handlePack;
    }

    const UIWidgetHandlePack UIModule::NewButtonWidget(const Core::Common::StringBase& in_szrName,
                                                       const HE::Uint32 in_uSort,
                                                       const Core::Math::Rect2& in_rBtnRect,
                                                       const HE::Uint32 in_uBtnColor,
                                                       const Core::Common::Handle& in_rViewHandle,
                                                       const Core::Common::Handle& in_rLevelHandle)

    {
        auto widgetHandle = this->NewWidget(in_szrName, in_uSort, in_rLevelHandle);

        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        Widget* pWidget = pLevelModule->GetLevel(widgetHandle._levelHandle)
                              .GetActor<Widget>(widgetHandle._widgetHandle);
        if (pWidget == NULL) return widgetHandle;

        // ボタンコンポーネント追加と設定
        auto [hButton, pBtnCmp] = this->AddComponent<UI::UIButtonComponent>(widgetHandle, in_uSort);
        {
            UI::UIButtonComponent* pButton = pWidget->GetComponent<UI::UIButtonComponent>(hButton);
            pButton->SetPos(in_rBtnRect.Pos());
            pButton->SetViewHandle(in_rViewHandle);
            pButton->SetSize(Core::Math::Vector2(in_rBtnRect.Width(), in_rBtnRect.Height()));
            pButton->SetColor(in_uBtnColor);
            pButton->SetAnchor(in_rBtnRect._eAnchor);
        }

        return widgetHandle;
    }

    const UIWidgetHandlePack UIModule::NewWidget(const Core::Common::StringBase& in_szrName,
                                                 const HE::Uint32 in_uSort,
                                                 const Core::Common::Handle& in_rLevelHandle)
    {
        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        auto handle = pLevelModule->GetLevel(in_rLevelHandle).AddActor<Widget>();
        HE_ASSERT(handle.Null() == FALSE);

        return UIWidgetHandlePack(handle, in_rLevelHandle);
    }

    HE::Bool UIModule::AddChildWidget(UIWidgetHandlePack& in_rParent,
                                      UIWidgetHandlePack& in_rWidget)
    {
        HE_ASSERT(in_rParent._levelHandle == in_rWidget._levelHandle);
        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        return pLevelModule->GetLevel(in_rParent._levelHandle)
            .ChainActor(in_rWidget._widgetHandle, in_rParent._widgetHandle);
    }

    HE::Bool UIModule::_VRelease()
    {
        return TRUE;
    }
}  // namespace UI
