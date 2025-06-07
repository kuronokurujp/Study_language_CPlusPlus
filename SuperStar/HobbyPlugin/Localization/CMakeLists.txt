# プラグインビルドの共通処理.
include(${CMAKE_CURRENT_SOURCE_DIR}/../Plugin.cmake NO_POLICY_SCOPE)
# Renderプラグインの共通情報.
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

message("build localization plugin lib!")

# 共有ライブラリとして生成するためのオプションの定義.
option(LOCALIZATION_PLUGIN_BUILD_SHARED_LIBS "build shared library" OFF)

# プラグイン共通ビルド処理.
common_build_plugin(
    "${LOCALIZATION_PLUGIN_NAME}"
    "${LOCALIZATION_PLUGIN_TARGET_FILES}"
    "$CACHE{CACHE_ENGINE_INCLUDE_CODE}"
    ${LOCALIZATION_PLUGIN_BUILD_SHARED_LIBS}
)

target_include_directories(${LOCALIZATION_PLUGIN_NAME}
    PRIVATE
        # プラグインに依存しているプラグインコード
        # ライブラリのリンクはプロジェクトのCMakeLists.txtでやっている
        ${CMAKE_CURRENT_SOURCE_DIR}/../AssetManager/
)
