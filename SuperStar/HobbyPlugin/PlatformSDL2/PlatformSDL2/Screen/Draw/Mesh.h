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
            void* _pVertices           = NULL;
            HE::Uint32 _uVerticesCount = 0;
            HE::Uint32 _uVertSize      = 0;

            std::initializer_list<LayoutData> _aBindLayout;
        };

        struct IndexData
        {
            HE::Uint32* _pIndices     = NULL;
            HE::Uint32 _uIndicesCount = 0;
        };

        void Init();
        void Release();

        // 描画データを書き込む
        // 描画前に行う
        // この処理をした後でないとシェーダーにパラメータを渡せない
        void WriteDrawData(const VertexData& in_rMeshVertexData,
                           const IndexData& in_rMeshIndexData);
        /// <summary>
        /// 書き込んだ描画データを解放
        /// </summary>
        void FreeDrawData();

        /// <summary>
        /// 描画モードの設定と要素数を指定できる
        /// 書き込みデータに依存するので書き込みしたデータとの整合性を考えないといけない
        /// 頂点のみで頂点インデックスは使わない.
        /// 高速描画できる
        /// </summary>
        void DrawByVertexOnly(const HE::Uint32 in_uMode, const HE::Uint32 in_uVertCoun = 0);

        /// <summary>
        /// 頂点と頂点インデックスを使って描画
        /// 頂点インデックスを用意しないとだめ
        /// </summary>
        void DrawByVertexAndIndex(const HE::Uint32 in_uMode);

    private:
        void* _pMeshLayout = NULL;
    };
}  // namespace PlatformSDL2
