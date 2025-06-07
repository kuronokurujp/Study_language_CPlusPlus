# プラグインビルドの共通処理.
include(${CMAKE_CURRENT_SOURCE_DIR}/../Plugin.cmake NO_POLICY_SCOPE)
# Renderプラグインの共通情報.
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

message("build enchancde input plugin lib!")

# 共有ライブラリとして生成するためのオプションの定義.
option(ENHANCED_INPUT_PLUGIN_BUILD_SHARED_LIBS "build shared library" OFF)

# プラグイン共通ビルド処理.
common_build_plugin("${ENHANCED_INPUT_PLUGIN_NAME}" "${ENHANCED_INPUT_PLUGIN_TARGET_FILES}" "$CACHE{CACHE_ENGINE_INCLUDE_CODE}" ${ENHANCED_INPUT_PLUGIN_BUILD_SHARED_LIBS})

target_include_directories(${ENHANCED_INPUT_PLUGIN_NAME}
    PRIVATE
        # プラグインに依存しているプラグインコード
        ${CMAKE_CURRENT_SOURCE_DIR}/../Event/
        ${CMAKE_CURRENT_SOURCE_DIR}/../Actor/
)
