#include "Engine/Memory/Memory.h"

#ifdef HE_ENGINE_DEBUG

/// <summary>
/// newのオーバロード
/// アライメントはMinimumAlignSizeの倍数である必要がある
/// </summary>
/// <param name="in_size">確保したいサイズ</param>
/// <param name="in_page">メモリページ指定</param>
/// <param name="in_alignSize">メモリアライメントのサイズ</param>
/// <param name="in_locateType">確保位置</param>
/// <param name="in_pFile">呼び出したファイル</param>
/// <param name="in_line">呼び出したファイル行</param>
/// <returns>NULL 失敗 / 非NULL 確保したメモリのアドレス</returns>
void* operator new(size_t in_size, HE::Uint8 in_page, HE::Uint8 in_alignSize,
                   Core::Memory::Manager::EAllocateLocateType in_eLocateType,
                   const HE::UTF8* in_pFile, HE::Uint32 in_uLine)
{
    // VSのx64だとsize_tがunsigned int64になるが, 8byteフルサイズでの確保はないと思うので,
    // Uint32でサイズを調整した
    if (Core::Memory::Manager::Exist())
    {
        HE::Uint32 uMemSize = static_cast<HE::Uint32>(in_size);

        void* pMem = (Core::Memory::Manager::I().Allocate(uMemSize, in_page, in_alignSize,
                                                          in_eLocateType, in_pFile, in_uLine));
        return pMem;
    }

    HE_LOG_LINE(HE_STR_TEXT("警告: アプリ用のメモリアロケーターがない"));

    // TODO: メモリアロケーターがない場合はプラットフォームのAPIを呼ぶ
    return HE_ALLOC_MEM(in_size, in_page);
}

#if 0
/// <summary>
/// new[]のオーバーロード
/// </summary>
/// <param name="in_size">確保したいサイズ</param>
/// <param name="in_page">メモリページ指定ｊ</param>
/// <param name="in_alignSize">メモリアライメントのサイズ</param>
/// <param name="in_locateType">確保位置</param>
/// <param name="in_pFile">呼び出したファイル名</param>
/// <param name="in_line">呼び出したファイル行数</param>
/// <returns>NULL 失敗 / 非NULL 確保したメモリのアドレス</returns>
void* ::operator new[](size_t in_size, HE::Uint8 in_page, HE::Uint8 in_alignSize,
                       Core::Memory::Manager::EAllocateLocateType in_eLocateType,
                       const HE::UTF8 * in_pFile, HE::Uint32 in_uLine)
{
    // VSのx64だとsize_tがunsigned int64になるが, 8byteフルサイズでの確保はないと思うので,
    // Uint32でサイズを調整した
    if (Core::Memory::Manager::Exist())
    {
        HE::Uint32 uMemSize = static_cast<HE::Uint32>(in_size);
        void* pMem      = (Core::Memory::Manager::I().Allocate(uMemSize, in_page, in_alignSize,
                                                               in_eLocateType, in_pFile, in_uLine));
        // TODO: クラスでかつデストラクターがあると返った時のメモリアドレスが変わってしまう

        return pMem;
    }

    HE_LOG_LINE(HE_STR_TEXT("警告: アプリ用のメモリアロケーターがない"));

    return NULL;
}
#endif

void* AllocateMemory(const HE::Uint32 in_uAllocateSize, const HE::Uint8 in_page,
                     const HE::Uint8 in_alignSize,
                     const Core::Memory::Manager::EAllocateLocateType in_eLocateType,
                     const HE::UTF8* in_pFile, HE::Uint32 in_uLine)
{
    if (Core::Memory::Manager::Exist())
    {
        void* pMem = (Core::Memory::Manager::I().Allocate(in_uAllocateSize, in_page, in_alignSize,
                                                          in_eLocateType, in_pFile, in_uLine));
        return pMem;
    }
    // TODO: メモリアロケーターがない場合はプラットフォームのAPIを呼ぶ
    else
    {
#ifdef HE_WIN
        return ::_aligned_malloc(in_uAllocateSize, 32);
#endif
    }

    return NULL;
}

#else

/// <summary>
/// newのオーバロード
/// アライメントはMinimumAlignSizeの倍数である必要がある
/// </summary>
/// <param name="in_size">確保したいサイズ</param>
/// <param name="in_page">メモリページ指定</param>
/// <param name="in_alignSize">メモリアライメントのサイズ</param>
/// <param name="in_locateType">確保位置</param>
/// <param name="in_pFile">呼び出したファイル</param>
/// <param name="in_line">呼び出したファイル行</param>
/// <returns>NULL 失敗 / 非NULL 確保したメモリのアドレス</returns>
void* operator new(size_t in_size, HE::Uint8 in_page, HE::Uint8 in_alignSize,
                   Core::Memory::Manager::EAllocateLocateType in_eLocateType)
{
    // VSのx64だとsize_tがunsigned int64になるが, 8byteフルサイズでの確保はないと思うので,
    // Uint32でサイズを調整した
    HE::Uint32 uMemSize = static_cast<HE::Uint32>(in_size);
    return (
        Core::Memory::Manager::I().AllocateMemory(uMemSize, in_page, in_alignSize, in_eLocateType));
}

/// <summary>
/// new[]のオーバーロード
/// </summary>
/// <param name="in_size">確保したいサイズ</param>
/// <param name="in_page">メモリページ指定ｊ</param>
/// <param name="in_alignSize">メモリアライメントのサイズ</param>
/// <param name="in_locateType">確保位置</param>
/// <param name="in_pFile">呼び出したファイル名</param>
/// <param name="in_line">呼び出したファイル行数</param>
/// <returns>NULL 失敗 / 非NULL 確保したメモリのアドレス</returns>
void* operator new[](size_t in_size, HE::Uint8 in_page, HE::Uint8 in_alignSize,
                     Core::Memory::Manager::EAllocateLocateType in_eLocateType)
{
    // VSのx64だとsize_tがunsigned int64になるが, 8byteフルサイズでの確保はないと思うので,
    // Uint32でサイズを調整した
    HE::Uint32 uMemSize = static_cast<HE::Uint32>(in_size);
    return (
        Core::Memory::Manager::I().AllocateMemory(uMemSize, in_page, in_alignSize, in_eLocateType));
}

// メモリ確保
void* AllocateMemory(const HE::Uint32 in_uAllocateSize, const HE::Uint8 in_page,
                     const HE::Uint8 in_alignSize,
                     const Core::Memory::Manager::EAllocateLocateType in_eLocateType)
{
    void* pMem = (Core::Memory::Manager::I().AllocateMemory(in_uAllocateSize, in_page, in_alignSize,
                                                            in_eLocateType);
    return pMem;
}

#endif

/// <summary>
/// メモリ解放
/// </summary>
void FreeMemory(void* in_pPtr)
{
    // 初回の共有ポインターを生成したらここが呼ばれてin_pPtrが0になっている
    // ポインターがないのはおかしいので即終了している
    if (in_pPtr == 0) return;

    if (Core::Memory::Manager::Exist())
    {
        Core::Memory::Manager::I().Free(in_pPtr);
    }
    else
    {
        // メモリアロケーターがないという警告を出す
        HE_LOG_LINE(HE_STR_TEXT("警告: メモリアロケーターが存在しないかすでに破棄されている"));
#ifdef HE_WIN
        // 解放する
        return ::_aligned_free(in_pPtr);
#endif
    }
}
