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

        void Bind(const Mesh::BindMeshVertexData& in_rMeshVertexData,
                  const Mesh::BindMeshIndexData& in_rMeshIndexData)
        {
            this->_uVertCount    = in_rMeshVertexData._uVerticesCount;
            this->_uIndicesCount = in_rMeshIndexData._uIndicesCount;

            HE::Uint32 uVertSize = 0;
            {
                for (auto itr = in_rMeshVertexData._aBindLayout.begin();
                     itr != in_rMeshVertexData._aBindLayout.end(); ++itr)
                {
                    uVertSize += itr->_uDataSize;
                }
            }

            // 頂点リストオブジェクトを作成して使用可能にする
            {
                glGenVertexArrays(1, &this->vertexArray);
                glBindVertexArray(this->vertexArray);
            }

            // 頂点情報を作成
            {
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

            // 頂点並びの添え字を作成
            {
                glGenBuffers(1, &this->indexBuffer);
                // インデックスバッファ生成
                // GL_ELEMENT_ARRAY_BUFFERでインデックスバッファ専用のが作れる
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);

                // インデックス情報に書き込むデータサイズ計算
                int indexSize = this->_uIndicesCount * sizeof(in_rMeshIndexData._pIndices[0]);

                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, in_rMeshIndexData._pIndices,
                             GL_STATIC_DRAW);
            }

            // データレイアウト作成
            {
                // xyzの頂点とuv頂点の5つの頂点
                float strideSize = uVertSize;
                // ItemMax;
                int layoutOffsetPointerSize = 0;

                HE::Uint32 uGroupIdx = 0;
                for (auto itr = in_rMeshVertexData._aBindLayout.begin();
                     itr != in_rMeshVertexData._aBindLayout.end(); ++itr)
                {
                    // xyzの頂点データを関連付けする
                    // グループ
                    glEnableVertexAttribArray(uGroupIdx);
                    glVertexAttribPointer(uGroupIdx, itr->_uItemCount, GL_FLOAT, GL_FALSE,
                                          static_cast<GLsizei>(strideSize), 0);
                    // TODO: レイアウトのオフセットはどうするか
                    layoutOffsetPointerSize += itr->_uDataSize;
                    ++uGroupIdx;
                }
            }
        }

        void Release()
        {
            glDeleteBuffers(1, &this->vertexBuffer);
            glDeleteBuffers(1, &this->indexBuffer);
            glDeleteVertexArrays(1, &this->vertexArray);
        }

        void SetActive()
        {
            HE_ASSERT(glIsBuffer(this->vertexBuffer));
            HE_ASSERT(glIsBuffer(this->indexBuffer));

            glBindVertexArray(this->vertexArray);
        }

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

    void Mesh::BindMeshData(const BindMeshVertexData& in_rMeshVertexData,
                            const BindMeshIndexData& in_rMeshIndexData)
    {
        auto pVertexArray = reinterpret_cast<VertexArray*>(this->_pVertexArray);
        HE_ASSERT_RETURN(pVertexArray);

        pVertexArray->Bind(in_rMeshVertexData, in_rMeshIndexData);
    }

    void Mesh::Draw()
    {
        auto pVertexArray = reinterpret_cast<VertexArray*>(this->_pVertexArray);
        HE_ASSERT_RETURN(pVertexArray);

        pVertexArray->SetActive();

        ::glDrawElements(GL_TRIANGLE_STRIP, pVertexArray->GetIndicesCount(), GL_UNSIGNED_INT,
                         nullptr);
    }
}  // namespace PlatformSDL2
