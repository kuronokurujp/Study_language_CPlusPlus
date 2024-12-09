#pragma once

#include "Engine/MiniEngine.h"

// 利用モジュール
#include "LevelModule.h"

namespace InGame
{
    /// <summary>
    /// インゲームのシステムコンポーネント
    /// </summary>
    class InGameSystemComponent final : public Level::LevelBaseComponent
    {
        HE_CLASS_COPY_NG(InGameSystemComponent);
        HE_CLASS_MOVE_NG(InGameSystemComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameSystemComponent, Level::LevelBaseComponent);

    public:
        InGameSystemComponent() : Level::LevelBaseComponent() {}

        HE::Bool IsGameEnd() const { return this->_bGameEnd; }
        void SetFlgGameEnd(const HE::Bool in_bFlg) { this->_bGameEnd = in_bFlg; }
        void SetPoint(const HE::Uint32 in_uPoint) { this->_uPoint = in_uPoint; }
        HE::Uint32 GetPoint() const { return this->_uPoint; }
        void AddPoint(const HE::Uint32 in_uAddPoint) { this->_uPoint += in_uAddPoint; }

    private:
        void _Clear();

    private:
        HE::Bool _bGameEnd = FALSE;
        HE::Uint32 _uPoint = 0;
    };

}  // namespace InGame
