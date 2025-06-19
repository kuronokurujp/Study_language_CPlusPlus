#include <gtest/gtest.h>

#include "Engine/Common/CustomList.h"

namespace Core
{
    TEST(HobbyEngine_Common, ListPushPop)
    {
        // テスト用、文字クラス
        class CCharacter : public Core::Common::LinkedListNode<CCharacter>
        {
        public:
            CCharacter(HE::Char* letter) { _pLetter = letter; }
            virtual ~CCharacter() {}

            void Print() { HE_LOG(_pLetter); }

        private:
            HE::Char* _pLetter = NULL;
        };

        Core::Common::CustomList<CCharacter> linearlist;
        EXPECT_EQ(linearlist.Empty(), TRUE);

        CCharacter a(HE_STR_TEXT("a"));

        EXPECT_EQ(linearlist.PushBack(a), TRUE);
        EXPECT_NE(linearlist.Empty(), TRUE);
        EXPECT_EQ(linearlist.PopBack(), TRUE);
        EXPECT_EQ(linearlist.Empty(), TRUE);
        EXPECT_EQ(linearlist.PushFront(a), TRUE);
        EXPECT_NE(linearlist.Empty(), TRUE);
        EXPECT_EQ(linearlist.PopFront(), TRUE);
        EXPECT_EQ(linearlist.Empty(), TRUE);
    }

    TEST(HobbyEngine_Common, ListLoop)
    {
        // テスト用、文字クラス
        class CCharacter2 : public Core::Common::LinkedListNode<CCharacter2>
        {
        public:
            CCharacter2(HE::Char letter) { _pLetter = letter; }
            virtual ~CCharacter2() {}

            const HE::Char GetChar() const { return this->_pLetter; }

        private:
            HE::Char _pLetter = 0;
        };

        Core::Common::CustomList<CCharacter2> linearlist;
        EXPECT_EQ(linearlist.Empty(), TRUE);

        CCharacter2 a(HE_STR_TEXT('a'));
        CCharacter2 b(HE_STR_TEXT('b'));
        CCharacter2 c(HE_STR_TEXT('c'));
        CCharacter2 d(HE_STR_TEXT('d'));

        EXPECT_EQ(linearlist.PushBack(a), TRUE);
        EXPECT_EQ(linearlist.PushBack(b), TRUE);
        EXPECT_EQ(linearlist.PushBack(c), TRUE);
        EXPECT_EQ(linearlist.PushBack(d), TRUE);
        EXPECT_NE(linearlist.Empty(), TRUE);

        const HE::Char* pTextCode = HE_STR_TEXT("abcd");
        HE::Uint32 i              = 0;
        for (auto itr = linearlist.BeginItr(); itr != linearlist.EndItr(); ++itr)
        {
            EXPECT_EQ(itr->GetChar(), pTextCode[i]);
            ++i;
        }

        i = 3;
        for (auto itr = linearlist.BeginRevItr(); itr != linearlist.EndRevItr(); ++itr)
        {
            EXPECT_EQ(itr->GetChar(), pTextCode[i]);
            --i;
        }

        while (linearlist.Empty() == FALSE)
        {
            EXPECT_EQ(linearlist.PopBack(), TRUE);
        }

        EXPECT_EQ(linearlist.Empty(), TRUE);
    }
}  // namespace Core
