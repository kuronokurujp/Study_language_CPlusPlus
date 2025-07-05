set(PLUGIN_LOCALIZATION_NAME HobbyPlugin_Localization)
set(PLUGIN_LOCALIZATION_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/Inc)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(PLUGIN_LOCALIZATION_TARGET_COMMON_FILES
    .editorconfig
    LICENSE.txt
    README.md
    )

set(PLUGIN_LOCALIZATION_TARGET_INC_FILES
    Inc/LocalizationModule.h
)

set(PLUGIN_LOCALIZATION_TARGET_SRC_FILES
    Src/LocalizationModule.cpp
)

