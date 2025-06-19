#pragma once

#include "Engine/Core.h"

/// <summary>
/// 配列制御クラス
/// 配列制御をクラスで行う事ができる
/// Uint32 nums[32]
/// 上記のC++標準の配列を使うのではなく配列制御クラスを使って配列を扱うようにしてほしい
/// Uint32 nums[32]
/// ↓
/// CustomArray<Uint32, 32> nums;
/// </summary>

namespace Core::Common
{
    /// <summary>
    /// 固定長配列の基本クラス
    /// </summary>
    template <class TType>
    class ArrayBase
    {
        HE_CLASS_COPY_NG(ArrayBase);
        HE_CLASS_MOVE_NG(ArrayBase);

    public:
        explicit ArrayBase(TType* in_pArrayAddr, const HE::Uint32 in_uSize)
            : _pBuff(in_pArrayAddr), _uCapacity(in_uSize)
        {
        }

        inline HE::Uint32 Capacity() const HE_NOEXCEPT { return this->_uCapacity; }

        /// <summary>
        /// 指定した要素に値をコピーして設定
        /// </summary>
        inline void Set(const HE::Uint32 in_uIndex, const TType& in_data) HE_NOEXCEPT
        {
            HE_ASSERT(in_uIndex < this->_uCapacity);
            this->_pBuff[in_uIndex] = in_data;
        }

        inline void Set(const HE::Uint32 in_uIndex, TType&& in_data) HE_NOEXCEPT
        {
            HE_ASSERT(in_uIndex < this->_uCapacity);
            this->_pBuff[in_uIndex] = std::move(in_data);
        }

        /// <summary>
        /// 配列をコピー
        /// </summary>
        void DeepCopy(const ArrayBase& in_arArray)
        {
            HE_ASSERT(0 < this->_uCapacity);
            HE_ASSERT(0 < in_arArray._uCapacity);

            const HE::Uint32 uMinCapacity = HE_MIN(this->_uCapacity, in_arArray._uCapacity);
            std::copy(in_arArray._pBuff, in_arArray._pBuff + uMinCapacity, this->_pBuff);
        }

        TType& operator[](const HE::Uint32 in_uIndex) const
        {
            HE_ASSERT(0 < this->_uCapacity);
            return this->_pBuff[in_uIndex];
        }

    private:
        TType* _pBuff         = NULL;
        HE::Uint32 _uCapacity = 0;
    };

    /// <summary>
    /// 固定長配列
    /// テンプレートで要素を決めている
    /// </summary>
    template <typename TType, HE::Uint32 TCapacity>
    class FixedArray final : public ArrayBase<TType>
    {
    public:
        explicit FixedArray() : ArrayBase<TType>(this->_aBuff, TCapacity) {}

        // コンストラクタ
        // 配列宣言と同時に値を初期化できるようにしている
        FixedArray(std::initializer_list<TType>& in_rInitList)
            : ArrayBase<TType>(this->_aBuff, TCapacity)
        {
            const HE::Uint32 uMinCapacity = HE_MIN(TCapacity, in_rInitList.size());

            auto it = in_rInitList.begin();
            for (HE::Uint32 i = 0; i < uMinCapacity; ++i)
            {
                this->_aBuff[i] = *it;
            }
        }

    private:
        TType _aBuff[TCapacity];
    };
}  // namespace Core::Common
