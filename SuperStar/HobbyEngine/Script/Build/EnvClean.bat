@echo off

REM エンジンの環境構築の破棄

setlocal

set PROJ_PATH=%1
if not exist %PROJ_PATH% (
    echo "not exist proj path %PROJ_PATH%" 
    exit /b 1
)

REM vcpkgでインストールしたのを全てアンインストール
REM その後にvcpkgを破棄
set INSTALL_VCPKG_DIR_PATH=%PROJ_PATH%\ThirdParty
set VCPKG_PATH=%INSTALL_VCPKG_DIR_PATH%\vcpkg
if exist %VCPKG_PATH% (
    echo "uninstall vcpkg %VCPKG_PATH%"
    rmdir /s /q %VCPKG_PATH%
)

REM 追加した環境変数を削除
if defined CLANG_FORMAT_PATH (
    setx CLANG_FORMAT_PATH ""
    reg delete "HKEY_CURRENT_USER\Environment" /v CLANG_FORMAT_PATH /f
    echo "delete env CLANG_FORMAT_PATH"
)

if defined CLANG_TIDY_PATH (
    setx CLANG_TIDY_PATH "" 
    reg delete "HKEY_CURRENT_USER\Environment" /v CLANG_TIDY_PATH /f
    echo "delete env CLANG_TIDY_PATH"
)

exit /b 0

endlocal
