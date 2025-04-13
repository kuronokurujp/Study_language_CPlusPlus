#include "Command.h"

#include "Engine/Engine.h"
#include "RenderModule.h"

namespace Render
{
    void Command2DTextDraw(const Core::Common::Handle in_renderHandle,
                           const Core::Math::Vector2& in_rPos,
                           const Core::Common::StringBase& in_szrName, const HE::Uint32 in_uSize,
                           const Core::Math::Color in_color, const Core::Math::EAnchor in_eAnchor,
                           const HE::Uint32 in_uSpace)
    {
        HE_ASSERT(in_renderHandle.Null() == FALSE);

        // 必要なコマンド情報を作る
        Command cmd;
        {
            cmd._uType = ECmdType_2DTextDraw;

            Cmd2DTextDraw* pText2D = &cmd._data._2DDrawText;
            static_assert(sizeof(cmd._data._2DDrawText) <= sizeof(cmd._data._ulaWork));

            pText2D->_fX      = in_rPos._fX;
            pText2D->_fY      = in_rPos._fY;
            pText2D->_color   = in_color;
            pText2D->_eAnchor = in_eAnchor;
            pText2D->_uSize   = in_uSize;
            pText2D->_uSpace  = in_uSpace;
            HE_STR_ERRNO e    = HE_STR_COPY_S(pText2D->_szChars, HE_ARRAY_NUM(pText2D->_szChars),
                                             in_szrName.Str(), in_szrName.Capacity());
            HE_ASSERT(HE_STR_SUCCESS(e) && "文字列コピーに失敗");
        }

        auto pModule = HE_ENGINE.ModuleManager().Get<RenderModule>();
        pModule->PushRenderCommand(in_renderHandle, std::move(cmd));
    }

    void Command2DQuadDraw(const Core::Common::Handle in_renderHandle,
                           const Core::Math::Rect2& in_rRect, const Core::Math::Color in_color)
    {
        HE_ASSERT(in_renderHandle.Null() == FALSE);

        // 必要なコマンド情報を作る
        Command cmd;
        {
            cmd._uType             = ECmdType_2DQuadDraw;
            Cmd2DQuadDraw* pRect2D = &cmd._data._2DDrawRect;
            HE_STATIC_ASSERT(sizeof(cmd._data._2DDrawRect) <= sizeof(cmd._data._ulaWork));

            pRect2D->_fLeftX  = in_rRect._fLeft;
            pRect2D->_fLeftY  = in_rRect._fTop;
            pRect2D->_fRightX = in_rRect._fRight;
            pRect2D->_fRightY = in_rRect._fBottom;
            pRect2D->_color   = in_color;
        }

        auto pModule = HE_ENGINE.ModuleManager().Get<RenderModule>();
        pModule->PushRenderCommand(in_renderHandle, std::move(cmd));
    }

    void CommandClsScreen(const Core::Common::Handle in_renderHandle,
                          const Core::Math::Color& in_rColor)
    {
        HE_ASSERT(in_renderHandle.Null() == FALSE);

        // 必要なコマンド情報を作る
        Command cmd;
        {
            cmd._uType               = ECmdType_ClsScreen;
            CmdClsScreen* pClsScreen = &cmd._data._clsScree;
            HE_STATIC_ASSERT(sizeof(cmd._data._clsScree) <= sizeof(cmd._data._ulaWork));
        }
        auto pModule = HE_ENGINE.ModuleManager().Get<RenderModule>();
        pModule->PushRenderCommand(in_renderHandle, std::move(cmd));
    }

    void Command2DCircleDraw(const Core::Common::Handle in_renderHandle,
                             const Core::Math::Vector2& in_rPos,
                             const Core::Math::EAnchor in_eAhchor, const HE::Float32 in_fSize,
                             const Core::Math::Color& in_rColor)
    {
        // 2D円の描画コマンドを作る
        HE_ASSERT(in_renderHandle.Null() == FALSE);

        // 必要なコマンド情報を作る
        Command cmd;
        {
            cmd._uType            = ECmdType_2DCircleDraw;
            Cmd2DCircleDraw* pCmd = &cmd._data._2DDrawCircle;
            HE_STATIC_ASSERT(sizeof(cmd._data._2DDrawCircle) <= sizeof(cmd._data._ulaWork));
            pCmd->_fX      = in_rPos._fX;
            pCmd->_fY      = in_rPos._fY;
            pCmd->_color   = in_rColor;
            pCmd->_fSize   = in_fSize;
            pCmd->_eAnchor = in_eAhchor;
        }

        auto pModule = HE_ENGINE.ModuleManager().Get<RenderModule>();
        pModule->PushRenderCommand(in_renderHandle, std::move(cmd));
    }

    /// <summary>
    /// 三角形の2D描画
    /// </summary>
    void Command2DTriangleDraw(const Core::Common::Handle in_renderHandle,
                               const Core::Math::Vector2& in_rPos,
                               const Core::Math::EAnchor in_eAhchor,
                               const HE::Float32 in_fAngleDegress, const HE::Float32 in_fSize,
                               const Core::Math::Color& in_rColor)
    {
        // 2D円の描画コマンドを作る
        HE_ASSERT(in_renderHandle.Null() == FALSE);

        // 必要なコマンド情報を作る
        Command cmd;
        {
            cmd._uType              = ECmdType_2DTriangleDraw;
            Cmd2DTriangleDraw* pCmd = &cmd._data._2DDrawTriangle;
            HE_STATIC_ASSERT(sizeof(cmd._data._2DDrawCircle) <= sizeof(cmd._data._ulaWork));

            pCmd->_fX            = in_rPos._fX;
            pCmd->_fY            = in_rPos._fY;
            pCmd->_color         = in_rColor;
            pCmd->_fAngleDegrees = in_fAngleDegress;
            pCmd->_fSize         = in_fSize;
            pCmd->_eAnchor       = in_eAhchor;
        }

        auto pModule = HE_ENGINE.ModuleManager().Get<RenderModule>();
        pModule->PushRenderCommand(in_renderHandle, std::move(cmd));
    }

    void Command2DParticalDraw(const Core::Common::Handle in_renderHandle,
                               const Core::Common::Handle in_particlaHandle,
                               const Core::Math::Vector2& in_rPos)
    {
        // 実装する
        HE_ASSERT(in_particlaHandle.Null() == FALSE);

        // 必要なコマンド情報を作る
        Command cmd;
        {
            cmd._uType = ECmdType_2DParticalDraw;
            auto* pCmd = &cmd._data._Particle;

            pCmd->handle = in_particlaHandle;
            pCmd->_fX    = in_rPos._fX;
            pCmd->_fY    = in_rPos._fY;
            pCmd->_fZ    = 0.0f;
        }

        auto pModule = HE_ENGINE.ModuleManager().Get<RenderModule>();
        pModule->PushRenderCommand(in_renderHandle, std::move(cmd));
    }

}  // namespace Render
