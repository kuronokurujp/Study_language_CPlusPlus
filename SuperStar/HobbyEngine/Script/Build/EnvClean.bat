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
set VCPKG_PATH=%PROJ_PATH%\vcpkg
if exist %VCPKG_PATH% (
    echo "uninstall vcpkg %VCPKG_PATH%"
    rmdir /s /q %VCPKG_PATH%
)

REM vcpkgの設定ファイルがあれば削除
set VCPKG_CONFIG_FILE_PATH=%PROJ_PATH%\vcpkg-configuration.json
if exist %VCPKG_CONFIG_FILE_PATH% (
    echo "del %VCPKG_CONFIG_FILE_PATH%"
    del %VCPKG_CONFIG_FILE_PATH%
)

exit /b 0

endlocal
