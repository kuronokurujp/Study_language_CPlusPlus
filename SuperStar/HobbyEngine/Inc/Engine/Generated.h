#pragma once

#include "Engine/Common/RTTI.h"

// RTTIを付与する基本クラスの宣言にRTTI情報を付与するマクロ
// nameにはクラス型を指定
#define HE_GENERATED_CLASS_BASE_BODY_HEADER(name)                                             \
public:                                                                                       \
    static const Core::Common::RTTI* StaticRTTI()                                             \
    {                                                                                         \
        static Core::Common::RTTI name##_CLASS_RTTI = Core::Common::RTTI(HE_STR_TEXT(#name)); \
                                                                                              \
        return &name##_CLASS_RTTI;                                                            \
    }                                                                                         \
                                                                                              \
public:                                                                                       \
    const Core::Common::RTTI& GetRTTI()                                                       \
    {                                                                                         \
        return *(StaticRTTI());                                                               \
    }

// RTTIを付与した基本クラスを継承したクラスの宣言にRTTI情報を付与するマクロ
// nameにはクラス型を指定
#define HE_GENERATED_CLASS_BODY_HEADER(name, parentName)                         \
public:                                                                          \
    static const Core::Common::RTTI* StaticRTTI()                                \
    {                                                                            \
        static Core::Common::RTTI name##_CLASS_RTTI =                            \
            Core::Common::RTTI(HE_STR_TEXT(#name), parentName## ::StaticRTTI()); \
        return &name##_CLASS_RTTI;                                               \
    }                                                                            \
                                                                                 \
public:                                                                          \
    const Core::Common::RTTI& GetRTTI()                                          \
    {                                                                            \
        return *(StaticRTTI());                                                  \
    }

#define HE_GENERATED_CHECK_RTTI(rtti, checkRtti) \
    ((rtti).GetRTTI().DerivesFrom(checkRtti::StaticRTTI()))
