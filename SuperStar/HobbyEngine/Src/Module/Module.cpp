#include "Engine/Module/Module.h"

#include "Engine/Engine.h"

namespace Module
{
    ModuleBase::ModuleBase(const HE::UTF8* in_szName, const ELayer in_eLayer,
                           const HE::Uint32 in_iPriority)
        : _szName(in_szName), _eLayer(in_eLayer), _priority(in_iPriority)
    {
    }

    Core::Memory::SharedPtr<ModuleBase> ModuleBase::_GetModule(Core::Common::StringBase& in_szName)
    {
        HE_ASSERT_RETURN_VALUE(NULL, this->_eventModuleGetter);
        auto pTargetModule = this->_eventModuleGetter(in_szName.Str());
        if (pTargetModule == NULL)
        {
            HE_PG_LOG_LINE(HE_STR_TEXT("指定したモジュール(%s)が存在しない"), in_szName.Str());
            return NULL;
        }

        return pTargetModule;
    }

#ifdef HE_ENGINE_DEBUG
    HE::Bool ModuleBase::_ValidateDependenceModule()
    {
        for (HE::Uint32 i = 0; i < this->_vDependenceModule.Size(); ++i)
        {
            // 依存モジュールに設定している依存モジュールが自身の場合はエラー
            // 依存の循環参照は認めない
            const auto& vCheckModule = this->_vDependenceModule[i]->_vDependenceModule;
            for (HE::Uint32 j = 0; j < vCheckModule.Size(); ++j)
            {
                if (this->_szName == vCheckModule[j]->Name())
                {
                    HE_PG_LOG_LINE(HE_STR_TEXT(
                                       "%s モジュールに依存している %s と循環参照関係になっている"),
                                   this->_szName.Str(), vCheckModule[j]->Name());
                    return FALSE;
                }
            }
        }

        return TRUE;
    }
#endif

    Core::Memory::SharedPtr<ModuleBase> ModuleManager::Get(const HE::Char* in_szName) const
    {
        if (this->_mAppModule.Contains(in_szName))
        {
            return this->_mAppModule.FindKey(in_szName)->_data;
        }

        if (this->_mLogicModule.Contains(in_szName))
        {
            return this->_mLogicModule.FindKey(in_szName)->_data;
        }

        if (this->_mViewModule.Contains(in_szName))
        {
            return this->_mViewModule.FindKey(in_szName)->_data;
        }

        return NULL;
    }

    HE::Bool ModuleManager::Release()
    {
        // 全モジュール解放
        // TODO: 依存関係に応じてモジュールを破棄する順序を変える事はできないか？
        {
            for (HE::Uint32 i = 0; i < this->_vViewModuleProcess.Size(); ++i)
            {
                this->_vViewModuleProcess[i]->_VRelease();
            }
            this->_vViewModuleProcess.Clear();

            for (HE::Uint32 i = 0; i < this->_vLogicModuleProcess.Size(); ++i)
            {
                this->_vLogicModuleProcess[i]->_VRelease();
            }
            this->_vLogicModuleProcess.Clear();

            for (HE::Uint32 i = 0; i < this->_vAppModuleProcess.Size(); ++i)
            {
                this->_vAppModuleProcess[i]->_VRelease();
            }
            this->_vAppModuleProcess.Clear();
        }

        // 全モジュールメモリ破棄
        {
            for (auto b = this->_mViewModule.Begin(); b != this->_mViewModule.End(); ++b)
            {
                b->_data.reset();
            }
            this->_mViewModule.Clear();

            for (auto b = this->_mLogicModule.Begin(); b != this->_mLogicModule.End(); ++b)
            {
                b->_data.reset();
            }
            this->_mLogicModule.Clear();

            for (auto b = this->_mAppModule.Begin(); b != this->_mAppModule.End(); ++b)
            {
                b->_data.reset();
            }
            this->_mAppModule.Clear();
        }

        return TRUE;
    }

    void ModuleManager::BeforeUpdate(const HE::Float32 in_fDeltaTime)
    {
        for (HE::Uint32 i = 0; i < this->_vAppModuleProcess.Size(); ++i)
        {
            this->_vAppModuleProcess[i]->_VBeforeUpdate(in_fDeltaTime);
        }

        for (HE::Uint32 i = 0; i < this->_vLogicModuleProcess.Size(); ++i)
        {
            this->_vLogicModuleProcess[i]->_VBeforeUpdate(in_fDeltaTime);
        }

        for (HE::Uint32 i = 0; i < this->_vViewModuleProcess.Size(); ++i)
        {
            this->_vViewModuleProcess[i]->_VBeforeUpdate(in_fDeltaTime);
        }
    }

    void ModuleManager::Update(const HE::Float32 in_fDeltaTime)
    {
        for (HE::Uint32 i = 0; i < this->_vLogicModuleProcess.Size(); ++i)
        {
            this->_vLogicModuleProcess[i]->_VUpdate(in_fDeltaTime);
        }

        for (HE::Uint32 i = 0; i < this->_vViewModuleProcess.Size(); ++i)
        {
            this->_vViewModuleProcess[i]->_VUpdate(in_fDeltaTime);
        }

        // ロジックとビューのモジュール更新結果を元にアプリモジュールは更新される
        for (HE::Uint32 i = 0; i < this->_vAppModuleProcess.Size(); ++i)
        {
            this->_vAppModuleProcess[i]->_VUpdate(in_fDeltaTime);
        }
    }

    void ModuleManager::LateUpdate(const HE::Float32 in_fDeltaTime)
    {
        for (HE::Uint32 i = 0; i < this->_vViewModuleProcess.Size(); ++i)
        {
            this->_vViewModuleProcess[i]->_VLateUpdate(in_fDeltaTime);
        }

        for (HE::Uint32 i = 0; i < this->_vLogicModuleProcess.Size(); ++i)
        {
            this->_vLogicModuleProcess[i]->_VLateUpdate(in_fDeltaTime);
        }

        for (HE::Uint32 i = 0; i < this->_vAppModuleProcess.Size(); ++i)
        {
            this->_vAppModuleProcess[i]->_VLateUpdate(in_fDeltaTime);
        }
    }

    /// <summary>
    /// モジュールを追加
    /// </summary>
    HE::Bool ModuleManager::AddModule(Core::Memory::SharedPtr<ModuleBase> in_spModule)
    {
        // モジュールレイヤーに応じたリストに登録
        const auto eLayer = in_spModule->Layer();
        if (eLayer == ELayer_App)
        {
            this->_mAppModule.Add(in_spModule->Name(), in_spModule);
        }
        else if (eLayer == ELayer_Logic)
        {
            this->_mLogicModule.Add(in_spModule->Name(), in_spModule);
        }
        else if (eLayer == ELayer_View)
        {
            this->_mViewModule.Add(in_spModule->Name(), in_spModule);
        }
        else
        {
            return FALSE;
        }

        // TODO: モジュール内で依存しているモジュールを呼び出すためにモジュール取得するイベントを設定
        in_spModule->_eventModuleGetter =
            std::move([this](const HE::Char* in_szName) { return this->Get(in_szName); });

        return TRUE;
    }

    HE::Bool ModuleManager::Start()
    {
        for (HE::Uint32 i = Module::ELayer::ELayer_App; i <= Module::ELayer::ELayer_Logic; ++i)
        {
            switch (i)
            {
                case ELayer_App:
                {
                    // AppモジュールはプラットフォームなどのOS関連のモジュール
                    // newでメモリ確保とかもあり得る
                    // ゲーム特有処理の前段となる
                    for (auto b = this->_mAppModule.Begin(); b != this->_mAppModule.End(); ++b)
                    {
                        if (this->_StartModule(*b->_data))
                        {
                            // モジュールはこの管理クラスのみで扱っていて外部参照で壊す事がないアドレスを直接参照しても問題ない
                            this->_vAppModuleProcess.PushBack(b->_data.get());
                        }
                    }
                    this->_SortModuleVector(&this->_vAppModuleProcess);

                    break;
                }
                case ELayer_Logic:
                {
                    for (auto b = this->_mLogicModule.Begin(); b != this->_mLogicModule.End(); ++b)
                    {
                        if (this->_StartModule(*b->_data))
                        {
                            // モジュールはこの管理クラスのみで扱っていて外部参照で壊す事がないアドレスを直接参照しても問題ない
                            this->_vLogicModuleProcess.PushBack(b->_data.get());
                        }
                    }

                    this->_SortModuleVector(&this->_vLogicModuleProcess);
                    break;
                }
                case ELayer_View:
                {
                    for (auto b = this->_mViewModule.Begin(); b != this->_mViewModule.End(); ++b)
                    {
                        if (this->_StartModule(*b->_data))
                        {
                            // モジュールはこの管理クラスのみで扱っていて外部参照で壊す事がないアドレスを直接参照しても問題ない
                            this->_vViewModuleProcess.PushBack(b->_data.get());
                        }
                    }

                    this->_SortModuleVector(&this->_vViewModuleProcess);
                    break;
                }
            }
        }

        return TRUE;
    }

    void ModuleManager::_SortModuleVector(ModuleProcessBaseVector* out)
    {
        if (out->Size() <= 1) return;

        // モジュール数が3桁もいかないので高速ソートは不要
        for (HE::Uint32 i = 0; i < out->Size() - 1; ++i)
        {
            HE::Uint32 nowPriority = (*out)[i]->Priority();
            for (HE::Uint32 j = i; j < out->Size(); ++j)
            {
                HE::Uint32 cmpPriority = (*out)[j]->Priority();
                if (cmpPriority < nowPriority)
                {
                    auto tmp  = (*out)[i];
                    (*out)[i] = (*out)[j];
                    (*out)[j] = tmp;
                }
            }
        }
    }

    HE::Bool ModuleManager::_StartModule(ModuleBase& in_rModule)
    {
        HE_LOG_LINE(HE_STR_TEXT("Start Module(%s)"), in_rModule.Name());
        if (in_rModule._VStart() == FALSE)
        {
            HE_ASSERT(FALSE && "モジュール開始に失敗");
        }

        // 依存情報を見る
        auto& vDependenceModuleNames = in_rModule.GetDependenceModuleNames();
        if (0 < vDependenceModuleNames.Size())
        {
            // 依存しているモジュールリストを作成
            for (HE::Uint32 i = 0; i < vDependenceModuleNames.Size(); ++i)
            {
                auto szName             = vDependenceModuleNames[i];
                auto spDependenceModule = this->Get(szName.Str());
                if (spDependenceModule == NULL) continue;

#ifdef HE_ENGINE_DEBUG
                in_rModule._vDependenceModule.PushBack(spDependenceModule.get());
#endif
            }
#ifdef HE_ENGINE_DEBUG
            HE_ASSERT(in_rModule._ValidateDependenceModule() && "依存設定が循環になっている");
#endif
        }

        return TRUE;
    }

}  // namespace Module
