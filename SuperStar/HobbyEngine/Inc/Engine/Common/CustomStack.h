#pragma once

#include "Engine/Core.h"

namespace Core::Common
{
    /// <summary>
    /// 固定長スタックの基本クラス
    /// </summary>
    template <class TType>
    class StackBase
    {
        HE_CLASS_COPY_NG(StackBase);
        HE_CLASS_MOVE_NG(StackBase);

    public:
        StackBase(TType* in_tpArrayAddr, HE::Uint32 in_uSize)
            : _pBuff(in_tpArrayAddr), _uCapacity(in_uSize)
        {
        }

        virtual ~StackBase() { this->Clear(); }

        inline HE::Uint32 Capacity() const HE_NOEXCEPT { return this->_uCapacity; }
        inline HE::Uint32 Size() const HE_NOEXCEPT { return this->_uNum; }
        inline HE::Bool Empty() const HE_NOEXCEPT { return (this->_uNum <= 0); }

        void Clear() HE_NOEXCEPT { this->_uNum = 0; }

        void PushBack(const TType& in_rData)
        {
            HE_ASSERT(this->_uNum < this->Capacity());
            this->_pBuff[this->_uNum] = in_rData;
            ++this->_uNum;
        }

        /// <summary>
        /// データをプッシュで確保して利用側がデータ設定
        /// </summary>
        TType* PushBack()
        {
            HE_ASSERT(this->_uNum < this->Capacity());
            ++this->_uNum;

            return &this->_pBuff[(this->_uNum - 1)];
        }

        /// <summary>
        /// 配列の先頭に要素を追加
        /// </summary>
        TType* PushFront()
        {
            HE_ASSERT(this->_uNum < this->Capacity());

            if (0 < this->_uNum)
            {
                // 配列先頭からのデータを一つ横にずらしてインデックス0に空きを作る
                HE::Uint32 uMemMoveSize = this->_uNum * sizeof(TType);
                ::memmove(&this->_pBuff[1], &this->_pBuff[0], uMemMoveSize);
            }

            ++this->_uNum;

            return &this->_pBuff[0];
        }

        TType& PopBack()
        {
            HE::Sint32 i = this->_uNum - 1;
            if (0 < this->_uNum) --this->_uNum;
            HE_ASSERT(0 <= i);

            i = HE_MAX(i, 0);
            return this->_pBuff[i];
        }

    private:
        TType* _pBuff         = NULL;
        HE::Uint32 _uNum      = 0;
        HE::Uint32 _uCapacity = 0;
    };

    /// <summary>
    /// 固定長の高速処理のスタック
    /// テンプレートで要素を決めている
    /// </summary>
    template <class TType, HE::Uint32 TCapacity>
    class FixedStack final : public StackBase<TType>
    {
        HE_CLASS_COPY_NG(FixedStack);
        HE_CLASS_MOVE_NG(FixedStack);

    public:
        FixedStack() : StackBase<TType>(_taBuff, TCapacity) {}

    private:
        TType _taBuff[TCapacity];
    };

    // テンプレートクラス FixedStack の部分的な型特性
    template <typename T>
    struct IsFixedStack : std::false_type
    {
    };

    // FixedVector のインスタンスに対する特殊化
    template <typename TType, HE::Uint32 TCapacity>
    struct IsFixedStack<FixedStack<TType, TCapacity>> : std::true_type
    {
    };

}  // namespace Core::Common
