#include "PlatformSDL2/Graphic/Draw/Mesh.h"

#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    namespace Local
    {
        // TODO: これクラス化する必要あるのか？
        // 使いまわしをしないのであればメッシュに入れればいい気が
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
                HE_ASSERT(glGetError() == GL_NO_ERROR);
                glBindVertexArray(this->_vertexArrayObject);
            }

            void EndWrite() { glBindVertexArray(0); }

            void WriteVertexAndLayout(const MeshVertexData& in_rMeshVertexData)
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

            void WriteIndices(const MeshIndexData& in_rMeshIndexData)
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

            GLuint GetVertexBufferObject() const { return this->_vertexBufferObject; }

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

    void Mesh::Write(const MeshVertexData& in_rMeshVertexData,
                     const MeshIndexData& in_rMeshIndexData)
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

    void Mesh::Free()
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

    ParticleMesh::ParticleMesh(const HE::Uint32 in_uCount)
    {
        this->_uCount = in_uCount;
        this->_pItems = reinterpret_cast<Item*>(HE_ALLOC_MEM(sizeof(Item) * this->_uCount, 0));
    }

    void ParticleMesh::Init()
    {
        this->_pMeshLayout = HE_NEW_MEM(Local::MeshLayout, 0)();
        auto pMeshLayout   = reinterpret_cast<Local::MeshLayout*>(this->_pMeshLayout);
        HE_ASSERT_RETURN(pMeshLayout);

        pMeshLayout->BeginWrite();
        {
            //  頂点など描画に必要な情報をバインド
            MeshVertexData bindMeshVertexData;
            {
                // ポイント描画なので頂点情報は不要
                bindMeshVertexData._pVertices      = NULL;
                bindMeshVertexData._uVerticesCount = this->_uCount;
                bindMeshVertexData._uVertSize      = sizeof(Item);
                bindMeshVertexData._aBindLayout    = {MeshLayoutData(3, sizeof(Item::position)),
                                                      MeshLayoutData(3, sizeof(Item::velocity))};
            }

            // 頂点とそれに紐づいた頂点レイアウトの書き込み
            pMeshLayout->WriteVertexAndLayout(bindMeshVertexData);
        }
        pMeshLayout->EndWrite();
    }

    void ParticleMesh::Release()
    {
        auto pMeshLayout = reinterpret_cast<Local::MeshLayout*>(this->_pMeshLayout);
        if (pMeshLayout)
        {
            pMeshLayout->Release();
        }

        HE_SAFE_DELETE_MEM(this->_pMeshLayout);
        HE_SAFE_DELETE_MEM(this->_pItems);
    }

    void ParticleMesh::Draw()
    {
        auto pMeshLayout = reinterpret_cast<Local::MeshLayout*>(this->_pMeshLayout);
        HE_ASSERT_RETURN(pMeshLayout);

        HE::Uint32 uVertCount = pMeshLayout->GetVertsCount();
        if (uVertCount <= 0)
        {
            return;
        }

        pMeshLayout->Enable();

        if (this->_bUpdate)
        {
            // 頂点バッファオブジェクトを結合する
            glBindBuffer(GL_ARRAY_BUFFER, pMeshLayout->GetVertexBufferObject());

            // 頂点バッファオブジェクトにデータを格納する
            Item* p = static_cast<Item*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
            for (auto i = 0; i < this->_uCount; ++i)
            {
                p->position = this->_pItems[i].position;
                p->velocity = this->_pItems[i].velocity;
                ++p;
            }

            glUnmapBuffer(GL_ARRAY_BUFFER);
            this->_bUpdate = FALSE;
        }

        // 点で描画する
        ::glDrawArrays(GL_POINTS, 0, uVertCount);

        pMeshLayout->Disable();
    }

    void ParticleMesh::SetPositions(const Core::Common::ArrayBase<Core::Math::Vector3>& in_aPos)
    {
        HE_ASSERT_RETURN(in_aPos.Capacity() <= this->_uCount);
        for (auto i = 0; i < in_aPos.Capacity(); ++i)
        {
            this->_pItems[i].position = in_aPos[i];
        }
        this->_bUpdate = TRUE;
    }

    void ParticleMesh::SetVelocitys(
        const Core::Common::ArrayBase<Core::Math::Vector3>& in_aVelocity)
    {
        HE_ASSERT_RETURN(in_aVelocity.Capacity() <= this->_uCount);
        for (auto i = 0; i < in_aVelocity.Capacity(); ++i)
        {
            this->_pItems[i].velocity = in_aVelocity[i];
        }
        this->_bUpdate = TRUE;
    }

}  // namespace PlatformSDL2
