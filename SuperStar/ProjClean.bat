@echo off
chcp 65001

REM プロジェクトのクリーン処理

setlocal

REM  プロジェクトフォルダを削除

set BUILD_DIR="Build\"
if exist %BUILD_DIR% (
    rmdir /s /q %BUILD_DIR%
    echo "delete build dir"
) else (
    echo "error delete build dir"
)

REM 実行ファイルを収めてフォルダを削除
set BIN_DIR="Bin\"
if exist %BIN_DIR% (
    rmdir /s /q %BIN_DIR%
    echo "delete bin dir"
) else (
    echo "error delete bin dir"
)

endlocal

REM 正常終了
echo "all complete" & pause & exit /b 0

