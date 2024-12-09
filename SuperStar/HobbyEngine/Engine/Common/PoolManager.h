#pragma once

// 要素の数が動的に変わるので以下のデータ構造クラスを使っている
// TODO: 自前で動的対応したのを作るのでそれまでの利用
#include <unordered_map>
#include <vector>

#include "Engine/Common/CustomArray.h"
#include "Engine/Common/CustomMap.h"
#include "Engine/Common/CustomStack.h"
// #include "Engine/Common/CustomVector.h"
#include "Engine/Common/Handle.h"
#include "Engine/Core.h"
#include "Engine/Memory/Memory.h"

namespace Core::Common
{
    /// <summary>
    /// プールしたデータを使いまわすためのクラス
    /// データを使いまわしてメモリ確保と解放の処理コストを減らせる
    /// プールデータ型をテンプレートで指定する
    /// データを使いまわすのでクラス型を利用するとクラスのプロパティ値が残るので注意
    /// 継承利用版
    /// </summary>
    template <typename T>
    class RuntimePoolManager
    {
    public:
        /*
            struct AllocData
            {
                T* _pItem = NULL;
                Core::Common::Handle _handle;
            };
            */

    public:
        virtual ~RuntimePoolManager() { this->_ReleasePool(); }

        /// <summary>
        /// データ使用個数
        /// </summary>
        const HE::Uint32 UseCount() const
        {
            if (this->_upUserSlot == NULL) return 0;

            return static_cast<HE::Uint32>(this->_upUserSlot->size());
        }

        /// <summary>
        /// データ最大数
        /// </summary>
        const HE::Uint32 Max() const { return static_cast<HE::Uint32>(this->_upCacheDatas->capacity()); }

        /// <summary>
        /// キャッシュしたデータが現在いくつか
        /// </summary>
        const HE::Uint32 CacheCount() const { return static_cast<HE::Uint32>(this->_upCacheDatas->size()); }

        const bool Empty() const { return (this->UseCount() <= 0); }

        // 現在利用しているデータリストを取得
        // 参照にするとデータをコピーして遅くなるので注意
        inline const std::unordered_map<Core::Common::Handle, T*>* GetUserDataList() const
        {
            HE_ASSERT(this->_upUserSlot);
            return this->_upUserSlot.get();
        }

        /// <summary>
        /// ハンドルが存在するか
        /// </summary>
        HE::Bool Valid(const Core::Common::Handle& in_h)
        {
            if (this->_upUserSlot->at(in_h)) return TRUE;

            return FALSE;
        }

    protected:
        /// <summary>
        /// プールのバッファ予約
        /// プールするためのデータバッファ数を指定して確保
        /// 継承したクラスが必ず実行
        /// </summary>
        void _ReservePool(const HE::Uint32 in_uMax)
        {
            // TODO: 予約した数を変えたい場合にも対応できるようにしたほうがいい

            if (this->_upCacheDatas == NULL)
                this->_upCacheDatas = HE_MAKE_CUSTOM_UNIQUE_PTR((std::vector<T*>));

            if (this->_upUserSlot == NULL)
            {
                this->_upUserSlot =
                    HE_MAKE_CUSTOM_UNIQUE_PTR((std::unordered_map<Core::Common::Handle, T*>));
            }

            this->_upCacheDatas->reserve(in_uMax);
        }

        void _ReleasePool()
        {
            if (this->_upCacheDatas)
            {
                for (auto itr = this->_upCacheDatas->begin(); itr != this->_upCacheDatas->end();
                     ++itr)
                {
                    HE_SAFE_DELETE_MEM(*itr);
                }
                this->_upCacheDatas->clear();

                HE_SAFE_DELETE_UNIQUE_PTR(this->_upCacheDatas);
            }

            if (this->_upUserSlot)
            {
                for (auto itr = this->_upUserSlot->begin(); itr != this->_upUserSlot->end(); ++itr)
                {
                    HE_SAFE_DELETE_MEM(itr->second);
                }

                this->_upUserSlot->clear();

                HE_SAFE_DELETE_UNIQUE_PTR(this->_upUserSlot);
            }
        }

        /// <summary>
        /// プールしているデータの中で利用できるデータ枠を取得
        /// 利用するデータとそのデータを紐づけたハンドルを返す
        /// </summary>
        template <class S, typename... TArgs>
        std::tuple<Core::Common::Handle, T*> _Alloc(TArgs&&... in_args)
        {
            // TODO: タプル使った方がいい？
            HE_STATIC_ASSERT(std::is_base_of<T, S>::value, "SクラスはTクラスを継承していない");

            HE_ASSERT(this->_upCacheDatas);
            HE_ASSERT(this->_upUserSlot);

            // AllocData allocData;
            HE_ASSERT(0 < this->_upCacheDatas->capacity());

            // 割り当てられなかったら空の枠を返す
            if (this->UseCount() >= this->_upCacheDatas->capacity())
            {
                HE_ASSERT(FALSE && "オブジェクトを割り当てる数が足りない");
                return std::tuple<Core::Common::Handle, T*>(NullHandle, NULL);
            }

            Handle handle;

            HE::Bool bNewSlot = FALSE;
            S* pObject    = NULL;

            if (this->_upCacheDatas->empty())
            {
                bNewSlot = TRUE;
            }
            else
            {
                HE::Bool bFreeSlot = FALSE;

                // フリー領域にあるのがSクラスかどうかチェックしてあればそれを使う,
                // なければ新規作成する すでに生成したTクラスのインスタンスを再利用
                HE::Uint32 uChkIndex = 0;
                for (auto b = this->_upCacheDatas->begin(); b != this->_upCacheDatas->end(); ++b)
                {
                    // 再利用可能なデータかチェック
                    pObject = dynamic_cast<S*>(*b);
                    if (pObject != NULL)
                    {
                        // 再利用可能なデータなので追加フラグを立てる
                        bFreeSlot = TRUE;

                        // キャッシュデータを利用したのでキャッシュリストから外す
                        this->_upCacheDatas->erase(b);
                        handle.SetIndex(uChkIndex);

                        // TODO: 再利用する場合はコンストラクターを呼ばなくてもいいのか？

                        break;
                    }

                    ++uChkIndex;
                }

                if (bFreeSlot == FALSE)
                {
                    bNewSlot = TRUE;
                }
            }

            if (bNewSlot)
            {
                ++this->_uIndexCount;
                handle.SetIndex(this->_uIndexCount);

                // Tを継承したSクラスのインスタンスを生成
                // NEWは用意したマクロを使う
                pObject = HE_NEW_MEM(S, 0)(std::forward<TArgs>(in_args)...);
            }

            // allocData._handle = handle;
            // allocData._pItem  = pObject;

            // 利用リストに追加
            this->_upUserSlot->insert(std::make_pair(handle, pObject));

            return std::tuple<Core::Common::Handle, T*>(handle, pObject);
        }

        /// <summary>
        /// 割り当てデータを解放
        /// </summary>
        void _Free(const Handle& in_rHandle, const HE::Bool in_bCache)
        {
            HE_ASSERT(in_rHandle.Null() == FALSE && "解放するデータがないとだめ");

            T* pRemoveObj = this->_upUserSlot->at(in_rHandle);
            HE_ASSERT(pRemoveObj != NULL);
            this->_upUserSlot->erase(in_rHandle);

            if (in_bCache)
            {
                // メモリ確保した要素をキャッシュして使いまわす
                this->_upCacheDatas->push_back(pRemoveObj);
            }
            else
            {
                // キャッシュしたのを破棄する
                HE_SAFE_DELETE_MEM(pRemoveObj);
            }
        }

        // データの参照(非const版)
        T* _Ref(const Handle& in_rHandle)
        {
            if (in_rHandle.Null()) return NULL;
            // 要素があるかチェック
            if (this->_upUserSlot == FALSE) return NULL;

            if (this->_upUserSlot->find(in_rHandle) != this->_upUserSlot->end())
                return this->_upUserSlot->at(in_rHandle);

            return NULL;
        }

        // データの参照(const版)
        const T* _Ref(const Handle& in_rHandle) const
        {
            typedef RuntimePoolManager<T> ThisType;
            return (const_cast<ThisType*>(this)->_Ref(in_rHandle));
        }

    private:
        // 利用中のデータスロット
        Core::Memory::UniquePtr<std::unordered_map<Core::Common::Handle, T*>> _upUserSlot = NULL;

        // 再利用するキャッシュデータリスト
        Core::Memory::UniquePtr<std::vector<T*>> _upCacheDatas = NULL;
        HE::Uint32 _uIndexCount                                    = 0;
    };

    /// <summary>
    /// プールしたデータを使いまわすためのクラス
    /// データを使いまわしてメモリ確保と解放の処理コストを減らせる
    /// プールデータ型をテンプレートで指定する
    /// データ最大数は固定
    /// データを使いまわすのでクラス型を利用するとクラスのプロパティ値が残るので注意
    /// </summary>
    template <typename T, HE::Uint32 TCapacity>
    class FixedPoolManager final
    {
    public:
        using UseDataMap = FixedMap<Core::Common::Handle, T*, TCapacity>;

    public:
        /// <summary>
        /// データ使用個数
        /// </summary>
        inline HE::Uint32 Size() const HE_NOEXCEPT { return this->_mUseData.Size(); }

        /// <summary>
        /// オブジェクトが空かどうか
        /// </summary>
        inline HE::Bool Empty() const HE_NOEXCEPT { return (this->_mUseData.Size() <= 0); }

        /// <summary>
        /// 利用中のデータマップ
        /// </summary>
        const UseDataMap& GetUseDataMap() const { return this->_mUseData; }

        /// <summary>
        /// プールしているデータの中で利用できるデータ枠を取得
        /// 利用するデータとそのデータを紐づけたハンドルを返す
        /// </summary>
        std::tuple<Core::Common::Handle, T*> Alloc()
        {
            HE::Uint32 uUseSlotIndex = 0;
            if (this->_sFreeDataIndex.Empty())
            {
                if (TCapacity <= this->_uFreeSlotMax)
                    return std::tuple<Core::Common::Handle, T*>(NullHandle, NULL);

                uUseSlotIndex = this->_uFreeSlotMax;
                ++this->_uFreeSlotMax;

                this->_sFreeDataIndex.PushBack(uUseSlotIndex);
            }

            uUseSlotIndex = this->_sFreeDataIndex.PopBack();
            Core::Common::Handle handle;
            handle.SetIndex(uUseSlotIndex);

            HE_ASSERT(uUseSlotIndex < this->_aData.Capacity() &&
                      "プールオブジェクトのフリーインデックス値が確保数を超えている");

            this->_mUseData.Add(handle, &this->_aData[uUseSlotIndex]);

            return std::tuple<Core::Common::Handle, T*>(handle, &this->_aData[uUseSlotIndex]);
        }

        /// <summary>
        /// 割り当てデータを解放
        /// </summary>
        HE::Bool Free(const Handle& in_rHandle)
        {
            HE_ASSERT(in_rHandle.Null() == FALSE);
            if (in_rHandle.Null()) return FALSE;

            if (this->_mUseData.Contains(in_rHandle) == FALSE)
            {
                return FALSE;
            }

            const HE::Uint32 uIndex = in_rHandle.Index();
            HE_ASSERT(uIndex < this->_aData.Capacity());

            T* pData = &this->_aData[uIndex];

            // 共有ポインタであればresetをして解放する
            if constexpr (IsShaderPtrByTemplateType<T>::value)
            {
                pData->reset();
                pData = NULL;
            }

            this->_mUseData.Erase(in_rHandle);
            this->_sFreeDataIndex.PushBack(uIndex);

            return TRUE;
        }

        // データの参照(非const版)
        T* Ref(const Handle& in_rHandle)
        {
            if (in_rHandle.Null()) return NULL;

            HE::Uint32 uIndex = in_rHandle.Index();
            HE_ASSERT(uIndex < this->_aData.Capacity());

            return &this->_aData[uIndex];
        }

        // データの参照(const版)
        const T* Ref(const Handle& in_rHandle) const
        {
            typedef FixedPoolManager<T, TCapacity> ThisType;
            return (const_cast<ThisType*>(this)->_Ref(in_rHandle));
        }

    private:
        using ArrayData     = FixedArray<T, TCapacity>;
        using FreeDataStack = FixedStack<HE::Uint32, TCapacity>;

        ArrayData _aData;
        UseDataMap _mUseData;
        FreeDataStack _sFreeDataIndex;

        HE::Uint32 _uFreeSlotMax = 0;
    };
}  // namespace Core::Common
