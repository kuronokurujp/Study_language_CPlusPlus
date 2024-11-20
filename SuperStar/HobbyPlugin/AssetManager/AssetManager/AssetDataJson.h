#pragma once

#include "AssetDataBase.h"
#include "Engine/Memory/Memory.h"

// TODO: ヘッダーにOSSのヘッダーファイルをインクルードしない方がいい気がする
// 他のプラグインからインクルードするとそのプラグインからもOSSの機能が使えてしまうから
#include "AssetManager/ThirdParty/simidjson/simdjson.h"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

namespace AssetManager
{
    /// <summary>
    /// jsonアセットデータ
    /// </summary>
    class AssetDataJson : public AssetDataBase, public AssetDataWithTreeNodePropertyInterface
    {
        HE_CLASS_COPY_NG(AssetDataJson);
        HE_CLASS_MOVE_NG(AssetDataJson);

    public:
        using OutputJsonValue = simdjson::fallback::ondemand::value;

    public:
        AssetDataJson() : AssetDataBase() {}
        virtual ~AssetDataJson() = default;

        // 指定するノードのトークン名はアルファベットと数値のみなのでUTF8型にした
        virtual Uint32 VGetUInt32(const std::initializer_list<const UTF8*>&) override final;
        virtual Float32 VGetFloat32(const std::initializer_list<const UTF8*>&) override final;
        virtual Core::Common::FixString1024 VGetChar(
            const std::initializer_list<const UTF8*>&) override final;

    protected:
        virtual Bool _VLoad(Platform::FileInterface&) override;
        virtual void _VUnload() override;

/*
        template <typename... Args>
        typename std::enable_if<(std::is_same<Args, const Char*>::value && ...), const Bool>::type
        _OutputValue(OutputJsonValue* out, Args... args)
        {
            HE_ASSERT(out);

            // 引数の個数を取得
            Uint32 uCount = static_cast<Uint32>(sizeof...(args));
            if (uCount <= 0) return FALSE;

            // 初期化リストを使用して引数を処理
            const Char* szaName[] = {args...};
            return this->_OutputValue(out, szaName, uCount);
        }
        */

    private:
    /*
        Bool _OutputValue(simdjson::fallback::ondemand::value* out, const Char* in_szaName[],
                          const Uint32 in_uCount);
                          */

    protected:
        Core::Common::Handle _fileHandle;

        Core::Memory::UniquePtr<simdjson::padded_string> _json;
        Core::Memory::UniquePtr<simdjson::ondemand::parser> _parser;
        void* _pDoc = NULL;
    };
}  // namespace AssetManager
