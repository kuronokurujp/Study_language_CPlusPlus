#pragma once

#include "Engine/MiniEngine.h"

// 外部モジュール
#include "ActorModule.h"

namespace UI
{
    /// <summary>
    /// UIの基本Widget
    /// </summary>
    class Widget : public Actor::Object
    {
        HE_CLASS_COPY_NG(Widget);
        HE_CLASS_MOVE_NG(Widget);

    public:
        Widget(const Core::Common::StringBase& in_szrName);  // : Actor::Object();
        virtual ~Widget() = default;

        const HE::Char* Name() const { return this->_szName.Str(); }

        /// <summary>
        /// 開始
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        virtual HE::Bool VBegin() override;

        /// <summary>
        /// 終了
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        virtual HE::Bool VEnd() override;

    private:
        Core::Common::FixedString128 _szName;
    };
}  // namespace UI
