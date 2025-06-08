#pragma once

#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// プラットフォームモジュールを使う
// 主にインプット入力の取得など
#include "ActorModule.h"
#include "LocalizationModule.h"
#include "RenderModule.h"
// イベントモジュールを使ってUI用のイベントを発行
#include "EventModule.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "UI/Builder/UILayoutBuilder.h"
#include "UI/Builder/UILayoutData.h"
#include "UI/Component/Input/UIInputRouter.h"
#include "UI/Component/Widget/UIButton.h"
#include "UI/Component/Widget/UILayer.h"
#include "UI/Component/Widget/UIText.h"
#include "UI/Event.h"
#include "UI/Widget.h"

namespace UI
{
    /// <summary>
    /// UI用の追加モジュール
    /// </summary>
    class UIModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(UIModule);

    public:
        UIModule();

        /// <summary>
        /// UIが扱う拡張インプットの入力名を追加
        /// </summary>
        /// <param name=""></param>
        void AddActiveInputName(const HE::Char*);

        /// <summary>
        /// BuildFileをロード
        /// </summary>
        /// <returns></returns>
        const Core::Common::Handle LoadAssetWithLayoutBuild(const Core::File::Path&);

        /// <summary>
        /// ロードしたBuildFileのアセットを解放
        /// </summary>
        void UnloadAssetWithLayoutBuild(Core::Common::Handle& in_rHandle);

        /// <summary>
        /// アセットデータからレイアウトを新規作成
        /// </summary>
        const Core::Common::Handle NewLayoutByLayotuAsset(
            const Core::Common::Handle& in_rAssetHandle, const HE::Uint32 in_sort,
            const Core::Common::Handle& in_rViewHandle);

        // UIのLayerを作成する
        // 描画ソート機能は未実装
        const Core::Common::Handle NewLayer(const Core::Common::StringBase& in_szrName,
                                            const Core::Math::Vector2& in_rPos,
                                            const Core::Math::Vector2& in_rSize,
                                            const Core::Math::Color in_color,
                                            const HE::Uint32 in_sort,
                                            const Core::Common::Handle in_viewHandle);

        /// <summary>
        /// テキストラベルWidgetを作成
        /// </summary>
        /// <param name="in_bLoc">ローカライズテキスト対象フラグ</param>
        /// <returns></returns>
        const Core::Common::Handle NewLabelWidget(
            const Core::Common::StringBase& in_szrName, const HE::Uint32 in_sort,
            const HE::Char* in_pLocGroupName, const HE::Char* in_szText,
            const HE::Uint32 in_uFontSize, const Core::Math::Rect2& in_rTextRect,
            const HE::Uint32 in_uTextColor, const Core::Common::Handle& in_rViewHandle);

        /// <summary>
        /// ボタンWidget作成
        /// </summary>
        const Core::Common::Handle NewButtonWidget(const Core::Common::StringBase& in_szrName,
                                                   const HE::Uint32 in_sort,
                                                   const Core::Math::Rect2& in_rBtnRect,
                                                   const HE::Uint32 in_uBtnColor,
                                                   const Core::Common::Handle& in_rViewHandle);

        // UIのWidgetを作成する
        // TODO: 描画ソート機能は未実装
        const Core::Common::Handle NewWidget(const Core::Common::StringBase& in_szName,
                                             const HE::Uint32 in_uSort);

        /// <summary>
        /// Widgetを破棄
        /// </summary>
        void DeleteWidget(Core::Common::Handle&);

        // 親Widgetに子Widgetを追加
        HE::Bool AddChildWidget(Core::Common::Handle& in_rParent, Core::Common::Handle& in_rWidget);

        // Widgetにコンポーネント追加
        template <class T, typename... TArgs>
        std::tuple<Core::Common::Handle, T*> AddComponent(
            const Core::Common::Handle& in_rWidgetHandle, const HE::Uint32 in_iUpdateOrder,
            TArgs&&... in_args)
        {
            auto pActor = this->GetWidget(in_rWidgetHandle);
            // アクターにコンポーネント追加
            return pActor->AddComponentByHandleAndComp<T>(in_iUpdateOrder,
                                                          Actor::Component::EPriorty::EPriorty_Main,
                                                          std::forward<TArgs>(in_args)...);
        }

        Widget* GetWidget(const Core::Common::Handle&);

    protected:
        /// <summary>
        /// UIのモジュール初期化
        /// </summary>
        HE::Bool _VStart() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        HE::Bool _VRelease() override final;

        /// <summary>
        /// モジュール前更新
        /// </summary>
        void _VBeforeUpdate(const HE::Float32 in_fDeltaTime) override final;

        /// <summary>
        /// モジュール更新
        /// </summary>
        void _VUpdate(const HE::Float32 in_fDeltaTime) override final;

        /// <summary>
        /// モジュール後更新
        /// </summary>
        void _VLateUpdate(const HE::Float32 in_fDeltaTime) override final;

    private:
        /// <summary>
        /// レベルに紐づけるアクター管理
        /// </summary>
        Actor::ActorManager* _actorManager;

        Core::Common::Handle _eventHandle;
        HE::Uint64 _inputEventListenerHash = 0;

        UI::UIInputRouterStrategy::ActiveInputVector _vActiveInput;
    };

}  // namespace UI
