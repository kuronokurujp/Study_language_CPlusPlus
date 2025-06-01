#pragma once

#include "Engine/Core.h"

namespace Core::Common
{
    /// <summary>
    /// 文字列からハッシュ数を計算
    /// マークアドラーのAdler-32チェックサムをベースにしている。
    /// </summary>
    extern HE::Hash HashName(const HE::Char*);

}  // namespace Core::Common
