#include "AssetManager/AssetDataXml.h"

// サードパーティーのxmlライブラリ
#include "pugixml/pugixml.hpp"

// 依存モジュール
#include "Engine/Platform/PlatformFile.h"

namespace AssetManager
{
    namespace Local
    {
        /// <summary>
        /// ノード
        /// </summary>
        class XmlNode : public AbstractTreeNode
        {
        public:
            XmlNode(pugi::xml_node& in_rNode, const HE::UTF8* in_szKey, const HE::Sint32 in_sLevel)
                : AbstractTreeNode(in_szKey, in_sLevel), _value(in_rNode)
            {
            }

            void* VGetSource() override { return &this->_value; }

            HE::Uint32 VGetUInt32(const HE::UTF8* in_szName = NULL) override
            {
                return static_cast<HE::Uint32>(this->_value.attribute(in_szName).as_uint());
            }

            HE::Sint32 VGetSInt32(const HE::UTF8* in_szName = NULL) override
            {
                return static_cast<HE::Sint32>(this->VGetUInt32(in_szName));
            }

            HE::Float32 VGetFloat32(const HE::UTF8* in_szName = NULL) override
            {
                return static_cast<HE::Float32>(this->_value.attribute(in_szName).as_double());
            }

            void VOutputString(Core::Common::StringBase* out,
                               const HE::UTF8* in_szName = NULL) override
            {
                auto attr = this->_value.attribute(in_szName);
                if (attr.empty() == FALSE)
                {
                    *out = attr.as_string();
                }
                else
                {
                    out->Clear();
                }
            }

            const HE::Bool VOutputUTF8(HE::UTF8* out, const HE::Uint32 in_uLen,
                                       const HE::UTF8* in_szName) override
            {
                auto attr = this->_value.attribute(in_szName);
                if (attr.empty() == FALSE)
                {
                    HE_STR_U8_COPY_S(out, in_uLen, attr.as_string(),
                                     HE_STR_U8_LENGTH(attr.as_string()));
                    return TRUE;
                }
                return FALSE;
            }

        public:
            pugi::xml_node _value;
        };
    };  // namespace Local

    HE::Bool AssetDataXml::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        HE::Bool bRet = TRUE;

        this->_pDoc              = HE_NEW_MEM(pugi::xml_document, 0)();
        pugi::xml_document* pDoc = reinterpret_cast<pugi::xml_document*>(this->_pDoc);

        // ファイルを開く
        this->_fileHandle = in_rFileSystem.VFileOpen(this->_path);
        HE_ASSERT(this->_fileHandle.Null() == FALSE);
        {
            HE::UTF8* pReadTmpBuff = NULL;
            {
                // 開いたファイルのデータサイズを取得して読み込むメモリを確保
                HE::Sint32 iSize    = in_rFileSystem.VFileSize(this->_fileHandle);
                HE::Sint32 iMemSize = iSize + 1;

                pReadTmpBuff =
                    reinterpret_cast<HE::UTF8*>(HE_ALLOC_MEM(sizeof(HE::UTF8) * iMemSize, 0));
                ::memset(pReadTmpBuff, '\0', iMemSize);

                // ファイルの読み込み
                if (in_rFileSystem.VFileRead(pReadTmpBuff, this->_fileHandle, iSize))
                {
                    pugi::xml_parse_result result = pDoc->load_string(pReadTmpBuff);
                    if (result)
                    {
                        HE_LOG_LINE(HE_STR_TEXT("XML Load Success: %s"), this->_path.Str());
                    }
                    else
                    {
                        HE_LOG_LINE(HE_STR_TEXT("Error description: %s"), result.description());
                        HE_LOG_LINE(HE_STR_TEXT("Error offset: %td (error at [...%s] "),
                                    result.offset, (pReadTmpBuff + result.offset));
                    }
                }
                else
                {
                    HE_ASSERT(0);
                }
            }
            // 展開した時のメモリを利用するので読み込んだメモリを解放
            HE_SAFE_DELETE_MEM(pReadTmpBuff);
        }
        // ファイルを閉じる
        in_rFileSystem.VFileClose(this->_fileHandle);

        if (bRet == FALSE) return FALSE;

        return TRUE;
    }

    void AssetDataXml::_VUnload()
    {
        pugi::xml_document* pDoc = reinterpret_cast<pugi::xml_document*>(this->_pDoc);
        if (pDoc)
        {
            pDoc->reset();
        }
        HE_SAFE_DELETE_MEM(this->_pDoc);
    }

    InterfaceTreeData::NodeSharedPtr AssetDataXml::VGetNodeByName(
        const std::initializer_list<const HE::UTF8*>& in_aName)
    {
        pugi::xml_document* pDoc = reinterpret_cast<pugi::xml_document*>(this->_pDoc);

        auto szKey             = HE_STR_U8_TEXT("");
        pugi::xml_node xmlNode = pDoc->root();

        HE::Sint32 sLevel = AbstractTreeNode::sNoneLevel;
        for (auto it = in_aName.begin(); it != in_aName.end(); ++it)
        {
            auto node = xmlNode.child(*it);
            if (node)
            {
                xmlNode = node;
                szKey   = *it;
                sLevel  = 0;
            }
            else
            {
                break;
            }
        }

        return HE_MAKE_CUSTOM_SHARED_PTR((Local::XmlNode), xmlNode, szKey, sLevel);
    }

    InterfaceTreeData::NodeSharedPtr AssetDataXml::VGetNodeByName(
        AbstractTreeNode& in_rLocateNode, const std::initializer_list<const HE::UTF8*>& in_aName)
    {
        auto pLocateNode = (reinterpret_cast<pugi::xml_node*>(in_rLocateNode.VGetSource()));

        auto szKey = HE_STR_U8_TEXT("");
        pugi::xml_node xmlNode(*pLocateNode);
        HE::Sint32 sLevel = AbstractTreeNode::sNoneLevel;
        for (auto it = in_aName.begin(); it != in_aName.end(); ++it)
        {
            auto node = xmlNode.child(*it);
            if (node)
            {
                xmlNode = node;
                szKey   = *it;
                sLevel  = 0;
            }
            else
            {
                break;
            }
        }

        return HE_MAKE_CUSTOM_SHARED_PTR((Local::XmlNode), xmlNode, szKey, sLevel);
    }

    InterfaceTreeData::NodeSharedPtr AssetDataXml::VGetNodeByLevel(
        const std::initializer_list<const HE::UTF8*>& in_aName, const HE::Sint32 in_uLevel)
    {
        auto spNode = this->VGetNodeByName(in_aName);
        return this->VGetNodeByLevel(*spNode, in_uLevel);
    }

    InterfaceTreeData::NodeSharedPtr AssetDataXml::VGetNodeByLevel(AbstractTreeNode& in_rLocateNode,
                                                                   const HE::Sint32 in_uLevel)
    {
        auto pLocateNode = (reinterpret_cast<pugi::xml_node*>(in_rLocateNode.VGetSource()));

        pugi::xml_node xmlNode;
        HE::Sint32 sLevel = AbstractTreeNode::sNoneLevel;

        for (auto it = pLocateNode->begin(); it != pLocateNode->end(); ++it)
        {
            if (in_uLevel <= sLevel) break;

            xmlNode = *it;
            ++sLevel;
        }

        // レベル外のノードはなし
        if (sLevel < in_uLevel)
        {
            sLevel = AbstractTreeNode::sNoneLevel;
        }

        return HE_MAKE_CUSTOM_SHARED_PTR((Local::XmlNode), xmlNode, xmlNode.name(), sLevel);
    }

}  // namespace AssetManager
