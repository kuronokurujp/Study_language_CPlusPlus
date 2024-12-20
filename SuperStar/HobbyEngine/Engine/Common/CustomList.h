#pragma once

#include "Engine/Core.h"

namespace Core::Common
{
    /// <summary>
    /// 双方向のリンクリストのノード
    /// リストの利用者は必ずこのクラスを継承してリスト要素とする
    /// </summary>
    /// <typeparam name="T">リストで管理するデータ型</typeparam>
    template <typename T>
    class LinkedListNode
    {
        // ノードを操作する親リストにだけprivateを公開しておく
        template <typename T>
        friend class LinkedListBase;

        HE_CLASS_COPY_CONSTRUCT_NG(LinkedListNode);
        HE_CLASS_MOVE_CONSTRUCT_NG(LinkedListNode);

    public:
        static const HE::Uint32 uFlagLinked = 0x00000001;

    public:
        LinkedListNode()          = default;
        virtual ~LinkedListNode() = default;

        inline LinkedListNode* GetPrev() const { return this->_pPrev; }
        inline LinkedListNode* GetNext() const { return this->_pNext; }

        void operator=(LinkedListNode& in_rNode)
        {
            this->_pNext = in_rNode._pNext;
            this->_pPrev = in_rNode._pPrev;
            this->_uFlag = in_rNode._uFlag;
        }

        void operator=(const LinkedListNode& in_rNode)
        {
            this->_pNext = in_rNode._pNext;
            this->_pPrev = in_rNode._pPrev;
            this->_uFlag = in_rNode._uFlag;
        }

        // TODO: イテレーターを生成

    private:
        LinkedListNode* _pPrev = NULL;
        LinkedListNode* _pNext = NULL;
        HE::Uint32 _uFlag      = 0;
    };

    /// <summary>
    /// 双方向リストのイテレーターのテンプレート
    /// </summary>
    /// <typeparam name="TDataType">LinkedListNodeで指定してテンプレート型</typeparam>
    /// <typeparam name="TNodeType">LinkedListNode継承したクラス</typeparam>
    template <typename TDataType, typename TNodeType>
    class ListNodeIterator
    {
    public:
        ListNodeIterator() = default;
        ListNodeIterator(TNodeType* in_pNode) : _pNode(in_pNode) {}

        ListNodeIterator(const ListNodeIterator<TDataType, TNodeType>& in_rIt)
            : _pNode(in_rIt._pNode)
        {
        }

        ListNodeIterator(const ListNodeIterator<TDataType, TNodeType>&& in_rIt)
            : _pNode(in_rIt._pNode)
        {
        }

        virtual ~ListNodeIterator() = default;

        /// <summary>
        /// データ参照
        /// ノードクラスを継承した派生クラスへキャスト
        /// </summary>
        /// <returns></returns>
        TDataType& operator*()
        {
            HE_ASSERT(this->_pNode);
            return static_cast<TDataType&>(*this->_pNode);
        }

        /// <summary>
        /// データ参照
        /// </summary>
        const TDataType& operator*() const { return static_cast<TDataType&>(*this->_pNode); }

        /// <summary>
        /// データポインタ参照
        /// </summary>
        /// <returns></returns>
        TDataType* operator->() { return static_cast<TDataType*>(this->_pNode); }

        /// <summary>
        /// データポインタ参照
        /// </summary>
        /// <returns></returns>
        const TDataType* operator->() const { return static_cast<TDataType*>(this->_pNode); }

        /// <summary>
        /// インクリメント
        /// 次のノードを返す
        /// </summary>
        ListNodeIterator& operator++()
        {
            this->_pNode = this->_pNode->GetNext();
            return *this;
        }

        /// <summary>
        /// インクリメント
        /// 次のノードを返す
        /// </summary>
        ListNodeIterator operator++(int)
        {
            ListNodeIterator it = *this;
            ++(*this);
            return it;
        }

        /// <summary>
        /// デクリメント
        /// 前のノードを返す
        /// </summary>
        ListNodeIterator& operator--()
        {
            this->_pNode = this->_pNode->GetPrev();
            return *this;
        }

        /// <summary>
        /// デクリメント
        /// 前のノードを返す
        /// </summary>
        ListNodeIterator operator--(int)
        {
            ListNodeIterator it = *this;
            --(*this);
            return it;
        }

        /// <summary>
        /// 比較
        /// </summary>
        HE::Bool operator==(const ListNodeIterator<TDataType, TNodeType>& in_rRhs) const
        {
            return (this->_pNode == in_rRhs._pNode);
        }

        /// <summary>
        /// 比較
        /// </summary>
        HE::Bool operator!=(const ListNodeIterator<TDataType, TNodeType>& in_rRhs) const
        {
            return ((*this == in_rRhs) == FALSE);
        }

        /// <summary>
        /// 代入
        /// </summary>
        const ListNodeIterator& operator=(const ListNodeIterator<TDataType, TNodeType>& in_rRhs)
        {
            this->_pNode = in_rRhs._pNode;
            return *this;
        }

        TNodeType* GetNode() const { return this->_pNode; }

    private:
        TNodeType* _pNode = NULL;
    };

    /// <summary>
    /// 双方向リストの逆イテレータ
    /// </summary>
    /// <typeparam name="TDataType">LinkedListNodeで指定してテンプレート型</typeparam>
    /// <typeparam name="TNodeType">LinkedListNode継承したクラス</typeparam>
    template <typename TDataType, typename TNodeType>
    class ListNodeReverseIterator
    {
    public:
        ListNodeReverseIterator() = default;
        ListNodeReverseIterator(TNodeType* in_pNode) : _pNode(in_pNode) {}

        ListNodeReverseIterator(const ListNodeReverseIterator<TDataType, TNodeType>& it)
            : _pNode(it._pNode)
        {
        }

        ListNodeReverseIterator(const ListNodeReverseIterator<TDataType, TNodeType>&& it)
            : _pNode(it._pNode)
        {
        }

        virtual ~ListNodeReverseIterator() = default;

        /// <summary>
        /// データ参照
        /// 派生クラスにキャストして返している
        /// </summary>
        TDataType& operator*()
        {
            HE_ASSERT(this->_pNode);
            return static_cast<TDataType&>(*this->_pNode);
        }

        /// <summary>
        /// データ参照
        /// </summary>
        const TDataType& operator*() const { return static_cast<TDataType&>(this->_pNode); }

        /// <summary>
        /// データポインタ参照
        /// </summary>
        TDataType* operator->() { return static_cast<TDataType*>(this->_pNode); }

        /// <summary>
        /// データポインタ参照
        /// </summary>
        const TDataType* operator->() const { return static_cast<TDataType*>(this->_pNode); }

        /// <summary>
        /// インクリメント
        /// </summary>
        ListNodeReverseIterator& operator++()
        {
            this->_pNode = this->_pNode->GetPrev();
            return *this;
        }

        /// <summary>
        /// インクリメント
        /// </summary>
        ListNodeReverseIterator operator++(int)
        {
            ListNodeReverseIterator it = *this;
            ++(*this);
            return it;
        }

        /// <summary>
        /// デクリメント
        /// </summary>
        ListNodeReverseIterator& operator--()
        {
            this->_pNode = this->_pNode->GetNext();
            return *this;
        }

        /// <summary>
        /// デクリメント
        /// </summary>
        ListNodeReverseIterator operator--(int)
        {
            ListNodeReverseIterator it = *this;
            --(*this);
            return it;
        }

        /// <summary>
        /// 比較
        /// </summary>
        HE::Bool operator==(const ListNodeReverseIterator<TDataType, TNodeType>& in_rRhs) const
        {
            return (this->_pNode == in_rRhs._pNode);
        }

        /// <summary>
        /// 比較
        /// </summary>
        HE::Bool operator!=(const ListNodeReverseIterator<TDataType, TNodeType>& in_rRhs) const
        {
            return ((*this == in_rRhs) == FALSE);
        }

        /// <summary>
        /// 代入
        /// </summary>
        const ListNodeReverseIterator& operator=(
            const ListNodeReverseIterator<TDataType, TNodeType>& in_rRhs)
        {
            this->_pNode = in_rRhs._pNode;
            return *this;
        }

        TNodeType* GetNode() const { return _pNode; }

    private:
        TNodeType* _pNode = NULL;
    };

    /// <summary>
    /// リストの基本クラス
    /// </summary>
    /// <typeparam name="T">ノードで管理するデータ型</typeparam>
    template <typename T>
    class LinkedListBase
    {
    public:
        LinkedListBase()          = default;
        virtual ~LinkedListBase() = default;

    protected:
        /// <summary>
        /// ノードの消去
        /// </summary>
        HE::Bool Erase(LinkedListNode<T>* in_pNode)
        {
            // 既にリンクから外されているノードは無視
            if ((in_pNode->_uFlag & LinkedListNode<T>::uFlagLinked) == 0)
            {
                return FALSE;
            }

            HE_ASSERT(in_pNode->_pNext);
            HE_ASSERT(in_pNode->_pPrev);
            in_pNode->_pNext->_pPrev = in_pNode->_pPrev;
            in_pNode->_pPrev->_pNext = in_pNode->_pNext;

            // 接続済みフラグを消す
            in_pNode->_uFlag &= ~LinkedListNode<T>::uFlagLinked;

            return TRUE;
        }
        /// <summary>
        /// ノード挿入
        /// </summary>
        HE::Bool Insert(LinkedListNode<T>* in_pPrevNode, LinkedListNode<T>* in_pNewNode)
        {
            HE_ASSERT(in_pNewNode != in_pPrevNode);

            // 循環リストになってしまうので既にリンクされているノードは接続しない
            if (in_pNewNode->_uFlag & LinkedListNode<T>::uFlagLinked)
            {
                HE_ASSERT(FALSE && "すでにリストに挿入しているノードは挿入できない");
                return FALSE;
            }

            if (in_pPrevNode->_pNext)
            {
                in_pPrevNode->_pNext->_pPrev = in_pNewNode;
            }
            in_pNewNode->_pNext = in_pPrevNode->_pNext;

            in_pPrevNode->_pNext = in_pNewNode;
            in_pNewNode->_pPrev  = in_pPrevNode;

            // 接続済みフラグ
            in_pNewNode->_uFlag |= LinkedListNode<T>::uFlagLinked;
            HE_ASSERT(in_pNewNode->GetPrev() != in_pNewNode->GetNext() &&
                      "前と後ろの参照が同じなのはありえない");

            return TRUE;
        }
    };

    /// <summary>
    /// 双方向リスト
    /// リストのノードは利用者がLinkedListNodeを継承して作成したのが対象になる
    /// </summary>
    /// <typeparam name="T">ノードで管理するデータ型</typeparam>
    template <typename T>
    class CustomList final : public LinkedListBase<T>
    {
    public:
        // イテレータの型定義
        using IteratorChar = ListNodeIterator<T, LinkedListNode<T>>;
        using RevIterator  = ListNodeReverseIterator<T, LinkedListNode<T>>;

    public:
        CustomList() { LinkedListBase::Insert(&this->_head, &this->_tail); }
        virtual ~CustomList() { this->Clear(); }

        void Clear()
        {
            // リスト設定しているノードを一つずつ外している
            auto itr = this->BeginItr();
            while (itr != this->EndItr()) this->Erase(itr);
        }

        /// <summary>
        /// リストの終端につなげる
        /// </summary>
        HE::Bool PushBack(LinkedListNode<T>& in_rNode)
        {
            return LinkedListBase::Insert(this->_tail.GetPrev(), &in_rNode);
        }

        /// <summary>
        /// リストの先頭につなげる
        /// </summary>
        HE::Bool PushFront(LinkedListNode<T>& in_rNode)
        {
            return LinkedListBase::Insert(&this->_head, &in_rNode);
        }

        /// <summary>
        /// 任意の位置の直前にノードを挿入
        /// </summary>
        HE::Bool Insert(IteratorChar& in_rIt, LinkedListNode<T>& in_rNode)
        {
            return LinkedListBase<T>::Insert(in_rIt.GetNode()->GetPrev(), &in_rNode);
        }

        /// <summary>
        /// 終端のノードを取る
        /// </summary>
        HE::Bool PopBack()
        {
            if (this->Empty()) return FALSE;

            return LinkedListBase<T>::Erase(this->_tail.GetPrev());
        }

        /// <summary>
        /// 先頭のノードを取る
        /// </summary>
        HE::Bool PopFront()
        {
            if (this->Empty()) return FALSE;

            return LinkedListBase<T>::Erase(this->_head.GetNext());
        }

        /// <summary>
        /// ノードの消去
        /// </summary>
        IteratorChar Erase(LinkedListNode<T>* in_pNode)
        {
            HE_ASSERT(in_pNode);

            LinkedListNode<T>* pNext = in_pNode->GetNext();
            LinkedListBase<T>::Erase(in_pNode);

            return IteratorChar(pNext);
        }

        /// <summary>
        /// 設定したイテレータのノードを取る
        /// </summary>
        IteratorChar Erase(const IteratorChar& in_rIt)
        {
            LinkedListNode<T>* pNode = in_rIt.GetNode();
            LinkedListNode<T>* pNext = pNode->GetNext();
            LinkedListBase<T>::Erase(pNode);

            return IteratorChar(pNext);
        }

        /// <summary>
        /// 先頭のイテレータを取得
        /// </summary>
        IteratorChar BeginItr() { return IteratorChar(this->_head.GetNext()); }

        /// <summary>
        /// 終端のイテレータを取得
        /// </summary>
        IteratorChar EndItr() { return IteratorChar(&this->_tail); }

        /// <summary>
        /// 先頭の逆イテレータを取得
        /// リストの終端からリストの先端のループで利用
        /// </summary>
        RevIterator BeginRevItr() { return RevIterator(this->_tail.GetPrev()); }

        /// <summary>
        /// 終端の逆イテレータを取得
        /// </summary>
        RevIterator EndRevItr() { return RevIterator(&this->_head); }

        /// <summary>
        /// リストが空かどうか
        /// </summary>
        HE::Bool Empty() const { return this->_head.GetNext() == &this->_tail; }

    private:
        LinkedListNode<T> _head;
        LinkedListNode<T> _tail;
    };

    // テンプレートクラス CustomList の部分的な型特性
    template <typename T>
    struct IsCustomList : std::false_type
    {
    };

    // CustomList のインスタンスに対する特殊化
    template <typename TType>
    struct IsCustomList<CustomList<TType>> : std::true_type
    {
    };

}  // namespace Core::Common
