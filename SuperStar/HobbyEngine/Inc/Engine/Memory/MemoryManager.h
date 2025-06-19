#pragma once

#include "Engine/Common/Singleton.h"
#include "Engine/Core.h"

namespace Core::Memory
{
    // 最低アラインサイズ（ヘッダもこのアラインである必要あり）
    // X64であれば8byteなのでアライメントは8にする
#ifdef HE_X64
    constexpr HE::Uint8 minimumAlignSize = 8;
#else
    constexpr HE::Uint8 minimumAlignSize = 4;
#endif

    /// <summary>
    /// メモリ管理
    /// アプリで使用するメモリをリークを検知するためにアプリ独自のメモリ管理を使う
    /// メモリ管理のインスタンスは複数作るケースはメモリ管理が分散して複雑になるので絶対にだめ
    /// </summary>
    class Manager final : public Common::Singleton<Manager>
    {
        HE_CLASS_COPY_NG(Manager);
        HE_CLASS_MOVE_NG(Manager);

    public:
        explicit Manager() = default;

    public:
        /// <summary>
        /// 確保の位置タイプ
        /// 確保位置を決めれる
        /// </summary>
        enum EAllocateLocateType
        {
            // 前から取る
            EAllocateLocateType_Top = 0,
            // 後ろから取る
            EAllocateLocateType_Last,
        };

    private:
        // メモリページの最大数。アラインはこのサイズの公倍数である必要あり
        static constexpr HE::Uint8 MemoryPageMax = 64;

        // 念のためアラインを１に
        // アラインが4とかになっていた場合
        // 1バイトとかのメンバの後ろに勝手にパディングいれられてしまうので
#pragma pack(push, 1)
        /// <summary>
        /// メモリブロックの先頭に書き込まれる
        /// メモリブロックを管理データ
        /// </summary>
        struct BlockHeader
        {
            // メモリブロックのサイズ
            // ヘッダやパディングも含む
            HE::Uint32 _uSize = 0;
            // メモリブロックでアロケーションされるサイズ
            // 実際に使用できるサイズ
            HE::Uint32 _uAllocateSize = 0;
            // 前のメモリブロックアドレス
            BlockHeader* _pPrev = NULL;
            // 後ろのメモリブロックアドレス．
            BlockHeader* _pNext = NULL;
            // 使用状況
            // もしかしたら通常の場合と配列確保の場合で分けるかも
            HE::Uint8 _useFlag = 0;
            // アラインサイズ
            HE::Uint8 _alignSize = minimumAlignSize;
            // 使用ページ番号
            HE::Uint8 _page = 0;
            // 後ろのパディングサイズ
            HE::Uint8 _paddingSize = 0;

#ifdef HE_ENGINE_DEBUG
            // ファイル名（デバッグ用）
            HE::UTF8 _szFileName[256] = {};
            // 行番号（デバッグ用）
            HE::Uint32 _uLine = 0;
            // 門番（デバッグ用）
            HE::Uint32 _uMagicNumber = 0;
#endif
        };

#ifdef HE_ENGINE_DEBUG
        /// <summary>
        /// メモリブロックのフッターデータ
        /// メモリブロックの最後に書き込まれる
        /// メモリブロックの管理データ
        /// 現在はデバッグ用のデータしかないのでリリースには含めない
        /// </summary>
        struct BlockFooter
        {
            // 門番（デバッグ用）
            HE::Uint32 _uMagicNumber = 0;
        };
#endif

        /// <summary>
        /// メモリページデータ
        /// </summary>
        struct PageInfo
        {
            // ページのトップアドレス．
            void* _pTopAddr = NULL;
            // ページに割り当てられたサイズ．
            HE::Uint32 _uSize = 0;
            // メモリブロック先頭．
            BlockHeader* _pMemoryBlockTop = NULL;
        };
#pragma pack(pop)

    public:
        // 無効なメモリページ（主に終端子用）
        static constexpr HE::Uint8 invalidMemoryPage = 0xff;

#pragma pack(push, 1)
        /// <summary>
        /// メモリページ設定データ
        /// </summary>
        struct PageSetupInfo
        {
            // ページ番号．
            HE::Uint8 _page = invalidMemoryPage;
            // ページに割り当てるサイズ（MinimumAlignSizeの倍数であること）
            HE::Uint32 _uSize = HE::uInvalidUint32;
        };
#pragma pack(pop)

    public:
        // システムが稼働開始
        HE::Bool Start(const HE::Uint32 in_uManageSize);

        // システムの解放
        HE::Bool VRelease() override final;

#ifdef HE_ENGINE_DEBUG
        // メモリ確保
        void* Allocate(const HE::Uint32 in_uAllocateSize, const HE::Uint8 in_page,
                       const HE::Uint8 in_alignSize, const EAllocateLocateType in_eLocateType,
                       const HE::UTF8* in_pFile, HE::Uint32 in_uLine);
#else
        // メモリ確保
        void* Allocate(const Uint32 in_uAllocateSize, const HE::Uint8 in_page,
                       const HE::Uint8 in_alignSize, const EAllocateLocateType in_eLocateType);
#endif

        // メモリ開放
        void Free(void* in_pAllocatedMemory);

        // 確保したメモリのサイズを取得
        HE::Uint32 GetAllocateSize(void* in_pAllocatedMemory);

        // メモリページのセットアップ
        HE::Bool SetupMemoryPage(PageSetupInfo* in_pSetupInfoArray, const HE::Uint32 in_uNum);
        // メモリページのリマップ
        // 各メモリページを再構築する
        // しかしページ内に使用中のメモリが残っていると失敗してしまう
        HE::Bool RemapMemoryPage(PageSetupInfo* in_pSetupInfoArray, const HE::Uint32 in_uNum);

        // メモリが管理対象のメモリかそうでないか
        HE::Bool ValidMemory(void* in_pAllocatedMemory);

        /// <summary>
        /// 全ページ内に利用中のメモリがあるか
        /// </summary>
        HE::Bool UsedAllBlock() const;

        /// <summary>
        /// 指定ページに利用中のメモリがあるか
        /// </summary>
        HE::Bool UsedBlock(const HE::Uint8) const;

    private:
        /// <summary>
        /// 利用準備ができているか
        /// </summary>
        inline HE::Bool _IsReady() const { return (this->_pHeapTop != NULL); }

        /// <summary>
        /// 指定ページが有効か
        /// </summary>
        inline HE::Bool _ValidMemoryPage(const HE::Uint8 in_page) const
        {
            return (this->_aMemoryPageInfoArray[in_page]._pTopAddr != NULL);
        }

        /// <summary>
        /// メモリブロックのヘッダーサイズを取得
        /// </summary>
        inline HE::Uint32 _GetMemoryBlockHeaderSize() const { return (sizeof(BlockHeader)); }

        /// <summary>
        /// メモリブロックのフッターサイズを取得
        /// </summary>
        inline HE::Uint32 _GetMemoryBlockFooterSize() const
        {
            // フッターはデバッグ用のしかないから
#ifdef HE_ENGINE_DEBUG
            return (sizeof(BlockFooter));
#else
            return 0;
#endif
        }

        /// <summary>
        /// メモリブロックのシステム利用サイズを取得
        /// </summary>
        inline HE::Uint32 _GetMemoryBlockSystemDataSize() const
        {
            return (this->_GetMemoryBlockHeaderSize() + this->_GetMemoryBlockFooterSize());
        }

        // メモリページの初期化
        HE::Bool _InitMemoryPage(const HE::Uint8 in_page, const HE::Uint32 in_uOffset,
                                 const HE::Uint32 in_uSize);
        // 指定のメモリブロックの前にメモリブロックを連結する
        void _AddListMemoryBlockPrev(BlockHeader* in_pMemoryBlock,
                                     BlockHeader* in_pTargetMemoryBlock);
        // 指定のメモリブロックの後ろに、メモリブロックを連結する
        void _AddListMemoryBlockNext(BlockHeader* in_pMemoryBlock,
                                     BlockHeader* in_pTargetMemoryBlock);
        // メモリブロックをリストからはずす
        void _DelListMemoryBlock(BlockHeader* in_pMemoryBlock);
        // 指定のメモリブロックを指定サイズで二つに分ける
        BlockHeader* _SplitMemoryBlock(BlockHeader* in_pMemoryBlock, HE::Uint32 in_uSplitSize);
        // 指定されたメモリブロックをマージする
        BlockHeader* _MergeMemoryBlock(BlockHeader* in_pMemoryBlock1,
                                       BlockHeader* in_pMemoryBlock2);

#ifdef HE_ENGINE_DEBUG
    public:
        // 指定されたメモリページの情報を表示する
        void PrintMemoryPageInfo(const HE::Uint8);
        // すべてのメモリ情報を表示する
        void PrintAllMemoryInfo();

        // 指定されたメモリページのメモリブロックが正常かチェックする
        HE::Bool CheckMemoryBlockByPage(const HE::Uint8);
        // すべてのメモリブロックが正常かチェックする
        HE::Bool CheckAllMemoryBlock();

        /// <summary>
        /// 指定メモリページでメモリリークしているかチェック
        /// </summary>
        /// <param name=""></param>
        /// <returns>TRUE: リークしていない</returns>
        HE::Bool CheckAllMemoryLeakByPage(const HE::Uint8);

        /// <summary>
        /// メモリリークしているかチェック
        /// </summary>
        /// <returns>TRUE: リークしていない</returns>
        HE::Bool CheckAllMemoryLeak();

    private:
        // TODO: ランタイムでエンディアンを判定する方法あるか?
        // 門番（ビッグエンディアン）
        // static const Uint32 MAGIC_NUMBER = 0xDEADDEAD;
        // 門番(リトルエンディアン)
        static constexpr HE::Uint32 uMagicNumber = 0xADEDADDE;
        // 空きメモリ領域に埋めるデータ
        static constexpr HE::Uint8 freeMemoryFillData = 0xDD;
        // 取得直後のメモリに埋めるデータ
        static constexpr HE::Uint8 newMemoryFillData = 0xCD;

        /// <summary>
        /// 指定メモリブロックが有効かチェック
        /// </summary>
        /// <param name="in_pMemoryBlock">メモリブロック</param>
        /// <returns>TRUE 有効 / FALSE 無効</returns>
        HE::Bool _IsValidMemoryBlock(BlockHeader* in_pMemoryBlock)
        {
            // ヘッダーの管理データをチェック
            if (in_pMemoryBlock->_uMagicNumber != uMagicNumber)
            {
                return FALSE;
            }

            //  フッターの管理データをチェック
            HE::Sint8* pAddr = (reinterpret_cast<HE::Sint8*>(in_pMemoryBlock)) +
                               (in_pMemoryBlock->_uSize - this->_GetMemoryBlockFooterSize());
            if (reinterpret_cast<BlockFooter*>(pAddr)->_uMagicNumber != uMagicNumber)
            {
                return FALSE;
            }

            return TRUE;
        }

        /// <summary>
        /// デバッグ用
        /// メモリブロックに門番を書き込む
        /// </summary>
        /// <param name="in_pMemoryBlock">メモリブロックのポインタ</param>
        inline void _SetMemoryBlockMagicNo(BlockHeader* in_pMemoryBlock)
        {
            // ヘッダーに書き込む
            in_pMemoryBlock->_uMagicNumber = uMagicNumber;

            BlockFooter* pBlockFooter = reinterpret_cast<BlockFooter*>(
                reinterpret_cast<HE::Sint8*>(in_pMemoryBlock) +
                (in_pMemoryBlock->_uSize - _GetMemoryBlockFooterSize()));
            pBlockFooter->_uMagicNumber = uMagicNumber;
        }

        // 指定されたメモリブロックを空きメモリデータで塗りつぶす
        void _FillFreeMemoryBlock(BlockHeader* in_pMemoryBlock);
        // 指定されたメモリブロックを新規メモリデータで塗りつぶす
        void _FillNewMemoryBlock(BlockHeader* in_pMemoryBlock);
#endif

    private:
        // プラットフォームから取得した際のヒープの先頭アドレス
        void* _pHeapTop = NULL;
        // ヒープサイズ
        HE::Uint32 _uHeapSize = 0;
        // メモリページ情報
        // あまり柔軟性よくてもしょうがない(速度重視の配列)
        PageInfo _aMemoryPageInfoArray[MemoryPageMax];
    };
}  // namespace Core::Memory
