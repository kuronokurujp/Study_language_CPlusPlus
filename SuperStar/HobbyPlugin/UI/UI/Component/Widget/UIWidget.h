#pragma once

#include "ActorModule.h"
#include "Engine/Math/Math.h"
#include "Engine/Platform/PlatformInput.h"
#include "EnhancedInputModule.h"

namespace UI
{
    /// <summary>
    /// UIのWidgetの基本コンポーネント
    /// Widgetコンポーネントは必ずこのクラスを継承する
    /// </summary>
    class UIWidgetComponent : public Actor::Component
    {
        HE_CLASS_COPY_NG(UIWidgetComponent);
        HE_GENERATED_CLASS_BODY_HEADER(UIWidgetComponent, Actor::Component);

    public:
        UIWidgetComponent() : Actor::Component() { this->_Clear(); }
        virtual ~UIWidgetComponent() = default;

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void VSetup(const HE::Bool in_bReleaseMem = TRUE) override;

        /// <summary>
        /// タスク開始
        /// </summary>
        virtual HE::Bool VBegin() override;

        /// <summary>
        /// タスク終了
        /// </summary>
        /// <returns></returns>
        virtual HE::Bool VEnd() override;

        virtual void VOnTouch(const HE::Char* in_szInputName,
                              const EnhancedInput::InputData::Item::Touch&)
        {
        }
        virtual void VOnKey(const HE::Char* in_szInputName,
                            const EnhancedInput::InputData::Item::Key&)
        {
        }

        void SetPos(const Core::Math::Vector2&);
        void SetSize(const Core::Math::Vector2&);
        void SetColor(const HE::Uint32 in_color) { this->_uColor = in_color; }
        void SetViewHandle(const Core::Common::Handle& in_rHandle)
        {
            this->_viewHandle = in_rHandle;
        }

        Actor::TransformComponent* GetTransformComponent();

    protected:
        void _Clear()
        {
            this->_pos.Clear();
            this->_size.Clear();
            this->_uColor = 0;
            this->_viewHandle.Clear();
            this->_transformHandle.Clear();
        }

    protected:
        Core::Math::Vector2 _pos;
        Core::Math::Vector2 _size;
        HE::Uint32 _uColor = 0;
        Core::Common::Handle _viewHandle;
        Core::Common::Handle _transformHandle;
    };
}  // namespace UI
