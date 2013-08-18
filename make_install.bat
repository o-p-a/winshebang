@echo off
title %~n0
setlocal
pushd "%~dp0"

set CUIBIN=winshebang.exe
set GUIBIN=winshebangw.exe

if not exist "%CUIBIN%" (
	echo %~n0: %CUIBIN% not found
	goto :eof
)
if not exist "%GUIBIN%" (
	echo %~n0: %GUIBIN% not found
	goto :eof
)

call :exec1 C "%USRLOCAL%\bat\AutoHotKey_run"
call :exec1 C "%USRLOCAL%\bat\Hayate_run"
call :exec1 G "%USRLOCAL%\bat\ExtractArchive"
call :exec1 G "%USRLOCAL%\bat\pedTaskTray"
call :exec1 G "%USRLOCAL%\bat\startx"
call :exec1 G "%USRLOCAL%\bat\TeraTerm"
call :exec1 G "%USRLOCAL%\bat\vncviewer"
call :exec1 G "%USRLOCAL%\bat\WebAppExecutor"

call :exec1 C "%USRLOCAL%\bin\_Archive"
call :exec1 C "%USRLOCAL%\bin\BackupWithDate"
call :exec1 C "%USRLOCAL%\bin\btype"
call :exec1 C "%USRLOCAL%\bin\CheckOsVersion"
call :exec1 C "%USRLOCAL%\bin\create_file_list"
call :exec1 C "%USRLOCAL%\bin\danpenkun"
call :exec1 C "%USRLOCAL%\bin\DeleteIfExist"
call :exec1 C "%USRLOCAL%\bin\MakeRename"
call :exec1 C "%USRLOCAL%\bin\MakeShortcut"
call :exec1 C "%USRLOCAL%\bin\NetUse"
call :exec1 C "%USRLOCAL%\bin\NetUseDeleteAll"
call :exec1 C "%USRLOCAL%\bin\NoMore"
call :exec1 C "%USRLOCAL%\bin\renren"
call :exec1 C "%USRLOCAL%\bin\rm2recycle"
call :exec1 C "%USRLOCAL%\bin\TimeSync"
call :exec1 C "%USRLOCAL%\bin\wait_workstation_up"
call :exec1 G "%USRLOCAL%\bin\dpk"

call :exec1 G "%USRLOCAL%\program\WinMergePortable\WinMergePortable"

popd
endlocal
echo [END]
pause>nul
goto :eof

:exec1
if /i "%~1" == "C" (
	set SRC=%CUIBIN%
) else if /i "%~1" == "G" (
	set SRC=%GUIBIN%
) else (
	echo %~n0: unknown action: "%~1"
	goto :eof
)
if exist "%~dpn2" (
	goto found1
) else if exist "%~dpn2.bat" (
	goto found1
) else if exist "%~dpn2.cmd" (
	goto found1
) else if exist "%~dpn2.js" (
	goto found1
) else if exist "%~dpn2.jse" (
	goto found1
) else if exist "%~dpn2.vbs" (
	goto found1
) else if exist "%~dpn2.vbe" (
	goto found1
) else if exist "%~dpn2.jar" (
	goto found1
) else if exist "%~dpn2.class" (
	goto found1
) else (
	echo %~n0: not found target script: "%~2"
	goto :eof
)
:found1
set DST=%~dpn2.exe
echo "%SRC%" -^> "%DST%"
copy /y "%SRC%" "%DST%"
