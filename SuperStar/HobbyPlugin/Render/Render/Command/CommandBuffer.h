#pragma once

#include "Engine/Common/CustomVector.h"
#include "Render/Command/Command.h"

namespace Render
{
    /// <summary>
    /// コマンドバッファ
    /// </summary>
    class CommandBuffer final : public Core::Common::FixedVector<Command, 2048>
    {
    };
}  // namespace Render
