cmake_minimum_required(VERSION 3.26)
# Makeに必要な情報をインストール
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICY_SCOPE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Gameアプリのプロジェクト登録.
add_executable(${APP_NAME} WIN32)

# ファイル群をターゲット設定.
target_sources(${APP_NAME}
  PRIVATE
    ${TARGET_FILES}
)

# プロジェクトが参照できるインクルードディレクトリのパス.
target_include_directories(${APP_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Code)

# ランタイムライブラリを設定
if(MSVC)
  target_compile_options(
    ${APP_NAME}
    PRIVATE
    $<IF:$<CONFIG:Debug>,/MTd,/MT>
  )
endif()

# ビルドのマルチプロセス設定.
target_compile_options(${APP_NAME} PRIVATE /MP)
# Unicode文字セットを設定.
target_compile_options(${APP_NAME} PRIVATE /DUNICODE /D_UNICODE)

# ファイルのフィルタリング設定.
# VSプロジェクト上で見やすくするため.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TARGET_FILES})

# スタートプロジェクトの設定.
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${APP_NAME})

if (MSVC)
  # デバッグ実行時に参照する作業ディレクトリ.
  set_target_properties(${APP_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY
      ${CMAKE_CURRENT_SOURCE_DIR}/Bin/${WORKSPACE_NAME}/${CMAKE_CFG_INTDIR}
  )
endif()

# ライブラリ名をリストにまとめる
set(PLUGIN_LIBRARIES
# TODO: デバッグのみ入れる
#   ${GAMEDEVGUI_PLUGIN_NAME}
# TODO: デバッグのみ入れる
#   ${RUNTIMEDEBUG_PLUGIN_NAME}
  ${LUA_PLUGIN_NAME}
  ${EVENT_PLUGIN_NAME}
  ${ENHANCED_INPUT_PLUGIN_NAME}
#   ${UI_PLUGIN_NAME}
  ${LOCALIZATION_PLUGIN_NAME}
  ${PLUGIN_LEVEL_NAME}
  ${ASSET_MANAGER_PLUGIN_NAME}
  ${RENDER_PLUGIN_NAME}
  ${PLUGIN_ACTOR_NAME}
  ${PLUGIN_PLATFORMSDL2_NAME}
  ${ENGINE_NAME}
)

# ゲームアプリがリンクするライブラリ
# 設定プロジェクトのライブラリ・インクルードディレクトリ登録
# target_link_libraries(${APP_NAME} ${PLUGIN_LIBRARIES})

# バッチファイルを実行するカスタムターゲットを作成
add_custom_target(
  # 必ず実行されるように設定
    RunBatch ALL
    COMMAND cmd /c "${CMAKE_CURRENT_SOURCE_DIR}/UpdateAsset.bat"
    COMMENT "Running UpdateAsset.bat during build."
)

add_dependencies(${APP_NAME} RunBatch)
