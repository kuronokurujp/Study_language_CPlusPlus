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
        Handle() { this->Clear(); }
        // 値をコピーする
        Handle(Handle& in_rHandle) { this->_ulHandle = in_rHandle._ulHandle; }
        Handle(const Handle& in_rHandle) { this->_ulHandle = in_rHandle._ulHandle; }
        Handle(const Uint64 in_ulHandle) { this->_ulHandle = in_ulHandle; }

        // ムーブセマンティクス
        // コピーと同じにする
        Handle(Handle&& r) HE_NOEXCEPT { this->_ulHandle = r._ulHandle; }

        /// <summary>
        /// 管理するindexを設定
        /// </summary>
        void Set(const Uint32 in_uIndex);

        inline void Clear() HE_NOEXCEPT { this->_ulHandle = ulInvalidUint64; }
        inline Uint32 Index() const HE_NOEXCEPT { return this->_handleField._uIndex; }
        inline Uint32 Magic() const HE_NOEXCEPT { return this->_handleField._uMagic; }
        inline Bool Null() const HE_NOEXCEPT { return (this->_ulHandle == ulInvalidUint64); }

        operator Uint64() const HE_NOEXCEPT { return this->_ulHandle; }

        Bool operator!=(Handle& r) HE_NOEXCEPT { return (this->_ulHandle != r._ulHandle); }

        Bool operator==(Handle& r) HE_NOEXCEPT { return (this->_ulHandle == r._ulHandle); }

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
                Uint64 _uMagic : ESize_MagicBit;
                // インデックス
                Uint64 _uIndex : ESize_IndexBit;
            } _handleField;

            Uint64 _ulHandle = ulInvalidUint64;
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
        size_t operator()(const Core::Common::Handle& p) const { return p.operator Uint64(); }
    };
}  // namespace std

// 空ハンドル
// ハンドルを戻り値とする処理で失敗した時の戻り値に利用
extern const Core::Common::Handle NullHandle;
