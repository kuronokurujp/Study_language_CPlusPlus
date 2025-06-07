#pragma once

// エンジン最小インクルード
#include "Engine/Common/CustomMap.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformInput.h"

namespace EnhancedInput
{
    /// <summary>
    /// 入力タイプ
    /// </summary>
    enum EInputType
    {
        EInputType_Keyboard = 0,
        EInputType_Touch,
    };

    /// <summary>
    /// アクションを入力したデータ
    /// </summary>
    struct InputData
    {
        EInputType eType;

        union Item
        {
            struct Key
            {
                Platform::EKeyboard eKey;
            } keyboard;

            struct Touch
            {
                Platform::EInputMouseType eType;
                HE::Float32 _fX, _fY;
            } touch;
        } item;
    };

    /// <summary>
    /// 入力したデータのベクター型
    /// </summary>
    using InputDataVector = Core::Common::FixedVector<InputData, 16>;

    /// <summary>
    /// 入力データのマップ型
    /// </summary>
    using InputMap = Core::Common::FixedMap<Core::Common::FixedString32, InputDataVector, 16>;

}  // namespace EnhancedInput
