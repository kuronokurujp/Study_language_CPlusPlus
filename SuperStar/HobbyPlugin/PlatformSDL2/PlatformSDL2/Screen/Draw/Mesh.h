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
        struct BindMeshLayoutData
        {
            HE::Uint32 _uItemCount = 0;
            HE::Uint32 _uDataSize  = 0;

            BindMeshLayoutData(const HE::Uint32 _uItemCount, const HE::Uint32 _uDataSize)
            {
                this->_uItemCount = _uItemCount;
                this->_uDataSize  = _uDataSize;
            }
        };

        struct BindMeshVertexData
        {
            void* _pVertices;
            HE::Uint32 _uVerticesCount;

            std::initializer_list<BindMeshLayoutData> _aBindLayout;
        };

        struct BindMeshIndexData
        {
            HE::Uint32* _pIndices;
            // HE::Uint32 _uIndSize;
            HE::Uint32 _uIndicesCount;
        };

        void Init();
        void Release();

        // TODO: 描画
        void BindMeshData(const BindMeshVertexData& in_rMeshVertexData,
                          const BindMeshIndexData& in_rMeshIndexData);

#if 0
        void BindFromVerticesAndIndces(void* in_pVertices, const HE::Uint32 in_uVertSize,
                                       const HE::Uint32 _uVerticesCount, void* in_pIndices,
                                       const HE::Uint32 in_uIndSize,
                                       const HE::Uint32 _uIndicesCount);
#endif
        // 描画
        void Draw();

    private:
        void* _pVertexArray = NULL;
        // HE::Uint32 _uVerticesCount = 0;
    };
}  // namespace PlatformSDL2
