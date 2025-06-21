#pragma once

#include "Engine/Common/RTTI.h"

// RTTIを付与する基本クラスの宣言にRTTI情報を付与するマクロ
// nameにはクラス型を指定
#define HE_GENERATED_CLASS_BASE_BODY_HEADER(name) \
public: \
    static inline const Core::Common::RTTI kRtti_##name = \
        Core::Common::RTTI(HE_STR_TEXT(#name)); \
    static const Core::Common::RTTI& StaticRTTI() \
    { \
        return kRtti_##name; \
    } \
public: \
    const Core::Common::RTTI& VGetRTTI() \
    { \
        return name::StaticRTTI(); \
    }
#define HE_GENERATED_CLASS_BODY_HEADER(name, parentName) \
public: \
    static inline const Core::Common::RTTI kRtti_##name = \
        Core::Common::RTTI(HE_STR_TEXT(#name), parentName::StaticRTTI()); \
    static const Core::Common::RTTI& StaticRTTI() \
    { \
        return kRtti_##name; \
    } \
public: \
    const Core::Common::RTTI& VGetRTTI() \
    { \
        return name::StaticRTTI(); \
    }
#define HE_GENERATED_CHECK_RTTI(rtti, checkRtti) \
    ((rtti).VGetRTTI().DerivesFrom(&checkRtti::StaticRTTI()))
