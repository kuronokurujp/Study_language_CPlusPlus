#pragma once

#include "Engine/Core.h"

namespace Core::Common
{
    /// <summary>
    /// Vectorの基本クラス
    /// </summary>
    template <typename TType>
    class VectorBase
    {
        HE_CLASS_DEFAULT_CONSTRUCT_NG(VectorBase);
        HE_CLASS_COPY_CONSTRUCT_NG(VectorBase);
        HE_CLASS_MOVE_CONSTRUCT_NG(VectorBase);

    public:
        VectorBase(TType* in_tpArrayAddr, HE::Uint32 in_uSize)
            : _pBuff(in_tpArrayAddr), _uCapacity(in_uSize)
        {
        }

        virtual ~VectorBase() { this->Clear(); }

        inline HE::Uint32 Capacity() const HE_NOEXCEPT { return this->_uCapacity; }
        inline HE::Uint32 Size() const HE_NOEXCEPT { return this->_uSize; }
        inline HE::Bool Empty() const HE_NOEXCEPT { return (this->_uSize <= 0); }

        void Clear() HE_NOEXCEPT { this->_uSize = 0; }

        /// <summary>
        /// 配列の末尾に要素を追加
        /// クラスや構造体だとコピー処理が走るのでコピー処理ができるクラスのみ使える
        /// </summary>
        TType& PushBack(const TType& in_rData)
        {
            HE_ASSERT(this->_uSize < this->Capacity());
            // コピー処理が発生
            this->_pBuff[this->_uSize] = in_rData;
            ++this->_uSize;

            return this->_pBuff[this->_uSize - 1];
        }

        /// <summary>
        /// 配列の先頭に要素を追加
        /// </summary>
        TType& PushFront(const TType& in_rData)
        {
            HE_ASSERT(this->_uSize < this->Capacity());

            if (0 < this->_uSize)
            {
                // 配列先頭からのデータを一つ横にずらしてインデックス0に空きを作る
                HE::Uint32 _uSize = this->_uSize * sizeof(TType);
                ::memmove(&this->_pBuff[1], &this->_pBuff[0], _uSize);
            }

            // コピー処理が発生
            this->_pBuff[0] = in_rData;

            ++this->_uSize;

            return this->_pBuff[0];
        }

        TType* PopBack()
        {
            const HE::Uint32 uPopItemIndex = this->_uSize - 1;
            if (0 < this->_uSize)
            {
                --this->_uSize;
                return &this->_pBuff[uPopItemIndex];
            }

            return NULL;
        }

        /// <summary>
        /// 指定インデックスにデータ挿入
        /// 挿入するとデータの配置が変化するので使う側はインデックスを取得するのはNG
        /// </summary>
        TType& Insert(const HE::Uint32 in_uIndex, const TType& in_rData)
        {
            HE_ASSERT(this->_uSize < this->Capacity());

            // データ末尾位置より後ろに挿入ならPushでいい
            if (this->_uSize <= in_uIndex) return this->PushBack(in_rData);

            // データ末尾位置より前に挿入
            // 配置したデータを一つ後ろにずらす
            const HE::Uint32 _uSize = (this->_uSize - in_uIndex) * sizeof(TType);
            ::memmove(&this->_pBuff[in_uIndex + 1], &this->_pBuff[in_uIndex], _uSize);

            // 後ろに一つずらした事で挿入先が空いたので挿入
            this->_pBuff[in_uIndex] = in_rData;
            ++this->_uSize;

            return this->_pBuff[in_uIndex];
        }

        /// <summary>
        /// 引数の要素と一致したのを削除
        /// クラスの場合だと==の定義が必要
        /// </summary>
        HE::Bool Remove(const TType& in_rData)
        {
            HE::Bool bRet = FALSE;
            // 要素が重複している可能性があるので全要素チェック
            for (HE::Uint32 i = 0; i < this->_uSize; ++i)
            {
                if (this->_pBuff[i] == in_rData)
                {
                    this->_RemoveAt(i);
                    bRet = TRUE;
                }
            }

            return bRet;
        }

        /// <summary>
        /// プッシュした要素にアクセスできる
        /// アクセスするには通常こちらを使う
        /// </summary>
        TType& operator[](const HE::Uint32 in_uIndex) const
        {
            HE_ASSERT(0 < this->_uSize);
            return this->_pBuff[in_uIndex];
        }

        void operator=(VectorBase& in_vrData) { this->_DeepCopy(in_vrData); }
        void operator=(const VectorBase& in_vrData) { this->_DeepCopy(in_vrData); }

        /// <summary>
        /// プッシュしていない要素にもアクセスできる特殊メソッド
        /// </summary>
        TType* GetPtr(const HE::Uint32 in_uIndex) const
        {
            HE_ASSERT(0 < this->_uCapacity);
            return &this->_pBuff[in_uIndex];
        }

    protected:
        void _DeepCopy(const VectorBase& in_vrData)
        {
            if (in_vrData._uSize <= 0) return;

            const HE::Uint32 uNewSize = HE_MIN(this->_uCapacity, in_vrData._uSize);
            std::copy(in_vrData._pBuff, in_vrData._pBuff + uNewSize, this->_pBuff);
            this->_uSize = uNewSize;
        }

        /// <summary>
        /// 指定インデックス削除
        /// Vectorはデータを挿入するとデータ位置が変わってしまう
        /// 使う側がインデックスを保存して保存したインデックスを使って削除すると消したくないデータが削除される可能性がある
        /// このようなエラーを出来るだけ起こさないようにできるだけインデックスを使わせない作りにしている
        /// しかし利用したいケースもあるので,
        /// その場合はクラスを継承して呼び出すようにするのがいいかも
        /// </summary>
        void _RemoveAt(const HE::Uint32 in_uIndex)
        {
            HE_ASSERT(in_uIndex < this->_uSize);
            HE_ASSERT(0 < this->_uSize);

            const HE::Uint32 uLastIndex = this->_uSize - 1;
            if (in_uIndex < uLastIndex)
            {
                // 削除する要素位置に上書きして削除
                // メモリ移動のみで高速削除できる
                const HE::Uint32 _uSize = (uLastIndex - in_uIndex) * sizeof(TType);
                ::memmove(&this->_pBuff[in_uIndex], &this->_pBuff[in_uIndex + 1], _uSize);
            }
            else
            {
                // 末尾の削除は要素数を減らすだけでよい
            }

            --this->_uSize;
        }

    protected:
        HE::Uint32 _uSize = 0;

    private:
        TType* _pBuff         = NULL;
        HE::Uint32 _uCapacity = 0;
    };

    /// <summary>
    /// 固定長のカスタムVector
    /// テンプレートで要素を決めている
    /// </summary>
    template <typename TType, HE::Uint32 TCapacity>
    class FixedVector : public VectorBase<TType>
    {
        HE_CLASS_MOVE_NG(FixedVector);

    public:
        FixedVector() : VectorBase<TType>(this->_aBuff, TCapacity) {}
        FixedVector(FixedVector& in_rSrc) : VectorBase<TType>(this->_aBuff, TCapacity)
        {
            this->_DeepCopy(in_rSrc);
        }

        FixedVector(const FixedVector& in_rSrc) : VectorBase<TType>(this->_aBuff, TCapacity)
        {
            this->_DeepCopy(in_rSrc);
        }

        // コンストラクタ
        // 宣言と同時に初期化できるようにしている
        FixedVector(const std::initializer_list<TType>& in_rInitList)
            : VectorBase<TType>(this->_aBuff, TCapacity)
        {
            if (in_rInitList.size() <= 0) return;

            const HE::Uint32 _uSize = HE_MIN(TCapacity, in_rInitList.size());

            auto it = in_rInitList.begin();
            for (HE::Uint32 i = 0; i < _uSize; ++i)
            {
                this->_aBuff[i] = *it;
                ++it;
            }

            this->_uSize = _uSize;
        }

        virtual ~FixedVector() = default;

        void operator=(FixedVector& in_vrData) { this->_DeepCopy(in_vrData); }
        void operator=(const FixedVector& in_vrData) { this->_DeepCopy(in_vrData); }

    private:
        TType _aBuff[TCapacity];
    };

    // テンプレートクラス FixedVector の部分的な型特性
    template <typename T>
    struct IsFixedVector : std::false_type
    {
    };

    // FixedVector のインスタンスに対する特殊化
    template <typename TType, HE::Uint32 TCapacity>
    struct IsFixedVector<FixedVector<TType, TCapacity>> : std::true_type
    {
    };

}  // namespace Core::Common
