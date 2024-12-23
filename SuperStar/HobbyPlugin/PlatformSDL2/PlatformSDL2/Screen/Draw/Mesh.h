#pragma once

#include "Engine/MiniEngine.h"

namespace PlatformSDL2
{
    /// <summary>
    /// メッシュ
    /// </summary>
    class Mesh
    {
    public:
        struct LayoutData
        {
            HE::Uint32 _uItemCount = 0;
            HE::Uint32 _uDataSize  = 0;

            LayoutData(const HE::Uint32 _uItemCount, const HE::Uint32 _uDataSize)
            {
                this->_uItemCount = _uItemCount;
                this->_uDataSize  = _uDataSize;
            }
        };

        struct VertexData
        {
            void* _pVertices;
            HE::Uint32 _uVerticesCount;
            HE::Uint32 _uVertSize;

            std::initializer_list<LayoutData> _aBindLayout;
        };

        struct IndexData
        {
            HE::Uint32* _pIndices;
            HE::Uint32 _uIndicesCount;
        };

        void Init();
        void Release();

        // TODO: 描画データを書き込む
        // 描画前に行う
        // バインド後でないとシェーダーにパラメータを渡せない
        void WriteDrawData(const VertexData& in_rMeshVertexData,
                           const IndexData& in_rMeshIndexData);
        void FreeDrawData();

        /// <summary>
        /// 描画モードの設定と要素数を指定できる
        /// バインドデータに依存するのでバインドしたデータとの整合性を考えないといけない
        /// 頂点のみで頂点インデックスは使わない.
        /// 高速描画できる
        /// </summary>
        void DrawByVertexOnly(const HE::Uint32 in_uMode);

        /// <summary>
        /// 頂点と頂点インデックスを使って描画
        /// 頂点インデックスを用意しないとだめ
        /// </summary>
        void DrawByVertexAndIndex(const HE::Uint32 in_uMode);

    private:
        void* _pVertexArray = NULL;
    };
}  // namespace PlatformSDL2
