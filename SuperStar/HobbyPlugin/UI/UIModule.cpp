#include "UIModule.h"

#include "Engine/Common/CustomMap.h"
#include "Engine/Memory/Memory.h"

namespace UI
{
    // クラス内のみで利用する
    namespace Local
    {
        /// <summary>
        /// UIイベント管理の拡張クラス
        /// </summary>
        class UIEventNetworkStrategy final : public Event::EventNetworkStrategyInterface
        {
        public:
            UIEventNetworkStrategy()
                : _ulEventTypeHash(Core::Common::HashName(EVENT_NETWORK_NAME_UIMODULE))
            {
            }

            HE::Bool VIsHash(const HE::Uint64 in_ulHash)
            {
                return (this->_ulEventTypeHash == in_ulHash);
            }

        private:
            HE::Uint64 _ulEventTypeHash = 0;
        };

        static Core::Common::FixedMap<Builder::EAnchor, Core::Math::EAnchor, 3>
            mPosAnthorToRect2Anchor = {{Builder::EAnchor::EAnchor_Left,
                                        Core::Math::EAnchor::EAnchor_Left},
                                       {Builder::EAnchor::EPosAnchor_Center,
                                        Core::Math::EAnchor::EAnchor_Center}};
    }  // namespace Local

    UIModule::UIModule() : ModuleBase(ModuleName())
    {
        // 依存しているモジュールを設定
        // モジュールの依存設定していないと依存したモジュールが使えない
        this->_AppendDependenceModule<AssetManager::AssetManagerModule>();
        this->_AppendDependenceModule<Localization::LocalizationModule>();
        this->_AppendDependenceModule<Render::RenderModule>();
        this->_AppendDependenceModule<Event::EventModule>();
        this->_AppendDependenceModule<Actor::ActorModule>();
    }

    Widget* UIModule::GetWidget(const Core::Common::Handle& in_rHandlePack)
    {
        Actor::Object* pActor = NULL;
        pActor                = this->_actorManager.Get(in_rHandlePack);
        HE_ASSERT(pActor);

        return reinterpret_cast<Widget*>(pActor);
    }

    /// <summary>
    /// UIのモジュール初期化
    /// </summary>
    HE::Bool UIModule::_VStart()
    {
        // TODO: UIイベント専用の制御を追加
        {
            auto pEventModule  = this->GetDependenceModule<Event::EventModule>();
            auto upStrategy    = HE_MAKE_CUSTOM_UNIQUE_PTR((Local::UIEventNetworkStrategy));
            this->_eventHandle = pEventModule->AddNetwork(std::move(upStrategy));
            HE_ASSERT(this->_eventHandle.Null() == FALSE);
        }

        return ACTOR_MANAGER_START(this->_actorManager, 128, 2);
    }

    HE::Bool UIModule::_VRelease()
    {
        // TODO: UIイベント専用の制御を破棄
        {
            auto pEventModule = this->GetDependenceModule<Event::EventModule>();
            pEventModule->RemoveNetwork(this->_eventHandle);
        }

        this->_actorManager.Release();
        return TRUE;
    }

    void UIModule::_VBeforeUpdate(const HE::Float32 in_fDeltaTime)
    {
        this->_actorManager.BeginUpdate(in_fDeltaTime);
    }

    void UIModule::_VUpdate(const HE::Float32 in_fDeltaTime)
    {
        this->_actorManager.Update(in_fDeltaTime);
    }

    void UIModule::_VLateUpdate(const HE::Float32 in_fDeltaTime)
    {
        this->_actorManager.LateUpdate(in_fDeltaTime);
    }

    void UIModule::_QuqueEventByButtonClick(const Core::Common::StringBase& in_rMsg)
    {
        // TODO: ボタンクリックのイベント作成
        auto spEvent = HE_MAKE_CUSTOM_SHARED_PTR((EventButtonClick), in_rMsg);

        // TODO: 作成したイベントをキューに積む
        auto pEventModule = this->GetDependenceModule<Event::EventModule>();
        pEventModule->QueueEvent(spEvent);
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

    const Core::Common::Handle UIModule::NewLayer(const Core::Common::StringBase& in_szrName,
                                                  const Core::Math::Vector2& in_rPos,
                                                  const Core::Math::Vector2& in_rSize,
                                                  const Core::Math::Color in_color,
                                                  const HE::Uint32 in_uSort,
                                                  const Core::Common::Handle in_viewHandle)
    //                                             const Core::Common::Handle in_levelHandle)
    {
        auto handlePack = this->NewWidget(in_szrName, in_uSort);  //, in_levelHandle);

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

    const Core::Common::Handle UIModule::NewLayoutByLayotuAsset(
        const Core::Common::Handle& in_rAssetHandle, const HE::Uint32 in_uSort,
        const Core::Common::Handle& in_rViewHandle)
    {
        HE_ASSERT(in_rAssetHandle.Null() == FALSE);
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
        Core::Common::Handle rootLayerHandle;
        {
            auto pLayerData = &layerNode._data._exData._layer;
            Core::Math::Color c{pLayerData->_style._uColor};

            rootLayerHandle =
                this->NewLayer(Core::Common::FixedString64(layerNode._data._szId),
                               Core::Math::Vector2(pLayerData->_fX, pLayerData->_fY),
                               Core::Math::Vector2(pLayerData->_style._fW, pLayerData->_style._fH),
                               c, in_uSort, in_rViewHandle);
        }

        Core::Common::Handle parentHandle;
        asset.DoSearchHierarchyNode(
            {HE_STR_U8_TEXT("ui"), HE_STR_U8_TEXT("layer")},
            [this, in_rViewHandle, &parentHandle,
             rootLayerHandle](const UI::Builder::Node& in_rWidgetData, HE::Uint32 in_uSort,
                              const HE::Bool in_bRoot)
            {
                if (in_bRoot)
                {
                    parentHandle = rootLayerHandle;
                }

                // auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
                // HE_ASSERT(pLevelModule);

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
                                                      in_rViewHandle);

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
                                                  in_uSort, rect, pStyle->_uColor, in_rViewHandle);

                        // ボタンを押した時のイベントを設定
                        {
                            Actor::Object* pWidget = NULL;
                            pWidget                = this->_actorManager.Get(h);
                            HE_ASSERT(pWidget);

                            auto [btnWidgetHandle, pWidgetCmp] =
                                pWidget->GetComponentHandleAndComponent(
                                    &UI::UIButtonComponent::CLASS_RTTI);
                            HE_ASSERT(btnWidgetHandle.Null() == FALSE);

                            UI::UIButtonComponent* pBtnComp =
                                reinterpret_cast<UI::UIButtonComponent*>(pWidgetCmp);

                            auto handler =
                                HE_MAKE_CUSTOM_UNIQUE_PTR((UI::UIButtonMessageHandlerDefault),
                                                          pNodeData->_szId,
                                                          [this, parentHandle](
                                                              Core::Common::StringBase& in_rMsg)
                                                          {
                                                              // TODO:
                                                              // イベントモジュールにUIイベント発行
                                                              this->_QuqueEventByButtonClick(
                                                                  in_rMsg);
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

    const Core::Common::Handle UIModule::NewLabelWidget(
        const Core::Common::StringBase& in_szrName, const HE::Uint32 in_uSort,
        const HE::Char* in_szLocGroupName, const HE::Char* in_szText, const HE::Uint32 in_uFontSize,
        const Core::Math::Rect2& in_rTextRect, const HE::Uint32 in_uTextColor,
        const Core::Common::Handle& in_rViewHandle)
    {
        auto handlePack = this->NewWidget(in_szrName, in_uSort);

        Actor::Object* pWidget = NULL;

        pWidget = this->_actorManager.Get(handlePack);
        if (pWidget == NULL) return handlePack;

        // ボタンの上に表示するテキストコンポーネント追加と設定

        auto pActor = this->GetWidget(handlePack);

        auto [textHandle, pTextComp] =
            //  アクターにコンポーネント追加
            pActor->AddComponentByHandleAndComp<UI::UITextComponent>(
                in_uSort + 1, Actor::Component::EPriorty::EPriorty_Main,
                [this](const HE::Char* in_szLocGrop, const HE::Char* in_szText)
                {
                    // 言語モジュールがあるなら言語データと紐づいたテキストを返す
                    auto pLocalModule =
                        this->GetDependenceModule<Localization::LocalizationModule>();
                    HE_ASSERT_RETURN_VALUE(HE_STR_EMPTY, pLocalModule);

                    return pLocalModule->Text(HE_STR_TEXT("JP"), in_szLocGrop, in_szText);
                });
        {
            UI::UITextComponent* pText =
                pTextComp;  // pWidget->GetComponent<UI::UITextComponent>(textHandle);
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

    const Core::Common::Handle UIModule::NewButtonWidget(const Core::Common::StringBase& in_szrName,
                                                         const HE::Uint32 in_uSort,
                                                         const Core::Math::Rect2& in_rBtnRect,
                                                         const HE::Uint32 in_uBtnColor,
                                                         const Core::Common::Handle& in_rViewHandle)
    {
        auto widgetHandle = this->NewWidget(in_szrName, in_uSort);

        auto pWidget = this->_actorManager.Get(widgetHandle);
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

    const Core::Common::Handle UIModule::NewWidget(const Core::Common::StringBase& in_szrName,
                                                   const HE::Uint32 in_uSort)
    {
        auto handle = this->_actorManager.Add<Widget>();
        HE_ASSERT(handle.Null() == FALSE);

        return Core::Common::Handle(handle);
    }

    /// <summary>
    /// TODO: Widgetを破棄
    /// </summary>
    void UIModule::DeleteWidget(Core::Common::Handle& in_rPack)
    {
        this->_actorManager.Remove(&in_rPack);
    }

    HE::Bool UIModule::AddChildWidget(Core::Common::Handle& in_rParent,
                                      Core::Common::Handle& in_rWidget)
    {
        // HE_ASSERT(in_rParent._levelHandle == in_rWidget._levelHandle);
        //  auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        //  HE_ASSERT(pLevelModule);

        //        return pLevelModule->GetLevel(in_rParent._levelHandle)
        //            .ChainActor(in_rWidget._widgetHandle, in_rParent._widgetHandle);
        auto pParentActor = this->_actorManager.Get(in_rParent);
        // 子アクターに親アクターを設定する
        return pParentActor->AddChildTask(in_rWidget);
    }

}  // namespace UI
