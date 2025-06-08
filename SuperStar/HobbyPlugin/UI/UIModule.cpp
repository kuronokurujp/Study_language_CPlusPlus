#include "UIModule.h"

#include "Engine/Common/CustomMap.h"
#include "Engine/Memory/Memory.h"

namespace UI
{
    // cpp内のみで利用する
    namespace Local
    {
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
        this->_AppendDependenceModule<EnhancedInput::EnhancedInputModule>();
    }

    void UIModule::AddActiveInputName(const HE::Char* in_szName)
    {
        this->_vActiveInput.PushBack(in_szName);
    }

    Widget* UIModule::GetWidget(const Core::Common::Handle& in_rHandlePack)
    {
        Actor::Object* pActor = NULL;
        pActor                = this->_actorManager->Get(in_rHandlePack);
        HE_ASSERT(pActor);

        return reinterpret_cast<Widget*>(pActor);
    }

    /// <summary>
    /// UIのモジュール初期化
    /// </summary>
    HE::Bool UIModule::_VStart()
    {
        // UIWidgetアクター管理を作成
        {
            auto upStg =
                HE_MAKE_CUSTOM_UNIQUE_PTR((EnhancedInput::ActorManagerDecoraterWithInputSystem));
            this->_actorManager = HE_NEW_MEM(Actor::ActorManager, 0)(std::move(upStg));
        }

        // インプットのイベントリスナー登録
        {
            auto pEventModule      = this->GetDependenceModule<Event::EventModule>();
            auto spUIEventListener = HE_MAKE_CUSTOM_SHARED_PTR(
                (Event::EventListenerWithRegistEventFunc), HE_STR_TEXT("UIModule_InputEvent"),
                [this](Event::EventDataInterfacePtr const& in_spEventData)
                {
                    // HE_LOG_LINE(HE_STR_TEXT("%s"), in_spEventData->VEventTypeName());

                    // 入力イベント
                    if (in_spEventData->VEventHash() == EnhancedInput::EventInput::Hash())
                    {
                        auto pEvent =
                            reinterpret_cast<EnhancedInput::EventInput*>(in_spEventData.get());
                        // 入力情報を伝える
                        auto pInputSystem =
                            reinterpret_cast<EnhancedInput::ActorManagerDecoraterWithInputSystem*>(
                                this->_actorManager->GetDecorater());
                        pInputSystem->ProcessInput(pEvent->_mInputAction);
                    }

                    return TRUE;
                });

            // 拡張インプットの方で先に設定してもらわないと失敗した
            this->_inputEventListenerHash =
                pEventModule->AddListener(spUIEventListener, EVENT_TYPE_ENHANCEDINPUT);
        }
        return ACTOR_MANAGER_START((*this->_actorManager), 128, 2);
    }

    HE::Bool UIModule::_VRelease()
    {
        this->_vActiveInput.Clear();
        this->_actorManager->Release();
        HE_SAFE_DELETE_MEM(this->_actorManager);
        return TRUE;
    }

    void UIModule::_VBeforeUpdate(const HE::Float32 in_fDeltaTime)
    {
        this->_actorManager->BeginUpdate(in_fDeltaTime);
    }

    void UIModule::_VUpdate(const HE::Float32 in_fDeltaTime)
    {
        this->_actorManager->Update(in_fDeltaTime);
    }

    void UIModule::_VLateUpdate(const HE::Float32 in_fDeltaTime)
    {
        this->_actorManager->LateUpdate(in_fDeltaTime);
    }

    const Core::Common::Handle UIModule::LoadAssetWithLayoutBuild(
        const Core::File::Path& in_rFilePath)
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
    {
        auto handlePack = this->NewWidget(in_szrName, in_uSort);

        // レイヤーにユーザー入力ルーティングを追加
        // ルーティングに拡張インプットのインプット名の配列を設定する
        auto pInputStrategy =
            HE_MAKE_CUSTOM_SHARED_PTR((UI::UIInputRouterStrategy), this->_vActiveInput);

        auto [hInputRouter, pComp] =
            this->AddComponent<EnhancedInput::InputComponent>(handlePack, 0, pInputStrategy);

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
        // TODO: 生成用のファクトリークラスが必要
        // TODO: ファクトリを切り替えると事で構築するUIを変えるようにしたい
        auto handlePack = this->NewWidget(in_szrName, in_uSort);

        Actor::Object* pWidget = NULL;

        pWidget = this->_actorManager->Get(handlePack);
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

                    // TODO: 言語切り替えを入れる
                    return pLocalModule->Text(HE_STR_TEXT("JP"), in_szLocGrop, in_szText);
                });
        {
            UI::UITextComponent* pText = pTextComp;
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

    const Core::Common::Handle UIModule::NewButtonWidget(const Core::Common::StringBase& in_szName,
                                                         const HE::Uint32 in_uSort,
                                                         const Core::Math::Rect2& in_rBtnRect,
                                                         const HE::Uint32 in_uBtnColor,
                                                         const Core::Common::Handle& in_rViewHandle)
    {
        auto widgetHandle = this->NewWidget(in_szName, in_uSort);

        auto pWidget = this->_actorManager->Get(widgetHandle);
        if (pWidget == NULL) return widgetHandle;

        // ボタンを押した時のハンドラーを生成して押した時のイベント通知する
        auto handler =
            HE_MAKE_CUSTOM_UNIQUE_PTR((UI::UIButtonMessageHandlerDefault),
                                      [this](UIButtonComponent* in_pBtnComp)
                                      {
                                          auto pEventModule =
                                              this->GetDependenceModule<Event::EventModule>();
                                          HE_ASSERT_RETURN(pEventModule);

                                          // ボタンイベントを通知
                                          auto pWidget =
                                              reinterpret_cast<Widget*>(in_pBtnComp->Owner());
                                          HE_ASSERT_RETURN(pWidget);

                                          auto spEvent =
                                              HE_MAKE_CUSTOM_SHARED_PTR((EventButtonClick),
                                                                        pWidget->Name());
                                          // 実行したフレームで通知が必要かも?
                                          pEventModule->QueueEvent(spEvent, EVENT_TYPE_UIMODULE);
                                      });

        // ボタンコンポーネント追加と設定
        auto [hButton, pBtnCmp] =
            this->AddComponent<UIButtonComponent>(widgetHandle, in_uSort, std::move(handler));
        {
            UI::UIButtonComponent* pButton = pWidget->GetComponent<UIButtonComponent>(hButton);
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
        auto handle = this->_actorManager->Add<Widget>(in_szrName);
        HE_ASSERT(handle.Null() == FALSE);

        return Core::Common::Handle(handle);
    }

    /// <summary>
    /// Widgetを破棄
    /// </summary>
    void UIModule::DeleteWidget(Core::Common::Handle& in_rPack)
    {
        this->_actorManager->Remove(&in_rPack);
    }

    HE::Bool UIModule::AddChildWidget(Core::Common::Handle& in_rParent,
                                      Core::Common::Handle& in_rWidget)
    {
        auto pParentActor = this->_actorManager->Get(in_rParent);
        // 子アクターに親アクターを設定する
        return pParentActor->AddChildTask(in_rWidget);
    }

}  // namespace UI
