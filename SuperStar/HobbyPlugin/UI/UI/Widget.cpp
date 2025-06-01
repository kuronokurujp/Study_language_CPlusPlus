#include "Widget.h"

#include "Component/Input/UIInputRouter.h"

namespace UI
{
    Widget::Widget(const Core::Common::StringBase& in_szrName) : Actor::Object()
    {
        this->_szName = in_szrName;
    }

    HE::Bool Widget::VBegin()
    {
        if (Actor::Object::VBegin() == FALSE) return FALSE;

        return TRUE;
    }

    HE::Bool Widget::VEnd()
    {
        if (Actor::Object::VEnd() == FALSE) return FALSE;

        return TRUE;
    }

}  // namespace UI
