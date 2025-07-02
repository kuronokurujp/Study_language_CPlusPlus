#include <gtest/gtest.h>

#include <array>
#include <string>

#include "Engine/MiniEngine.h"

namespace Core
{
    // 関数システムのテストコードを実装する
    // 関数ヘルパー構造体の前方宣言
    template <class>
    struct FuncHelper;

    // 通常のラムダ式(constなメンバ関数)の関数ヘルパー
    template <class TResult, class TClass, class... TArgs>
    struct FuncHelper<TResult (TClass::*)(TArgs...) const>
    {
        static std::array<std::string, 1 + sizeof...(TArgs)> signatureNames()
        {
            return {typeid(TResult).name(), typeid(TArgs).name()...};
        }
    };

    TEST(HobbyEngine_Common, TestLambda01)
    {
        // テストするラムダ式を用意
        auto myLambda = [](HE::Uint32 in_uCommand, HE::Char* in_szText) { return "hello"; };

        // ラムダ式の型
        using MyLambdaType = decltype(myLambda);

        // ラムダ式の型情報を取得して表示
        HE_LOG_LINE(HE_STR_TEXT("MyLambdaType: %s"), typeid(MyLambdaType).name());
    }

    TEST(HobbyEngine_Common, TestLambda02)
    {
        // テストするラムダ式を用意
        auto myLambda = [](HE::Uint32 in_command, HE::Char* in_szText) { return "hello"; };

        // ラムダ式の型
        using MyLambdaType = decltype(myLambda);

        // ラムダ式の型ヘルパー
        using MyLambdaHelper = FuncHelper<decltype(&MyLambdaType::operator())>;

        // シグネチャ情報を出力する
        for (auto name : MyLambdaHelper::signatureNames())
        {
            HE_LOG_LINE(HE_STR_TEXT("%s"), name.c_str());
        }
    }
}  // namespace Core
