@echo off
chcp 65001

REM cpp/h/hppファイルのコード整理を一括で行うバッチファイル

setlocal

REM エンジンにあるコード整理を行うスクリプトを起動
call .\HobbyEngine\Script\CodeCleanup\CodeCleanup.bat .\
pause & exit /b %errorlevel%

endlocal

REM 正常終了
echo "all complete" & pause & exit /b 0