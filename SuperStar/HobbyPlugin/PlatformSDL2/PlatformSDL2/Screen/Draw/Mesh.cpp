#include "Mesh.h"

#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    /// <summary>
    /// 2D/UI/3Dなどで色々なパターンで頂点レイアウトが変わるので基本クラスにして継承先で頂点設定とレイアウト作成をする
    /// </summary>
    class VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        void WriteVertexAndLayout(const Mesh::VertexData& in_rMeshVertexData)
        {
            this->_uVertCount = 0;

            HE::Uint32 uVertSize = in_rMeshVertexData._uVertSize;

            // 頂点リストオブジェクトを作成して使用可能にする
            {
                glGenVertexArrays(1, &this->vertexArray);
                glBindVertexArray(this->vertexArray);
            }

            // 頂点情報を作成
            {
                this->_uVertCount = in_rMeshVertexData._uVerticesCount;

                glGenBuffers(1, &this->vertexBuffer);
                // 頂点情報を格納するバッファ生成
                // GL_ARRAY_BUFFERで指定
                glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
                // GenBufferで作成したデータ領域に書き込むサイズを計算
                // 頂点データ構造(x,y,z,u,v) => 3次元座標とUV座標

                int vertexsSize = this->_uVertCount * uVertSize;
                glBufferData(GL_ARRAY_BUFFER, vertexsSize, in_rMeshVertexData._pVertices,
                             GL_STATIC_DRAW);
            }

            // データレイアウト作成
            {
                // xyzの頂点とuv頂点の5つの頂点
                int layoutOffsetPointerSize = 0;

                HE::Uint32 uGroupIdx = 0;
                for (auto itr = in_rMeshVertexData._aBindLayout.begin();
                     itr != in_rMeshVertexData._aBindLayout.end(); ++itr)
                {
                    // こうしないと警告でエラーになる
                    void* pPointer = reinterpret_cast<void*>(
                        static_cast<std::uintptr_t>(layoutOffsetPointerSize));

                    // xyzの頂点データを関連付けする
                    // グループ
                    glEnableVertexAttribArray(uGroupIdx);
                    glVertexAttribPointer(uGroupIdx, itr->_uItemCount, GL_FLOAT, GL_FALSE,
                                          uVertSize, pPointer);

                    // レイアウトのオフセット
                    layoutOffsetPointerSize += itr->_uDataSize;

                    ++uGroupIdx;
                }
            }
        }

        void WriteIndices(const Mesh::IndexData& in_rMeshIndexData)
        {
            this->_uIndicesCount = 0;

            // 頂点並びの添え字を作成
            {
                glGenBuffers(1, &this->indexBuffer);
                // インデックスバッファ生成
                // GL_ELEMENT_ARRAY_BUFFERでインデックスバッファ専用のが作れる
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);

                this->_uIndicesCount = in_rMeshIndexData._uIndicesCount;
                // インデックス情報に書き込むデータサイズ計算
                int indexSize = this->_uIndicesCount * sizeof(in_rMeshIndexData._pIndices[0]);

                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, in_rMeshIndexData._pIndices,
                             GL_STATIC_DRAW);
            }
        }

        void Release()
        {
            glDeleteBuffers(1, &this->vertexBuffer);
            glDeleteBuffers(1, &this->indexBuffer);
            glDeleteVertexArrays(1, &this->vertexArray);
        }

        void SetActive() { glBindVertexArray(this->vertexArray); }

        const HE::Uint32 GetIndicesCount() const { return this->_uIndicesCount; }
        const HE::Uint32 GetVertsCount() const { return this->_uVertCount; }

    protected:
        HE::Uint32 _uVertCount    = 0;
        HE::Uint32 _uIndicesCount = 0;
        unsigned int vertexBuffer = 0;
        unsigned int indexBuffer  = 0;
        unsigned int vertexArray  = 0;
    };

    void Mesh::Init()
    {
        this->_pVertexArray = HE_NEW_MEM(VertexArray, 0)();
    }

    void Mesh::Release()
    {
        auto pVertexArray = reinterpret_cast<VertexArray*>(this->_pVertexArray);
        if (pVertexArray)
        {
            pVertexArray->Release();
        }

        HE_SAFE_DELETE_MEM(this->_pVertexArray);
    }

    void Mesh::WriteDrawData(const VertexData& in_rMeshVertexData,
                             const IndexData& in_rMeshIndexData)
    {
        auto pVertexArray = reinterpret_cast<VertexArray*>(this->_pVertexArray);
        HE_ASSERT_RETURN(pVertexArray);

        // 頂点とそれに紐づいた頂点レイアウトの書き込み
        pVertexArray->WriteVertexAndLayout(in_rMeshVertexData);

        // 頂点と紐づいた頂点インデックスの書き込み
        if ((in_rMeshIndexData._pIndices != NULL) && (0 < in_rMeshIndexData._uIndicesCount))
        {
            pVertexArray->WriteIndices(in_rMeshIndexData);
        }
    }

    void Mesh::FreeDrawData()
    {
        auto pVertexArray = reinterpret_cast<VertexArray*>(this->_pVertexArray);
        HE_ASSERT_RETURN(pVertexArray);

        pVertexArray->Release();
    }

    void Mesh::DrawByVertexOnly(const HE::Uint32 in_uMode)
    {
        auto pVertexArray = reinterpret_cast<VertexArray*>(this->_pVertexArray);
        HE_ASSERT_RETURN(pVertexArray);

        pVertexArray->SetActive();

        ::glDrawArrays(in_uMode, 0, pVertexArray->GetVertsCount());
    }

    void Mesh::DrawByVertexAndIndex(const HE::Uint32 in_uMode)
    {
        auto pVertexArray = reinterpret_cast<VertexArray*>(this->_pVertexArray);
        HE_ASSERT_RETURN(pVertexArray);
        HE_ASSERT_RETURN(0 < pVertexArray->GetIndicesCount());

        pVertexArray->SetActive();

        ::glDrawElements(in_uMode, pVertexArray->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
    }

}  // namespace PlatformSDL2
