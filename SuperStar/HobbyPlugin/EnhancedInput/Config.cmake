set(ENHANCED_INPUT_PLUGIN_NAME HobbyEnginePlugin_EnhancedInput)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(ENHANCED_INPUT_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md

    EnhancedInputModule.cpp
    EnhancedInputModule.h

    EnhancedInput/Common.h
    EnhancedInput/Event.h

    EnhancedInput/Actor/ActorManagerDecorater.cpp
    EnhancedInput/Actor/ActorManagerDecorater.h
    EnhancedInput/Actor/InputComponent.h
    EnhancedInput/Actor/InputComponent.cpp
)
