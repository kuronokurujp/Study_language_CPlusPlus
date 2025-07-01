#pragma once

#include "Engine/Common/RTTI.h"

// RTTIを付与する基本クラスの宣言にRTTI情報を付与するマクロ
// nameにはクラス型を指定
#define HE_GENERATED_CLASS_BASE_BODY_HEADER(name)              \
public:                                                        \
    static inline const Core::Common::RTTI name##_CLASS_RTTI = \
        Core::Common::RTTI(HE_STR_TEXT(#name));                \
    static const Core::Common::RTTI* StaticRTTI()              \
    {                                                          \
        return &name##_CLASS_RTTI;                             \
    }                                                          \
                                                               \
public:                                                        \
    virtual const Core::Common::RTTI& VGetRTTI()               \
    {                                                          \
        return *(StaticRTTI());                                \
    }

// RTTIを付与した基本クラスを継承したクラスの宣言にRTTI情報を付与するマクロ
// nameにはクラス型を指定
#define HE_GENERATED_CLASS_BODY_HEADER(name, parentName)                     \
public:                                                                      \
    static inline const Core::Common::RTTI name##_CLASS_RTTI =               \
        Core::Common::RTTI(HE_STR_TEXT(#name), parentName## ::StaticRTTI()); \
    static const Core::Common::RTTI* StaticRTTI()                            \
    {                                                                        \
        return &name##_CLASS_RTTI;                                           \
    }                                                                        \
                                                                             \
public:                                                                      \
    const Core::Common::RTTI& VGetRTTI() override                            \
    {                                                                        \
        return *(StaticRTTI());                                              \
    }

#define HE_GENERATED_CHECK_RTTI(rtti, checkRtti) \
    ((rtti).VGetRTTI().DerivesFrom(checkRtti::StaticRTTI()))
