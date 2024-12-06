@echo off
chcp 65001

REM 開発構築をバッチ作業で自動化
REM 64bit環境向け

setlocal

net session >nul 2>&1
if %errorlevel% neq 0 (
    powershell -Command "Start-Process cmd -ArgumentList '/c \"%~f0\"' -Verb RunAs"
    exit /b
)

REM エンジンを使うための環境構築
set SCRIPT_PATH=%~dp0HobbyEngine\Script\Build\

REM プロジェクトフォルダパスを指定
call "%SCRIPT_PATH%EnvBuild.bat" %~dp0
if %errorlevel% neq 0 pause & exit /b %errorlevel%

REM アプリ専用の環境構築

endlocal

REM 正常終了
echo "all complete" & pause & exit /b 0
