#pragma once

#include "Engine/Core.h"

namespace Core::Common
{
    /// <summary>
    /// インデックスをハンドルとして持つクラス
    /// </summary>
    class Handle final
    {
    public:
        using ValueType = HE::Uint64;

    public:
        Handle() { this->Clear(); }
        // 値をコピーする
        Handle(Handle& in_rHandle) { this->_ulHandle = in_rHandle._ulHandle; }
        Handle(const Handle& in_rHandle) { this->_ulHandle = in_rHandle._ulHandle; }
        Handle(const HE::Uint64 in_ulHandle) { this->_ulHandle = in_ulHandle; }

        // ムーブセマンティクス
        // コピーと同じにする
        Handle(Handle&& r) HE_NOEXCEPT { this->_ulHandle = r._ulHandle; }

        /// <summary>
        /// 管理するindexを設定
        /// </summary>
        void SetIndex(const HE::Uint32 in_uIndex);
        /// <summary>
        /// 重複しないマジックナンバーを使う側で管理する時に使用
        /// </summary>
        void SetIndex(const HE::Uint32 in_uIndex, const HE::Uint32 in_uMagicNumber);

        inline void Clear() HE_NOEXCEPT { this->_ulHandle = HE::ulInvalidUint64; }
        inline HE::Uint32 Index() const HE_NOEXCEPT { return this->_handleField._uIndex; }
        inline HE::Uint32 Magic() const HE_NOEXCEPT { return this->_handleField._uMagic; }
        inline HE::Bool Null() const HE_NOEXCEPT
        {
            return (this->_ulHandle == HE::ulInvalidUint64);
        }

        operator HE::Uint64() const HE_NOEXCEPT { return this->_ulHandle; }

        HE::Bool operator!=(Handle& r) HE_NOEXCEPT { return (this->_ulHandle != r._ulHandle); }

        HE::Bool operator==(Handle& r) HE_NOEXCEPT { return (this->_ulHandle == r._ulHandle); }

        void operator=(const Handle& r) HE_NOEXCEPT { this->_ulHandle = r._ulHandle; }

        void operator=(Handle&& r) HE_NOEXCEPT { this->_ulHandle = r._ulHandle; }

    private:
        enum
        {
            // インデックスとマジックナンバーの使用ビットフィールドサイズ
            // 今のところ合計64bitまで使用可能
            ESize_MagicBit = 32,
            ESize_IndexBit = 64 - ESize_MagicBit,

            // インデックスとマジックナンバーの最大値
            // ハンドルを扱うことが出来る最大数
            EMax_Magic = (1LL << ESize_MagicBit) - 1,
            EMax_Index = (1LL << ESize_IndexBit) - 1,
        };

        /// <summary>
        /// ハンドルするデータ
        /// </summary>
        union
        {
            struct
            {
                // マジックナンバー
                ValueType _uMagic : ESize_MagicBit;
                // インデックス
                ValueType _uIndex : ESize_IndexBit;
            } _handleField;

            ValueType _ulHandle = HE::ulInvalidUint64;
        };
    };

}  // namespace Core::Common

// TODO: 使わないか確認
// Handleクラスをunordered_mapで利用できるようにするための対応
// 詳細は以下
// https://qiita.com/izmktr/items/8e0fd1b6e37de59a9bd0
namespace std
{
    template <>
    class hash<Core::Common::Handle>
    {
    public:
        size_t operator()(const Core::Common::Handle& p) const
        {
            return p.operator Core::Common::Handle::ValueType();
        }
    };
}  // namespace std

// 空ハンドル
// ハンドルを戻り値とする処理で失敗した時の戻り値に利用
extern const Core::Common::Handle NullHandle;
