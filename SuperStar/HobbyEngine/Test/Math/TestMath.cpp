#include <gtest/gtest.h>

#include "Engine/Engine.h"
#include "Engine/Math/Math.h"

namespace Core::Math::RC
{
    TEST(HobbyEngine_Math, TestRect)
    {
        // XYがプラスでのチェック
        {
            Rect2D rect(32, 48, 10, 20);
            EXPECT_EQ(rect.Height(), 20);
            EXPECT_EQ(rect.Width(), 10);

            auto v = rect.GetPos();
            // 中心位置計算が正しいか
            EXPECT_EQ(v._fX, 32);
            EXPECT_EQ(v._fY, 48);

            // 矩形同士が重なっているか
            EXPECT_EQ(rect.IsRectIntersect(rect), TRUE);
            {
                Rect2D rect2(32 + rect._fW, 48, 9, 19);
                EXPECT_EQ(rect.IsRectIntersect(rect2), FALSE);
            }

            // 座標が矩形の中に入っているか
            EXPECT_EQ(rect.IsPointInside(v), TRUE);
            EXPECT_EQ(rect.IsPointInside(
                          Vector2(v._fX + rect.WidthHalf() + 1.0f, v._fY + rect.HeightHalf())),
                      FALSE);
        }

        // XYがマイナスでのチェック
        {
            Rect2D rect(-32, -48, 10, 20);
            EXPECT_EQ(rect.Height(), 20);
            EXPECT_EQ(rect.Width(), 10);

            auto v = rect.GetPos();
            // 中心位置計算が正しいか
            EXPECT_EQ(v._fX, -32);
            EXPECT_EQ(v._fY, -48);

            /// <summary>
            /// 引数の矩形が中にあるかどうか.
            /// </summary>
            EXPECT_EQ(rect.IsRectIntersect(rect), TRUE);
            {
                Rect2D rect2(-32 - rect._fW, -48, 9, 19);
                EXPECT_EQ(rect.IsRectIntersect(rect2), FALSE);
            }

            // 座標が矩形の中に入っているか
            EXPECT_EQ(rect.IsPointInside(v), TRUE);
            EXPECT_EQ(rect.IsPointInside(
                          Vector2(v._fX + rect.WidthHalf() + 1.0f, v._fY + rect.HeightHalf())),
                      FALSE);
        }
    }

}  // namespace Core::Math::RC
