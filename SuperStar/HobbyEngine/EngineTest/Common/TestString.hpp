#pragma once

#include "Engine/Common/CustomString.h"

TEST_CASE("FixString Use")
{
    Core::Common::FixString<32> str(HE_STR_TEXT("test"));
    // テキストが存在するか
    CHECK_FALSE(str.Empty());

    // 意図したテキストが存在するか
    CHECK(str == HE_STR_TEXT("test"));

    // 文字数が意図通りか
    CHECK(str.Length() == 4);

    // 文字位置判定が意図通りか
    CHECK(str.Find(HE_STR_TEXT("t")) == 0);
    CHECK(str.Find(HE_STR_TEXT("s")) == 2);

    // 日本語が使えるか
    Core::Common::FixString<128> str2(HE_STR_TEXT("日本語"));
    CHECK(str2 == HE_STR_TEXT("日本語"));

    // 文字数が意図通りか
    CHECK(str2.Length() == 3);

    // 文字位置判定が意図通りか
    CHECK(str2.Find(HE_STR_TEXT("日")) == 0);
}

TEST_CASE("FixString OutputUTF8")
{
    // Char型の文字列を作る
    Core::Common::FixString128 szStr(HE_STR_TEXT("日本語"));
    UTF8 szUTF8[32] = {NULL};

    // UTF8型に変える
    szStr.OutputUTF8(szUTF8, 32);

    // UTF8型のをChar型に変えて
    Core::Common::FixString32 szStr32 = szUTF8;

    // 正しい文字列になっているかチェック
    CHECK(szStr32.Length() == 3);

    // 文字位置判定が意図通りか
    CHECK(szStr32.Find(HE_STR_TEXT("日")) == 0);
}
