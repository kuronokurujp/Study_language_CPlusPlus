#pragma once

#include <functional>

#include "Actor/Component/Component.h"
#include "Engine/Common/CustomStack.h"
#include "Engine/Task/TaskManager.h"
#include "Engine/Task/TaskTree.h"

// 最小エンジンのインクルード
#include "Engine/MiniEngine.h"

// 前方宣言
namespace Platform
{
    class InputInterface;
}

namespace Actor
{
    /// <summary>
    /// アクターオブジェクト
    /// </summary>
    class Object : public Core::TaskTree
    {
        HE_CLASS_COPY_NG(Object);
        HE_CLASS_MOVE_NG(Object);
        HE_GENERATED_CLASS_BASE_BODY_HEADER(Object);

    public:
        // アクターのプライベートな点まで管理アクターは参照できるようにする
        friend class ActorManager;

        /// <summary>
        /// Actorの状態.
        /// </summary>
        enum EState
        {
            EState_Active = 0,
            EState_Paused,
            EState_Dead,
        };

        enum EComponentState
        {
            EComponentState_Regist = 1,
            EComponentState_UnRegist,
        };

        using EventComponent = std::function<void(Component*, const EComponentState)>;

    public:
        Object(const HE::Uint32 in_uGroupId);
        virtual ~Object();

        /// <summary>
        /// 生成直後の設定処理
        /// データ初期化はここで行う
        /// </summary>
        virtual void VSetup(const HE::Bool in_bAutoDelete) override;

        /// <summary>
        /// 開始
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        virtual HE::Bool VBegin() override;

        /// <summary>
        /// 終了
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        virtual HE::Bool VEnd() override;

        /// <summar>
        /// 更新
        /// </summary>
        virtual void VBeginUpdate(const HE::Float32 in_fDt);

        /// <summar>
        /// 更新
        /// </summary>
        void VUpdate(const HE::Float32 in_fDt) override;

        /// <summar>
        /// 更新
        /// </summary>
        virtual void VLateUpdate(const HE::Float32 in_fDt);

        /// <summary>
        /// コンポーネントを追加
        /// TArgsにクラスのコンストラクタ引数を渡す
        /// </summary>
        template <class T, typename... TArgs>
        Core::Common::Handle AddComponent(const HE::Uint32 in_uUpdateOrder,
                                          const Component::EPriorty in_ePriorty, TArgs&&... in_args)
        {
            HE_STATIC_ASSERT(std::is_base_of<Component, T>::value,
                             "TクラスはComponentクラスを継承していない");

            auto [h, c] = this->AddComponentByHandleAndComp<T>(in_uUpdateOrder, in_ePriorty,
                                                               std::forward<TArgs>(in_args)...);
            if (h.Null())
            {
                return NullHandle;
            }

            return h;
        }

        /// <summary>
        /// コンポーネント追加
        /// 作成ハンドルと追加コンポーネントを返す
        /// TArgsにクラスのコンストラクタ引数を渡す
        /// </summary>
        template <class T, typename... TArgs>
        std::tuple<Core::Common::Handle, T*> AddComponentByHandleAndComp(
            const HE::Uint32 in_uUpdateOrder, const Component::EPriorty in_ePriorty,
            TArgs&&... in_args)
        {
            HE_STATIC_ASSERT(std::is_base_of<Component, T>::value,
                             "TクラスはComponentクラスを継承していない");

            // TODO: アクターの準備が整う前に呼ばれるケースもある
            // その場合はペンディングリストに追加して準備が整った時にコンポーネントを追加する
            HE_ASSERT(0 <= in_uUpdateOrder);

            // TODO: 更新優先準備による追加処理を指定が必要
            // コンポーネントは確保したメモリを使いまわす
            auto* pCurrentComponents = &this->_components;
            if (in_ePriorty == Component::EPriorty::EPriorty_Late)
            {
                pCurrentComponents = &this->_lateComponents;
            }

            HE_ASSERT(in_uUpdateOrder < static_cast<HE::Uint32>(pCurrentComponents->GetGroupNum()));

            auto handle     = pCurrentComponents->CreateAndAdd<T>(in_uUpdateOrder, FALSE,
                                                                  std::forward<TArgs>(in_args)...);
            auto pComponent = this->GetComponent<T>(handle);
            if (this->_VSetupComponent(pComponent) == FALSE)
            {
                pCurrentComponents->RemoveTask(handle);
                return std::make_tuple(NullHandle, pComponent);
            }

            return std::make_tuple(handle, pComponent);
        }

        /// <summary>
        /// くっつている全てのコンポーネント外す
        /// </summary>
        void RemoveAllComponent();

        /// <summary>
        /// Removes the component.
        /// </summary>
        HE::Bool RemoveComponent(Core::Common::Handle);

        /// <summary>
        /// 親アクターがあればその親アクターのコンポーネントを取得
        /// </summary>
        template <class T>
        T* GetParentComponent()
        {
            HE_STATIC_ASSERT(std::is_base_of<Component, T>::value,
                             "TクラスはComponentクラスを継承していない");

            // 親アクターがないなら即終了
            if (this->_parentActorHandle.Null()) return NULL;

            // 親アクターに目的のコンポーネント取得
            Object* pParentObj = this->_pDataAccess->Get(this->_parentActorHandle);
            HE_ASSERT(pParentObj != NULL);

            T* pFindComp = pParentObj->GetComponent<T>();
            if (pFindComp != NULL) return pFindComp;

            // なければ親アクターの更に親アクターに目的コンポーネントがあるかチェック
            // 再帰処理をしている
            return pParentObj->GetParentComponent<T>();
        }

        const HE::Uint32 GetGropuId() const { return this->_uGroupId; }

        /// <summary>
        /// Gets the state.
        /// </summary>
        EState GetState() const { return this->_eState; }

        /// <summary>
        /// Sets the state.
        /// </summary>
        void SetState(const EState in_eState) { this->_eState = in_eState; }

        void SetEventComponent(EventComponent in_func)
        {
            if (in_func == NULL)
            {
                this->_componentFunction = NULL;
                return;
            }

            this->_componentFunction = std::move(in_func);
        }

        /// <summary>
        /// コンポーネントのアドレスを取得(ハンドル)
        /// </summary>
        template <class T>
        T* GetComponent(const Core::Common::Handle& in_rHandle)
        {
            HE_STATIC_ASSERT(std::is_base_of<Component, T>::value,
                             "TクラスはComponentクラスを継承していない");

            Task* p = this->_components.GetTask(in_rHandle);
            if (p == NULL) p = this->_lateComponents.GetTask(in_rHandle);
            HE_ASSERT(p);

            return reinterpret_cast<T*>(p);
        }

        /// <summary>
        /// コンポーネントのアドレスを取得
        /// テンプレートの型から取得
        /// </summary>
        template <class T>
        T* GetComponent()
        {
            HE_STATIC_ASSERT(std::is_base_of<Component, T>::value,
                             "TクラスはComponentクラスを継承していない");

            auto [handle, p] = this->GetComponentHandleAndComponent(T::StaticRTTI());
            if (handle.Null()) return NULL;

            return reinterpret_cast<T*>(p);
        }

        /// <summary>
        /// RTTIから目的のコンポーネントのハンドルとコンポーネントを取得
        /// </summary>
        std::tuple<Core::Common::Handle, Actor::Component*> GetComponentHandleAndComponent(
            const Core::Common::RTTI*);

        void ForeachComponents(
            std::function<HE::Bool(const Core::Common::Handle&, Component*)> in_func);

        inline HE::Bool ValidComponent(const Core::Common::Handle& in_h)
        {
            if (in_h.Null()) return FALSE;

            if (this->_components.Valid(in_h)) return TRUE;
            return this->_lateComponents.Valid(in_h);
        }

        /// <summary>
        /// アクター自身, 子アクターに設定しているコンポーネントを全て出力
        /// アクターの階層構造が深くなると再帰処理が多くなるので注意
        /// </summary>
        void OutputChildrenComponent(Core::Common::StackBase<Component*>* out,
                                     const Core::Common::RTTI*);

    protected:
        /// <summary>
        /// タスク破棄
        /// </summary>
        virtual void _VDestory() override;

        /// <summary>
        /// 追加したコンポーネントのセットアップ
        /// </summary>
        virtual HE::Bool _VSetupComponent(Component* in_pComp);

    private:
        /// <summary>
        /// くっつている全てのコンポーネント外す
        /// </summary>
        void _RemoveAllComponent(Core::TaskManager*);

        HE::Bool _RemoveComponent(Core::Common::Handle, Core::TaskManager*);

        void _Clear()
        {
            // Actorの状態
            this->_eState = EState_Active;
            this->_components.RemoveAll();
            this->_lateComponents.RemoveAll();
            this->_componentFunction = NULL;
            this->_uGroupId          = Task::uNoneGroupId;
        }

    private:
        // Actorの状態
        EState _eState = EState_Active;

        Core::TaskManager _components;
        Core::TaskManager _lateComponents;

        EventComponent _componentFunction;

        HE::Uint32 _uGroupId;
    };
}  // namespace Actor
