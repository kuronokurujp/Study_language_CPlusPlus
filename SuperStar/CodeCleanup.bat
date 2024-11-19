@echo off

rem cpp/h/hppファイルのコード整理を一括で行うバッチファイル

setlocal

call .\HobbyEngine\Script\CodeCleanup\CodeCleanup.bat .\
pause & exit /b %errorlevel%

endlocal