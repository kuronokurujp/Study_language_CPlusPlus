@echo off

REM エンジンの環境構築の自動バッチ
REM 64bit環境向け

setlocal

REM 管理者権限で実行しているか
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo "retry admin required"
    exit /b 1
)

set PROJ_PATH=%1
if not exist %PROJ_PATH% (
    echo "not exist proj path %PROJ_PATH%" 
    exit /b 1
)

REM ①chocolateでcmakeをインストール
choco install cmake --version=3.30.0
if %errorlevel% neq 0 exit /b %errorlevel%

REM ②chocolateでvs2022をインストール
choco install visualstudio2022community
if %errorlevel% neq 0 exit /b %errorlevel%

REM ③chocolateでgitをインストール
choco install git --version=2.47.1
if %errorlevel% neq 0 exit /b %errorlevel%

REM chocolateがインストールしたパスを即時反映させる
call refreshenv

REM ④使用するライブラリを簡単にインストールできるようにするためにvcpkgをインストール
REM vcpkgをインストールするためにgitが必要なのでgitをインストールしてコマンド利用
REM すでにvcpkgがインストールしているかチェック
set INSTALL_VCPKG_DIR_PATH=%PROJ_PATH%\ThirdParty
set VCPKG_PATH=%INSTALL_VCPKG_DIR_PATH%\vcpkg
if not exist %VCPKG_PATH% (
    REM インストール先へ移動
    echo "install vcpkg to %INSTALL_VCPKG_DIR_PATH%"
    if not exist %INSTALL_VCPKG_DIR_PATH% (
        mkdir %INSTALL_VCPKG_DIR_PATH%
    )
    cd %INSTALL_VCPKG_DIR_PATH%
    git clone https://github.com/microsoft/vcpkg.git
    if %errorlevel% neq 0 exit /b %errorlevel%
) else (
    echo "exist %VCPKG_PATH%"
)

REM 取得したvcpkgを構築
if exist %VCPKG_PATH% (
    cd %PROJ_PATH%
    call %VCPKG_PATH%\bootstrap-vcpkg.bat
    if %errorlevel% neq 0 exit /b %errorlevel%

) else (
    echo "error not exist %VCPKG_PATH%"
    exit /b 1
)

REM コード整理するツールの環境変数追加
REM VC2022を利用する
REM Clang-Formatのexeファイルパスがあるかチェック
if not defined CLANG_FORMAT_PATH (
    setx CLANG_FORMAT_PATH "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\bin\clang-format.exe"
)

REM Clang-Tidyのexeファイルパスがあるかチェック
if not defined CLANG_TIDY_PATH (
    setx CLANG_TIDY_PATH  "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\bin\clang-tidy.exe"
)

endlocal

exit /b 0
