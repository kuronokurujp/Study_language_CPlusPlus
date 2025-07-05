#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/CustomVector.h"
#include "Engine/Memory/Memory.h"

namespace Module
{
    class ModuleBase;

    /// <summary>
    /// モジュールの実行レイヤー
    /// 上から順にモジュールが実行される
    /// 解放は下から順に実行する
    /// </summary>
    enum ELayer
    {
        ELayer_App = 0,
        ELayer_Logic,
        ELayer_View,
    };

    /// <summary>
    /// モジュール管理クラス
    /// </summary>
    class ModuleManager
    {
    public:
        Core::Memory::SharedPtr<ModuleBase> Get(const HE::Char* in_szName) const;

        template <class T>
        Core::Memory::SharedPtr<T> Get() const
        {
            Core::Common::FixedString128 szName(T::ModuleName());
            auto pModule = this->Get(szName.Str());

            HE_ASSERT(pModule && "モジュールが存在しない");
            return HE_SHADER_PTR_CAST(T, pModule);
        }

        /// <summary>
        /// モジュールを登録
        /// </summary>
        HE::Bool AddModule(Core::Memory::SharedPtr<ModuleBase>);

        /// <summary>
        /// モジュールの利用開始
        /// </summary>
        HE::Bool Start();

        /// <summary>
        /// 解放
        /// </summary>
        HE::Bool Release();

        /// <summary>
        /// モジュール群の前更新
        /// </summary>
        void BeforeUpdate(const HE::Float32 in_fDeltaTime);

        /// <summary>
        /// モジュール群の更新
        /// </summary>
        void Update(const HE::Float32 in_fDeltaTime);

        /// <summary>
        /// モジュール群の後更新
        /// </summary>
        void LateUpdate(const HE::Float32 in_fDeltaTime);

    private:
        using ModuleProcessBaseVector  = Core::Common::VectorBase<ModuleBase*>;
        using ModuleProcessFixedVector = Core::Common::FixedVector<ModuleBase*, 16>;
        using ModuleFixedMap           = Core::Common::FixedMap<Core::Common::FixedString128,
                                                                Core::Memory::SharedPtr<ModuleBase>, 16>;

    private:
        HE::Bool _StartModule(ModuleBase&);
        void _SortModuleVector(ModuleProcessBaseVector* out);

    private:
        ModuleProcessFixedVector _vAppModuleProcess;
        ModuleProcessFixedVector _vLogicModuleProcess;
        ModuleProcessFixedVector _vViewModuleProcess;

        ModuleFixedMap _mAppModule;
        ModuleFixedMap _mLogicModule;
        ModuleFixedMap _mViewModule;
    };

    // アプリ層とロジック層とビュー層で種類を分けるべきか
    /// <summary>
    /// 外部モジュールの基本クラス
    /// </summary>
    class ModuleBase
    {
        HE_CLASS_COPY_NG(ModuleBase);
        HE_CLASS_MOVE_NG(ModuleBase);

    public:
        ModuleBase(const char* in_szName, ELayer const in_eLayer = ELayer::ELayer_Logic,
                   const HE::Uint32 in_prioriry = 0);
        virtual ~ModuleBase() = default;

        template <typename T>
        Core::Memory::SharedPtr<T> GetDependenceModule()
        {
            Core::Common::FixedString64 szName(T::ModuleName());
            Core::Memory::SharedPtr<T> spTargetModule = HE_SHADER_PTR_CAST(T, this->_GetModule(szName));
            HE_ASSERT_RETURN_VALUE(NULL, spTargetModule && "モジュールとして登録していないのでだめ");

            // 依存対象のモジュールかどうかチェック
            HE::Bool bHit = FALSE;
            for (HE::Uint32 i = 0; i < this->_vDependenceModuleName.Size(); ++i)
            {
                if (this->_vDependenceModuleName[i] == szName)
                {
                    bHit = TRUE;
                    break;
                }
            }

            HE_ASSERT_RETURN_VALUE(NULL, bHit && "依存モジュールが存在しないのはまずい");

            return spTargetModule;
        }

        /// <summary>
        /// モジュールが依存しているモジュール名リスト
        /// </summary>
        const Core::Common::VectorBase<Core::Common::FixedString64>& GetDependenceModuleNames()
            const
        {
            return this->_vDependenceModuleName;
        }

        inline const HE::Char* Name() const HE_NOEXCEPT { return this->_szName.Str(); }
        inline ELayer Layer() const HE_NOEXCEPT { return this->_eLayer; }

        /// <summary>
        /// モジュール処理優先度
        /// 値が大きいほど後に処理
        /// </summary>
        inline HE::Uint32 Priority() const { return this->_priority; }

    protected:
        /// <summary>
        /// モジュールの開始
        /// </summary>
        virtual HE::Bool _VStart() = 0;

        /// <summary>
        /// モジュールの解放
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual HE::Bool _VRelease() = 0;

        // TODO: 本当に前と後更新が必要なのだろうか？
        // 正直無駄な処理なのではと思った
        /// <summary>
        /// モジュール前更新
        /// </summary>
        virtual void _VBeforeUpdate(const HE::Float32 in_fDeltaTime) {}

        /// <summary>
        /// モジュール更新
        /// </summary>
        virtual void _VUpdate(const HE::Float32 in_fDeltaTime) {}

        /// <summary>
        /// モジュール後更新
        /// </summary>
        virtual void _VLateUpdate(const HE::Float32 in_fDeltaTime) {}

        template <class T>
        void _AppendDependenceModule()
        {
            this->_vDependenceModuleName.PushBack(T::ModuleName());
        }

    private:
        using EventModuleGetter = std::function<Core::Memory::SharedPtr<ModuleBase>(const HE::Char*)>;

    private:
        Core::Memory::SharedPtr<ModuleBase> _GetModule(Core::Common::StringBase&);

#ifdef HE_ENGINE_DEBUG
        HE::Bool _ValidateDependenceModule();
#endif

    private:
        Core::Common::FixedString128 _szName;
        ELayer _eLayer       = ELayer_Logic;
        HE::Uint32 _priority = 0;

#ifdef HE_ENGINE_DEBUG
        Core::Common::FixedVector<ModuleBase*, 64> _vDependenceModule;
#endif
        Core::Common::FixedVector<Core::Common::FixedString64, 64> _vDependenceModuleName;

        EventModuleGetter _eventModuleGetter = NULL;

    private:
        // モジュールはモジュールマネージャーからのみアクセス可能
        friend class ModuleManager;
    };

}  // namespace Module

// モジュールクラスに宣言するマクロ
// _type_にクラス型を記述
#define HE_MODULE_GENRATE_DECLARATION(_type_) \
public:                                       \
    static const HE::UTF8* ModuleName()       \
    {                                         \
        return #_type_;                       \
    }
