#include "Memory.h"

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
void* operator new(size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                   Core::Memory::Manager::EAllocateLocateType in_eLocateType, const UTF8* in_pFile,
                   Uint32 in_uLine)
{
    // VSのx64だとsize_tがunsigned int64になるが, 8byteフルサイズでの確保はないと思うので,
    // Uint32でサイズを調整した
    Uint32 uMemSize = static_cast<Uint32>(in_size);
    void* pMem      = (Core::Memory::Manager::I().AllocateMemory(uMemSize, in_page, in_alignSize,
                                                                 in_eLocateType, in_pFile, in_uLine));
    return pMem;
}

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
        Core::Memory::Manager::I().FreeMemory(in_pPtr);
    }
    else
    {
        // メモリアロケーターがないという警告を出す
        HE_LOG_LINE(HE_STR_TEXT("警告: メモリアロケーターが存在しないかすでに破棄されている"));
    }
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
void* ::operator new[](size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                       Core::Memory::Manager::EAllocateLocateType in_eLocateType,
                       const UTF8 * in_pFile, Uint32 in_uLine)
{
    // VSのx64だとsize_tがunsigned int64になるが, 8byteフルサイズでの確保はないと思うので,
    // Uint32でサイズを調整した
    Uint32 uMemSize = static_cast<Uint32>(in_size);
    void* pMem      = (Core::Memory::Manager::I().AllocateMemory(uMemSize, in_page, in_alignSize,
                                                                 in_eLocateType, in_pFile, in_uLine));
    return pMem;
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
void* operator new(size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                   Core::Memory::Manager::EAllocateLocateType in_eLocateType)
{
    // VSのx64だとsize_tがunsigned int64になるが, 8byteフルサイズでの確保はないと思うので,
    // Uint32でサイズを調整した
    Uint32 uMemSize = static_cast<Uint32>(in_size);
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
void* operator new[](size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                     Core::Memory::Manager::EAllocateLocateType in_eLocateType)
{
    // VSのx64だとsize_tがunsigned int64になるが, 8byteフルサイズでの確保はないと思うので,
    // Uint32でサイズを調整した
    Uint32 uMemSize = static_cast<Uint32>(in_size);
    return (
        Core::Memory::Manager::I().AllocateMemory(uMemSize, in_page, in_alignSize, in_eLocateType));
}
#endif
