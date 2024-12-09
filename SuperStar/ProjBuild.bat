@echo off
chcp 65001

setlocal

REM ビルド先の絶対パスを指定.
set BUILD_DIR_FULLPATH=%~dp0
REM cmakeで実行したいCMakeLists.txtがあるディレクトリの絶対パスを指定.
set CMAKE_FILE_DIR_FULLPATH=%~dp0
REM パス連結では2重引用符はつけない.
set SCRIPT_PATH=%~dp0HobbyEngine\Script\Build\

REM プロジェクトをビルド.
call "%SCRIPT_PATH%ProjMSVC2022.bat" %BUILD_DIR_FULLPATH% %CMAKE_FILE_DIR_FULLPATH%
if %errorlevel% neq 0 pause & exit /b %errorlevel%

endlocal

REM 正常終了
echo "all complete" & pause & exit /b 0

