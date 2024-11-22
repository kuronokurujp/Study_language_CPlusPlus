#pragma once

// メモリシステムの宣言やマクロを記述
// メモリ確保・解放のマクロを定義
// メモリ管理クラスを使ったメモリ制御をラッピングしている

// メモリをページの前後から確保できる
// これはメモリ確保を繰り返してメモリが断片化して求めたサイズのメモリ確保ができない場合の対策
// メモリ確保する上で前後どちらで確保するのかをルールで決めるのがいい

#include <memory>
#include <new>

#include "Engine/Core.h"
#include "MemoryManager.h"

// プレースメントしたnewとそれに対応したdeleteを作っていないのでプレースメントしたnewを使うと警告が出るので抑制
#pragma warning(disable : 4291)

#ifdef HE_ENGINE_DEBUG

// newのオーバーロード
void* operator new(size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                   Core::Memory::Manager::EAllocateLocateType in_eLocateType, const UTF8* in_pFile,
                   Uint32 in_line);

#else

// newのオーバーロード
void* operator new(size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                   Core::Memory::Manager::EAllocateLocateType in_locateType);

#endif

#ifdef HE_ENGINE_DEBUG

// new[]のオーバーロード
void* operator new[](size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                     Core::Memory::Manager::EAllocateLocateType in_eLocateType,
                     const UTF8* in_pFile, Uint32 in_uLine);

#else

// new[]のオーバーロード
void* operator new[](size_t in_size, Uint8 in_page, Uint8 in_alignSize,
                     Core::Memory::Manager::EAllocateLocateType in_locateType);

#endif

#ifdef HE_ENGINE_DEBUG

// NEWマクロ
// メモリアライメント設定版
// 通常はこちらを利用
#define HE_NEW_MEM(type, page)                          \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Top, __FILE__, __LINE__)(type)

// マクロの引数で__FILE__と__LINE__を指定
#define HE_NEW_MEM_INFO(type, page, file, line)         \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Top, file, line)(type)

// NEWの配列マクロ
// メモリアライメント設定版
// 通常はこちらを利用
#define HE_NEW_MEM_ARRAY(type, num, page)               \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Top, __FILE__, __LINE__)(type[num])

// NEWマクロ
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_MEM_ALIENT(type, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocateType_Top, __FILE__, __LINE__)(type)

// NEW配列マクロ
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_MEM_ARRAY_ALIENT(type, num, page, alignSize)                         \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocateType_Top, __FILE__, \
         __LINE__)(type[num])

// NEWマクロ
// メモリをページの後ろから確保する
// アライメント設定版
#define HE_NEW_MEM_LAST(type, page)                     \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Last, __FILE__, __LINE__)(type)

// NEW配列のマクロ
// メモリをページの後ろから確保する
// アライメント設定版
#define HE_NEW_MEM_ARRAY_LAST(type, num, page)          \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Last, __FILE__, __LINE__)(type[num])

// NEWマクロ
// メモリをページの後ろから確保する
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_MEM_LAST_ALIENT(type, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocateType_Last, __FILE__, __LINE__)(type)

// NEW配列マクロ
// メモリをページの後ろから確保する
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_MEM_ARRAY_LAST_ALIENT(type, num, page, alignSize)                     \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocateType_Last, __FILE__, \
         __LINE__)(type[num])

/// <summary>
/// メモリ確保
/// </summary>
extern void* AllocateMemory(const Uint32 in_uAllocateSize, const Uint8 in_page,
                            const Uint8 in_alignSize,
                            const Core::Memory::Manager::EAllocateLocateType in_eLocateType,
                            const UTF8* in_pFile, Uint32 in_uLine);

#define HE_ALLOC_MEM(size, page)                                          \
    ::AllocateMemory(size, page, Core::Memory::Manager::MinimumAlignSize, \
                     Core::Memory::Manager::EAllocateLocateType_Top, __FILE__, __LINE__);

#define HE_ALLOC_MEM_LAST(size, page)                                     \
    ::AllocateMemory(size, page, Core::Memory::Manager::MinimumAlignSize, \
                     Core::Memory::Manager::EAllocateLocateType_Last, __FILE__, __LINE__);

#else

// NEWマクロ
// メモリアライメント設定版
// 通常はこちらを利用
#define HE_NEW_MEM(type, page)                          \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Top)(type)

// マクロの引数で__FILE__と__LINE__を指定
#define HE_NEW_MEM_INFO(type, page, file, line)         \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Top)(type)

// NEWの配列マクロ
// メモリアライメント設定版
// 通常はこちらを利用
#define HE_NEW_MEM_ARRAY(type, num, page)               \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocateType_Top)(type[num])

// NEWマクロ
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_MEM_ALIENT(type, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocateType_Top)(type)

// NEW配列マクロ
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_MEM_ARRAY_ALIENT(type, num, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocateType_Top)(type[num])

// NEWマクロ
// メモリをページの後ろから確保する
// アライメント設定版
#define HE_NEW_MEM_LAST(type, page)                       \
    / new (page, Core::Memory::Manager::MinimumAlignSize, \
           Core::Memory::Manager::EAllocateLocteType_Last)(type)

// NEW配列のマクロ
// メモリをページの後ろから確保する
// アライメント設定版
#define HE_NEW_MEM_ARRAY_LAST(type, num, page)          \
    new (page, Core::Memory::Manager::MinimumAlignSize, \
         Core::Memory::Manager::EAllocateLocteType_Last)(type[num])

// NEWマクロ
// メモリをページの後ろから確保する
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_MEM_LAST_ALIENT(type, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocteType_Last)(type)

// NEW配列マクロ
// メモリをページの後ろから確保する
// メモリアライメント指定(アライメントはMINIMUM_ALIGN_SIZEの倍数)
// 細かなメモリ確保で使用
// アライメント指定を間違えるとバグになるので蔵人向け
#define HE_NEW_MEM_ARRAY_LAST_ALIENT(type, num, page, alignSize) \
    new (page, alignSize, Core::Memory::Manager::EAllocateLocteType_Last)(type[num])

// メモリ確保
extern void* AllocateMemory(const Uint32 in_uAllocateSize, const Uint8 in_page,
                            const Uint8 in_alignSize,
                            const Core::Memory::Manager::EAllocateLocateType in_eLocateType);

#define HE_ALLOC_MEM(size, page)                                          \
    ::AllocateMemory(size, page, Core::Memory::Manager::MinimumAlignSize, \
                     Core::Memory::Manager::EAllocateLocateType_Top);

#define HE_ALLOC_MEM_LAST(size, page)                                     \
    ::AllocateMemory(size, page, Core::Memory::Manager::MinimumAlignSize, \
                     Core::Memory::Manager::EAllocateLocateType_Last);

#endif

/// <summary>
/// メモリ解放
/// </summary>
extern void FreeMemory(void*);

// deleteを安全する実行するためのマクロ
// ポインターチェックをしてすでに解放済みの場合でもエラーにはならないようにしている
#define HE_SAFE_DELETE_MEM(pPtr) \
    {                            \
        if (pPtr)                \
        {                        \
            ::FreeMemory(pPtr);  \
            (pPtr) = NULL;       \
        }                        \
    }

// 確保した配列メモリをdeleteで安全する実行するためのマクロ
// ポインターチェックをしてすでに解放済みの場合でもエラーにはならないようにしている
#define HE_SAFE_DELETE_MEM_ARRAY(pPtr) \
    {                                  \
        if (pPtr)                      \
        {                              \
            ::FreeMemory(pPtr);        \
            (pPtr) = NULL;             \
        }                              \
    }

// UniquePtrを解放して紐づいているメモリ削除
#define HE_SAFE_DELETE_UNIQUE_PTR(ptr) \
    {                                  \
        if (ptr)                       \
        {                              \
            auto p = ptr.release();    \
            HE_SAFE_DELETE_MEM(p);     \
            (ptr) = NULL;              \
        }                              \
    }

// メモリ解放をラップする構造体
struct DeleterFreeMemory
{
#ifdef HE_ENGINE_DEBUG
    const UTF8* szFilename = NULL;
    Uint32 uLine           = 0;

    DeleterFreeMemory(const UTF8* in_szFile, const Uint32 in_uLine)
        : szFilename(in_szFile), uLine(in_uLine)
    {
    }

    DeleterFreeMemory() : szFilename(""), uLine(0) {}
#endif

    void operator()(void* ptr) const
    {
#ifdef HE_ENGINE_DEBUG
//        HE_LOG_LINE(HE_STR_TEXT("DeleteFreeMemory: %s(%d)"), this->szFilename, this->uLine);
#endif
        HE_SAFE_DELETE_MEM(ptr);
        //::FreeMemory(ptr);
    }
};

namespace Core::Memory
{
    template <class T>
    using UniquePtr = std::unique_ptr<T, DeleterFreeMemory>;

    template <class T>
    using SharedPtr = std::shared_ptr<T>;

    /// <summary>
    /// 任意の型Tのスマートポインタをカスタムアロケータとデリータで作成する関数
    /// クラスの場合はデフォルトコンストラクターが必要
    /// TODO: メモリのページ指定が必要
    /// </summary>
    template <typename T, typename... TArgs>
#ifdef HE_ENGINE_DEBUG
    SharedPtr<T> MakeCustomSharedPtr(const UTF8* in_szFilename, const Uint32 in_uLine,
                                     TArgs&&... args)
#else
    SharedPtr<T> MakeCustomSharedPtr(Args&&... args)
#endif
    {
        // 配列の要素を構築し、shared_ptrを作成する
#ifdef HE_ENGINE_DEBUG
        return SharedPtr<T>(HE_NEW_MEM_INFO(T, 0, in_szFilename,
                                            in_uLine)(std::forward<TArgs>(args)...),
                            DeleterFreeMemory(in_szFilename, in_uLine));
#else
        return SharedPtr<T>(HE_NEW(T, 0)(std::forward<Args>(args)...), DeleterFreeMemory());
#endif
    }

    /// <summary>
    /// std::make_unique内で独自メモリシステムを利用
    /// </summary>
    template <typename T, typename... TArgs>
#ifdef HE_ENGINE_DEBUG
    UniquePtr<T> MakeCustomUniquePtr(const UTF8* in_szFilename, const Uint32 in_uLine,
                                     TArgs&&... args)
#else
    UniquePtr<T> MakeCustomUniquePtr(Args&&... args)
#endif
    {
#ifdef HE_ENGINE_DEBUG
        return UniquePtr<T>(HE_NEW_MEM_INFO(T, 0, in_szFilename,
                                            in_uLine)(std::forward<TArgs>(args)...),
                            DeleterFreeMemory(in_szFilename, in_uLine));
#else
        return UniquePtr<T>(HE_NEW(T, 0)(std::forward<Args>(args)...), DeleterFreeMemory());
#endif
    }

}  // namespace Core::Memory

// shader_ptr / unique_ptrを使うためのマクロ
// 利用箇所のファイル名とファイル行数を設定できるのでメモリリーク調査を少しでも進めるようにしている
#define HE_REMOVE_PARENS(...) __VA_ARGS__

#ifdef HE_ENGINE_DEBUG

// __T__型は括弧で囲ってほしい
//    HE_MAKE_CUSTOM_UNIQUE_PTR((Uint32));
// 括弧を囲う事で__T__型がテンプレートを使っている場合でも意図したマクロ置換が出来るようにしている
#define HE_MAKE_CUSTOM_SHARED_PTR(__T__, ...) \
    Core::Memory::MakeCustomSharedPtr<HE_REMOVE_PARENS __T__>(__FILE__, __LINE__, ##__VA_ARGS__)
#define HE_MAKE_CUSTOM_UNIQUE_PTR(__T__, ...) \
    Core::Memory::MakeCustomUniquePtr<HE_REMOVE_PARENS __T__>(__FILE__, __LINE__, ##__VA_ARGS__)

#else

#define HE_MAKE_CUSTOM_SHARED_PTR(__T__, ...) \
    Core::Memory::MakeCustomSharedPtr<HE_REMOVE_PARENS __T__>(##__VA_ARGS__)
#define HE_MAKE_CUSTOM_UNIQUE_PTR(__T__, ...) \
    Core::Memory::MakeCustomUniquePtr<HE_REMOVE_PARENS __T__>(##__VA_ARGS__)

#endif

// is_unique_ptr の安全な定義
// テンプレート型がUniquePtr型かどうかを判定
template <typename T, typename = void>
struct IsUniquePtrByTemplateType : std::false_type
{
};

template <typename T>
struct IsUniquePtrByTemplateType<T, std::void_t<typename T::element_type>>
    : std::is_same<T, Core::Memory::UniquePtr<typename T::element_type>>
{
};

// is_shader_ptr の安全な定義
// テンプレート型がShaderPtr型かどうかを判定
template <typename T, typename = void>
struct IsShaderPtrByTemplateType : std::false_type
{
};

template <typename T>
struct IsShaderPtrByTemplateType<T, std::void_t<typename T::element_type>>
    : std::is_same<T, Core::Memory::SharedPtr<typename T::element_type>>
{
};
