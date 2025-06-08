#pragma once

#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"

namespace Render
{
    // コマンド一覧
    enum ECmdType
    {
        // 画面を指定色でクリア
        ECmdType_ClsScreen = 0,
        // テキスト描画
        ECmdType_2DTextDraw,
        // 矩形描画
        ECmdType_2DQuadDraw,
        // 円の描画
        ECmdType_2DCircleDraw,
        // 三角形の描画
        ECmdType_2DTriangleDraw,
        // パーティクルの描画
        ECmdType_2DParticalDraw
    };

    /// <summary>
    /// 2D画面にテキスト表示
    /// </summary>
    struct Cmd2DTextDraw
    {
        HE::Float32 _fX    = 0.0f;
        HE::Float32 _fY    = 0.0f;
        HE::Uint32 _uSize  = 0;
        HE::Uint32 _uSpace = 0;
        Core::Math::Color _color;
        Core::Math::EAnchor _eAnchor = Core::Math::EAnchor_Left;

        HE::Char _szChars[128] = {0};
    };

    /// <summary>
    /// 2D画面に矩形を表示
    /// </summary>
    struct Cmd2DQuadDraw
    {
        HE::Float32 _fLeftX = 0.0f;
        HE::Float32 _fLeftY = 0.0f;

        HE::Float32 _fRightX = 0.0f;
        HE::Float32 _fRightY = 0.0f;

        Core::Math::Color _color;
    };

    /// <summary>
    /// 画面を指定色でクリア
    /// </summary>
    struct CmdClsScreen
    {
        Core::Math::Color _color;
    };

    /// <summary>
    /// 画面に2D円を描画
    /// </summary>
    struct Cmd2DCircleDraw
    {
        HE::Float32 _fX = 0.0f;
        HE::Float32 _fY = 0.0f;
        Core::Math::Color _color;
        // Point3D _point;
        HE::Float32 _fSize           = 0.0f;
        Core::Math::EAnchor _eAnchor = Core::Math::EAnchor_Left;
    };

    /// <summary>
    /// 画面に2D三角形を描画
    /// </summary>
    struct Cmd2DTriangleDraw
    {
        HE::Float32 _fX = 0.0f;
        HE::Float32 _fY = 0.0f;
        Core::Math::Color _color;

        // Point3D _point;
        HE::Float32 _fSize = 0.0f;
        // 三角形の角度(度)
        HE::Float32 _fAngleDegrees   = 0.0f;
        Core::Math::EAnchor _eAnchor = Core::Math::EAnchor_Left;
    };

    /// <summary>
    /// パーティクルの描画
    /// </summary>
    struct CmdParticleDraw
    {
        Core::Common::Handle::ValueType handle = 0;
        HE::Float32 _fX, _fY, _fZ;
    };

    /// <summary>
    /// 描画コマンド
    /// 各コマンドはこれをキャストしてデータ設定をする
    /// TODO:
    /// これをカスタムしてコマンドを渡された側でデータをキャストしてレンダリングできないだろうか？
    /// </summary>
    struct Command
    {
        HE::Uint32 _uType = 0;
        union
        {
            // バッファ
            HE::Uint64 _ulaWork[128] = {};

            // TODO: バッファ超えていた場合はコンパイルエラーを出すことはできるのだろうか？
            CmdClsScreen _clsScree;

            Cmd2DTextDraw _2DDrawText;
            Cmd2DQuadDraw _2DDrawRect;

            Cmd2DCircleDraw _2DDrawCircle;
            Cmd2DTriangleDraw _2DDrawTriangle;
            CmdParticleDraw _Particle;

        } _data;
    };

    /// <summary>
    /// 2Dテキスト描画
    /// </summary>
    extern void Command2DTextDraw(const Core::Common::Handle in_renderHandle,
                                  const Core::Math::Vector2& in_rPos,
                                  const Core::Common::StringBase& in_str, const HE::Uint32 in_uSize,
                                  const Core::Math::Color in_color,
                                  const Core::Math::EAnchor in_eAnchor, const HE::Uint32 in_uSpace);

    /// <summary>
    /// 2D矩形描画
    /// </summary>
    extern void Command2DQuadDraw(const Core::Common::Handle in_renderHandle,
                                  const Core::Math::Rect2& in_rRect,
                                  const Core::Math::Color in_color);
    /// <summary>
    /// 画面クリア
    /// </summary>
    extern void CommandClsScreen(const Core::Common::Handle in_renderHandle,
                                 const Core::Math::Color& in_rColor);

    /// <summary>
    /// 2D円の描画
    /// </summary>
    extern void Command2DCircleDraw(const Core::Common::Handle in_renderHandle,
                                    const Core::Math::Vector2& in_rPos,
                                    const Core::Math::EAnchor in_eAhchor,
                                    const HE::Float32 in_fSize, const Core::Math::Color& in_rColor);

    /// <summary>
    /// 2D三角形の描画
    /// </summary>
    extern void Command2DTriangleDraw(const Core::Common::Handle in_renderHandle,
                                      const Core::Math::Vector2& in_rPos,
                                      const Core::Math::EAnchor in_eAhchor,
                                      const HE::Float32 in_fAngleDegress,
                                      const HE::Float32 in_fSize,
                                      const Core::Math::Color& in_rColor);

    /// <summary>
    /// パーティクル描画
    /// </summary>
    extern void Command2DParticalDraw(const Core::Common::Handle in_renderHandle,
                                      const Core::Common::Handle in_particlaHandle,
                                      const Core::Math::Vector2& in_rPos);

}  // namespace Render
