#include "PlatformSDL2/SDL2Input.h"

#include "Engine/Platform/PlatformGraphic.h"
#include "SDL2/SDL.h"

// DxLib用の入力システムの実装

namespace PlatformSDL2
{
    namespace Local
    {
        constexpr HE::Uint32 aGameKeyMap[Platform::EKeyboard::EKeyboard_MAX] = {
            SDL_SCANCODE_BACKSPACE,
            SDL_SCANCODE_TAB,
            SDL_SCANCODE_RETURN,
            SDL_SCANCODE_LSHIFT,
            SDL_SCANCODE_RSHIFT,
            SDL_SCANCODE_LCTRL,
            SDL_SCANCODE_RCTRL,
            SDL_SCANCODE_ESCAPE,
            SDL_SCANCODE_SPACE,
            SDL_SCANCODE_PAGEUP,
            SDL_SCANCODE_PAGEDOWN,
            SDL_SCANCODE_END,
            SDL_SCANCODE_HOME,
            SDL_SCANCODE_LEFT,
            SDL_SCANCODE_UP,
            SDL_SCANCODE_RIGHT,
            SDL_SCANCODE_DOWN,
            SDL_SCANCODE_INSERT,
            SDL_SCANCODE_DELETE,
            SDL_SCANCODE_MINUS,
            SDL_SCANCODE_BACKSLASH,
            SDL_SCANCODE_KP_POWER,
            SDL_SCANCODE_PERIOD,
            SDL_SCANCODE_SLASH,
            SDL_SCANCODE_LALT,
            SDL_SCANCODE_RALT,
            SDL_SCANCODE_SCROLLLOCK,
            SDL_SCANCODE_SEMICOLON,
            SDL_SCANCODE_PERIOD,
            SDL_SCANCODE_LEFTBRACKET,
            SDL_SCANCODE_RIGHTBRACKET,
            SDL_SCANCODE_KP_AT,
            SDL_SCANCODE_BACKSLASH,
            SDL_SCANCODE_COMMA,
            SDL_SCANCODE_LANG5,
            SDL_SCANCODE_LANG2,
            // TODO: NoConvertがないようなのでConvertと同じにした
            SDL_SCANCODE_LANG2,
            // TODO: カナとひらがなが別になっているはどういうことだろうか？
            SDL_SCANCODE_LANG3,
            SDL_SCANCODE_APPLICATION,
            SDL_SCANCODE_CAPSLOCK,
            SDL_SCANCODE_SYSREQ,
            SDL_SCANCODE_PAUSE,
            SDL_SCANCODE_COMPUTER,
            SDL_SCANCODE_COMPUTER,
            SDL_SCANCODE_NUMLOCKCLEAR,
            SDL_SCANCODE_KP_0,
            SDL_SCANCODE_KP_1,
            SDL_SCANCODE_KP_2,
            SDL_SCANCODE_KP_3,
            SDL_SCANCODE_KP_4,
            SDL_SCANCODE_KP_5,
            SDL_SCANCODE_KP_6,
            SDL_SCANCODE_KP_7,
            SDL_SCANCODE_KP_8,
            SDL_SCANCODE_KP_9,
            SDL_SCANCODE_KP_MULTIPLY,
            SDL_SCANCODE_KP_PLUS,
            SDL_SCANCODE_KP_PLUSMINUS,
            SDL_SCANCODE_KP_DECIMAL,
            SDL_SCANCODE_KP_DIVIDE,
            SDL_SCANCODE_KP_ENTER,
            SDL_SCANCODE_F1,
            SDL_SCANCODE_F2,
            SDL_SCANCODE_F3,
            SDL_SCANCODE_F4,
            SDL_SCANCODE_F5,
            SDL_SCANCODE_F6,
            SDL_SCANCODE_F7,
            SDL_SCANCODE_F8,
            SDL_SCANCODE_F9,
            SDL_SCANCODE_F10,
            SDL_SCANCODE_F11,
            SDL_SCANCODE_F12,
            SDL_SCANCODE_A,
            SDL_SCANCODE_B,
            SDL_SCANCODE_C,
            SDL_SCANCODE_D,
            SDL_SCANCODE_E,
            SDL_SCANCODE_F,
            SDL_SCANCODE_G,
            SDL_SCANCODE_H,
            SDL_SCANCODE_I,
            SDL_SCANCODE_J,
            SDL_SCANCODE_K,
            SDL_SCANCODE_L,
            SDL_SCANCODE_M,
            SDL_SCANCODE_N,
            SDL_SCANCODE_O,
            SDL_SCANCODE_P,
            SDL_SCANCODE_Q,
            SDL_SCANCODE_R,
            SDL_SCANCODE_S,
            SDL_SCANCODE_T,
            SDL_SCANCODE_U,
            SDL_SCANCODE_V,
            SDL_SCANCODE_W,
            SDL_SCANCODE_X,
            SDL_SCANCODE_Y,
            SDL_SCANCODE_Z,
            SDL_SCANCODE_0,
            SDL_SCANCODE_1,
            SDL_SCANCODE_2,
            SDL_SCANCODE_3,
            SDL_SCANCODE_4,
            SDL_SCANCODE_5,
            SDL_SCANCODE_6,
            SDL_SCANCODE_7,
            SDL_SCANCODE_8,
            SDL_SCANCODE_9,
        };

        class InputObject : public Platform::InputObject
        {
        public:
            InputObject() : Platform::InputObject()
            {
                // キー入力初期化
                {
                    for (HE::Uint32 i = 0; i < Platform::EKeyboard::EKeyboard_MAX; ++i)
                    {
                    }

                    ::memset(this->_keyboard._aPrevState, Platform::EInputState::EInputState_NONE,
                             HE_ARRAY_SIZE(this->_keyboard._aPrevState));
                }

                // タッチ入力初期化
                {
                    int x                 = 0;
                    int y                 = 0;
                    this->_touch._pos._fX = static_cast<HE::Float32>(x);
                    this->_touch._pos._fY = static_cast<HE::Float32>(y);

                    this->_uPrevButton = Platform::EInputState::EInputState_NONE;
                }
            }

            void Update()
            {
                // キーボード入力取得
                const ::Uint8* pKeyboardState = ::SDL_GetKeyboardState(nullptr);

                // キー入力の処理
                {
                    for (HE::Uint32 i = 0; i < Platform::EKeyboard::EKeyboard_MAX; ++i)
                    {
                        // キーの前情報を設定
                        this->_keyboard._aPrevState[i] = this->_keyboard._aCurrState[i];

                        // キーの新しい情報を設定
                        this->_keyboard._aCurrState[i] = 0;
                        if (pKeyboardState[Local::aGameKeyMap[i]])
                            this->_keyboard._aCurrState[i] = 1;
                    }
                }

                // タッチ入力
                {
                    int x                = 0;
                    int y                = 0;
                    ::Uint32 mouseButton = ::SDL_GetMouseState(&x, &y);

                    this->_touch._pos._fX = static_cast<HE::Float32>(x);
                    this->_touch._pos._fY = static_cast<HE::Float32>(y);

                    this->_uPrevButton = this->_touch._uCurrTouchState;
                    this->_uCurrButton = mouseButton;

                    this->_touch._uPrevTouchState = 0;
                    this->_touch._uCurrTouchState = 0;

                    if (this->_uPrevButton & SDL_BUTTON_LMASK)
                        this->_touch._uPrevTouchState |= Platform::EInputMouseType_Left;
                    if (this->_uPrevButton & SDL_BUTTON_RMASK)
                        this->_touch._uPrevTouchState |= Platform::EInputMouseType_Right;
                    if (this->_uPrevButton & SDL_BUTTON_MMASK)
                        this->_touch._uPrevTouchState |= Platform::EInputMouseType_Middle;

                    if (this->_uCurrButton & SDL_BUTTON_LMASK)
                        this->_touch._uCurrTouchState |= Platform::EInputMouseType_Left;
                    if (this->_uCurrButton & SDL_BUTTON_RMASK)
                        this->_touch._uCurrTouchState |= Platform::EInputMouseType_Right;
                    if (this->_uCurrButton & SDL_BUTTON_MMASK)
                        this->_touch._uCurrTouchState |= Platform::EInputMouseType_Middle;
                }
            }

        private:
            HE::Uint32 _uCurrButton = 0;
            HE::Uint32 _uPrevButton = 0;
        };
    }  // namespace Local

    Input::Input()
    {
        this->_poolInputObject.POOL_RESERVE_POOL(32);
    }

    void Input::VRelease()
    {
        this->_poolInputObject.ReleasePool();
    }

    void Input::VUpdate(const HE::Float32 in_fDeltaTime)
    {
        SDL_Event eventData;
        SDL_zero(eventData);

        // SDLシステム内で発生したイベントはすべてキューに積まれている
        // キューに積まれたイベントを取得して分岐処理している

        auto map = this->_poolInputObject.GetUserDataList();
        while (::SDL_PollEvent(&eventData))
        {
            // 入力結果を他モジュールに通知
            {
                for (auto it = map->begin(); it != map->end(); ++it)
                {
                    it->second->Event(reinterpret_cast<void*>(&eventData));
                }
            }
        }

        // 入力更新　
        for (auto it = map->begin(); it != map->end(); ++it)
        {
            Local::InputObject* pObj = reinterpret_cast<Local::InputObject*>(it->second);
            pObj->Update();
        }
    }

    const Core::Common::Handle Input::VCreateObject()
    {
        auto [handle, obj] = this->_poolInputObject.Alloc<Local::InputObject>();

        return handle;
    }

    void Input::VReleaseObject(Core::Common::Handle& in_rHandle)
    {
        this->_poolInputObject.Free(in_rHandle, TRUE);
    }

    Platform::InputObject* Input::GetObj(const Core::Common::Handle in_handle)
    {
        auto pObj = reinterpret_cast<Local::InputObject*>(this->_poolInputObject.Ref(in_handle));
        HE_ASSERT_RETURN_VALUE(NULL, pObj);
        return pObj;
    }

}  // namespace PlatformSDL2
