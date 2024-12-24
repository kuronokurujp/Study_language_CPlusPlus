#include "Mesh.h"

#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    namespace Local
    {
        /// <summary>
        /// OpenGLのメッシュ構築するレイアウト
        /// </summary>
        class MeshLayout
        {
        public:
            virtual ~MeshLayout() = default;

            void BeginWrite()
            {
                // 頂点リストオブジェクトを作成して書き込み開始する
                glGenVertexArrays(1, &this->_vertexArrayObject);
                glBindVertexArray(this->_vertexArrayObject);
            }

            void EndWrite() { glBindVertexArray(0); }

            void WriteVertexAndLayout(const Mesh::VertexData& in_rMeshVertexData)
            {
                this->_uVertCount = 0;

                HE::Uint32 uVertSize = in_rMeshVertexData._uVertSize;

                // VBOを作成
                if (0 < in_rMeshVertexData._uVerticesCount)
                {
                    this->_uVertCount = in_rMeshVertexData._uVerticesCount;

                    glGenBuffers(1, &this->_vertexBufferObject);
                    // 頂点情報を格納するバッファ生成
                    // GL_ARRAY_BUFFERで指定
                    glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferObject);
                    // GenBufferで作成したデータ領域に書き込むサイズを計算
                    // 頂点データ構造(x,y,z,u,v) => 3次元座標とUV座標

                    HE::Uint32 uVertexsSize = this->_uVertCount * uVertSize;
                    glBufferData(GL_ARRAY_BUFFER, uVertexsSize, in_rMeshVertexData._pVertices,
                                 GL_STATIC_DRAW);
                }

                // データレイアウト作成
                {
                    int layoutOffsetPointerSize = 0;

                    HE::Uint32 uGroupIdx = 0;
                    for (auto itr = in_rMeshVertexData._aBindLayout.begin();
                         itr != in_rMeshVertexData._aBindLayout.end(); ++itr)
                    {
                        // こうしないと警告でエラーになる
                        void* pPointer =
                            reinterpret_cast<void*>(static_cast<HE::Ptr>(layoutOffsetPointerSize));

                        // グループ
                        glEnableVertexAttribArray(uGroupIdx);
                        // 頂点データをバイトオフセットにしている
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

                // IBOを作成
                if (0 < in_rMeshIndexData._uIndicesCount)
                {
                    glGenBuffers(1, &this->_indexBufferObject);
                    // インデックスバッファ生成
                    // GL_ELEMENT_ARRAY_BUFFERでインデックスバッファ専用のが作れる
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_indexBufferObject);

                    this->_uIndicesCount = in_rMeshIndexData._uIndicesCount;
                    // インデックス情報に書き込むデータサイズ計算
                    int indexSize = this->_uIndicesCount * sizeof(in_rMeshIndexData._pIndices[0]);

                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, in_rMeshIndexData._pIndices,
                                 GL_STATIC_DRAW);
                }
            }

            void Release()
            {
                glDeleteBuffers(1, &this->_vertexBufferObject);
                glDeleteBuffers(1, &this->_indexBufferObject);
                glDeleteVertexArrays(1, &this->_vertexArrayObject);
            }

            void Enable() { glBindVertexArray(this->_vertexArrayObject); }
            void Disable() { glBindVertexArray(0); }

            const HE::Uint32 GetIndicesCount() const { return this->_uIndicesCount; }
            const HE::Uint32 GetVertsCount() const { return this->_uVertCount; }

        protected:
            HE::Uint32 _uVertCount     = 0;
            HE::Uint32 _uIndicesCount  = 0;
            GLuint _vertexBufferObject = 0;
            GLuint _indexBufferObject  = 0;
            GLuint _vertexArrayObject  = 0;
        };
    }  // namespace Local

    void Mesh::Init()
    {
        this->_pMeshLayout = HE_NEW_MEM(Local::MeshLayout, 0)();
    }

    void Mesh::Release()
    {
        auto pMeshLayout = reinterpret_cast<Local::MeshLayout*>(this->_pMeshLayout);
        if (pMeshLayout)
        {
            pMeshLayout->Release();
        }

        HE_SAFE_DELETE_MEM(this->_pMeshLayout);
    }

    void Mesh::WriteDrawData(const VertexData& in_rMeshVertexData,
                             const IndexData& in_rMeshIndexData)
    {
        auto pMeshLayout = reinterpret_cast<Local::MeshLayout*>(this->_pMeshLayout);
        HE_ASSERT_RETURN(pMeshLayout);

        pMeshLayout->BeginWrite();
        {
            // 頂点とそれに紐づいた頂点レイアウトの書き込み
            pMeshLayout->WriteVertexAndLayout(in_rMeshVertexData);

            // 頂点と紐づいた頂点インデックスの書き込み
            if ((in_rMeshIndexData._pIndices != NULL) && (0 < in_rMeshIndexData._uIndicesCount))
            {
                pMeshLayout->WriteIndices(in_rMeshIndexData);
            }
        }
        pMeshLayout->EndWrite();
    }

    void Mesh::FreeDrawData()
    {
        auto pMeshLayout = reinterpret_cast<Local::MeshLayout*>(this->_pMeshLayout);
        HE_ASSERT_RETURN(pMeshLayout);

        pMeshLayout->Release();
    }

    void Mesh::DrawByVertexOnly(const HE::Uint32 in_uMode, const HE::Uint32 in_uVertCount)
    {
        auto pMeshLayout = reinterpret_cast<Local::MeshLayout*>(this->_pMeshLayout);
        HE_ASSERT_RETURN(pMeshLayout);

        pMeshLayout->Enable();

        HE::Uint32 uVertCount = pMeshLayout->GetVertsCount();
        if (0 < in_uVertCount) uVertCount = in_uVertCount;

        ::glDrawArrays(in_uMode, 0, uVertCount);

        pMeshLayout->Disable();
    }

    void Mesh::DrawByVertexAndIndex(const HE::Uint32 in_uMode)
    {
        auto pMeshLayout = reinterpret_cast<Local::MeshLayout*>(this->_pMeshLayout);
        HE_ASSERT_RETURN(pMeshLayout);
        HE_ASSERT_RETURN(0 < pMeshLayout->GetIndicesCount());

        pMeshLayout->Enable();

        ::glDrawElements(in_uMode, pMeshLayout->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);

        pMeshLayout->Disable();
    }
}  // namespace PlatformSDL2
