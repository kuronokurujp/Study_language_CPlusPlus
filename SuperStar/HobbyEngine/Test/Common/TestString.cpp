#include <gtest/gtest.h>

#include "Engine/Common/CustomString.h"

namespace Core
{
    TEST(HobbyEngine_Common, FixStringUse)
    {
        Core::Common::FixedString<32> str(HE_STR_TEXT("test"));
        // テキストが存在するか
        EXPECT_NE(str.Empty(), TRUE);

        // 意図したテキストが存在するか
        EXPECT_EQ(str, HE_STR_TEXT("test"));

        // 文字数が意図通りか
        EXPECT_EQ(str.Length(), 4);

        // 文字位置判定が意図通りか
        EXPECT_EQ(str.Find(HE_STR_TEXT("t")), 0);
        EXPECT_EQ(str.Find(HE_STR_TEXT("s")), 2);
        EXPECT_EQ(str.Find(HE_STR_TEXT("2")), str.Size());

        // 日本語が使えるか
        Core::Common::FixedString<128> str2(HE_STR_TEXT("日本語"));
        EXPECT_EQ(str2, HE_STR_TEXT("日本語"));

        // 文字数が意図通りか
        EXPECT_EQ(str2.Length(), 3);

        // 文字位置判定が意図通りか
        EXPECT_EQ(str2.Find(HE_STR_TEXT("日")), 0);

        Core::Common::FixedString<128> str3(HE_STR_TEXT(
            "このたびは誠にありがとうございます。今後ともどうかよろしくお願いいたします。"));
        EXPECT_EQ(str3.Length(), 38);
    }

    TEST(HobbyEngine_Common, FixStringOutputUTF8)
    {
        // Char型の文字列を作る
        Core::Common::FixedString128 szStr(HE_STR_TEXT("日本語"));
        HE::UTF8 szUTF8[32] = HE_STR_EMPTY;

        // UTF8型に変える
        szStr.OutputUTF8(szUTF8, 32);

        // UTF8型のをChar型に変えて
        Core::Common::FixedString32 szStr32 = szUTF8;

        // 正しい文字列になっているかチェック
        EXPECT_EQ(szStr32.Length(), 3);

        // 文字位置判定が意図通りか
        EXPECT_EQ(szStr32.Find(HE_STR_TEXT("日")), 0);
    }

    TEST(HobbyEngine_Common, FixStringIterator)
    {
        {
            Core::Common::FixedString128 s(HE_STR_TEXT("test"));

            auto itrEnd = s.End();
            for (auto itr = s.Begin(); itr != itrEnd; ++itr)
            {
                HE_LOG(*itr);
            }
        }

        {
            Core::Common::FixedString128 s(HE_STR_TEXT("テストtestだ。"));

            auto itrEnd = s.End();
            for (auto itr = s.Begin(); itr != itrEnd; ++itr)
            {
                HE_LOG(*itr);
            }
        }

        {
            Core::Common::FixedString128 s(HE_STR_TEXT("日本語のtestだ。"));

            auto itrEnd = s.End();
            for (auto itr = s.Begin(); itr != itrEnd; ++itr)
            {
                HE_LOG(*itr);
            }
        }
    }
}  // namespace Core
