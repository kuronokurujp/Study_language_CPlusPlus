#pragma once

#include <functional>

#include "Engine/Module/Module.h"

namespace Render
{
    /// <summary>
    /// 描画モジュール
    /// </summary>
    class RenderModule final : public Module::ModuleBase
    {
        HE_CLASS_COPY_NG(RenderModule);
        HE_CLASS_MOVE_NG(RenderModule);
        HE_MODULE_GENRATE_DECLARATION(RenderModule);

    public:
        RenderModule() = default;
    };
}  // namespace Render
