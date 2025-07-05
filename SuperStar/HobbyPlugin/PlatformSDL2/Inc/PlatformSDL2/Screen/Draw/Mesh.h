#pragma once

#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"

namespace PlatformSDL2
{
    struct MeshLayoutData
    {
        HE::Uint32 _uItemCount = 0;
        HE::Uint32 _uDataSize  = 0;

        MeshLayoutData(const HE::Uint32 _uItemCount, const HE::Uint32 _uDataSize)
        {
            this->_uItemCount = _uItemCount;
            this->_uDataSize  = _uDataSize;
        }
    };

    struct MeshVertexData
    {
        void* _pVertices           = NULL;
        HE::Uint32 _uVerticesCount = 0;
        HE::Uint32 _uVertSize      = 0;

        std::initializer_list<MeshLayoutData> _aBindLayout;
    };

    struct MeshIndexData
    {
        HE::Uint32* _pIndices     = NULL;
        HE::Uint32 _uIndicesCount = 0;
    };

    /// <summary>
    /// メッシュ
    /// </summary>
    class Mesh
    {
    public:
        void Init();
        void Release();

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

        // 描画データを書き込む
        // 描画前に行う
        // この処理をした後でないとシェーダーにパラメータを渡せない
        void Write(const MeshVertexData& in_rMeshVertexData,
                   const MeshIndexData& in_rMeshIndexData);
        /// <summary>
        /// 書き込んだ描画データを解放
        /// </summary>
        void Free();

    private:
        void* _pMeshLayout = NULL;
    };

    // パーティクルのメッシュ生成
    class ParticleMesh
    {
    public:
        struct Item
        {
            // 位置
            Core::Math::Vector3 position;

            // 速度
            Core::Math::Vector3 velocity;

            // 色
            // Core::Math::Color color;
        };

    public:
        ParticleMesh(const HE::Uint32 in_uCount);

        void Init();
        void Release();

        void Draw();

        void SetPositions(const Core::Common::ArrayBase<Core::Math::Vector3>&);
        void SetVelocitys(const Core::Common::ArrayBase<Core::Math::Vector3>&);

    private:
        void* _pMeshLayout = NULL;
        Item* _pItems      = NULL;
        HE::Uint32 _uCount = 0;
        HE::Bool _bUpdate  = FALSE;
    };

}  // namespace PlatformSDL2
