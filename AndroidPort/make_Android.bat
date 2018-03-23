@echo OFF
set CUR_MPATH=%~dp0
REM set ANDROID_HOME=Y:\android-sdk-windows
:START
cls
color A

	echo.
	echo          ======================
	echo          ======   MENU   ======
	echo          ======================
	echo.
	echo   * WORKING ON: %CUR_MPATH%
	echo.
	echo         1. Clean UP
	echo.
	echo         2. Update
	echo.        
	echo         3. Build APK FULL
	echo.
	echo         4. Build Native
	echo.
	echo         5. Build Java
	echo.
	echo         6. Install APK
	echo.
	echo         7. Push data
	echo.
	echo.
	set /P DO_JOB=.           Enter your choose: 
REM ...................................
cls
echo Do job: %DO_JOB%
if %DO_JOB%==1 (
  echo  ************************ 1. Clean UP ************************
TortoiseProc.exe /path:"%CUR_MPATH%" /command:cleanup /nodlg /noui /noprogressui /revert /delunversioned /delignored /refreshshell /externals /closeonend:2
goto END
) else if %DO_JOB%==2 (
echo.
  echo  ************************ 2. Update ************************
  svn info
  echo =============================
  svn cleanup
  echo Update SVN To Head Revision.
  svn update --non-interactive --trust-server-cert --accept=theirs-full
goto END
) else if %DO_JOB%==3 (
  pushd AndroidSource\scripts\
  call:CleanOldSoFile
  call build_native_java_install.bat
  popd
  goto END
) else if %DO_JOB%==4 (
  pushd AndroidSource\scripts\
  call:CleanOldSoFile
  call external_native_fastbuild.bat
  popd
  goto END
) else if %DO_JOB%==5 (
  pushd AndroidSource\scripts\
  if exist "..\app\libs\*" (
	call build_java.bat
  ) else (
  echo "Missing native lib" 
  )
  popd
  goto END
) else if %DO_JOB%==6 (
  pushd AndroidSource\scripts\
  call install.bat
  popd
  goto END
) else if %DO_JOB%==7 (
	pushd %CUR_MPATH%\..
	adb push Resources /sdcard/
  popd
  goto END
)
:CleanOldSoFile
	pushd %CUR_MPATH%\AndroidSource\app
	if exist "libs" (
		del /s /q "libs\*"
	)
	popd
	goto:eof
:END
cd %CUR_MPATH%
pause
goto START
:eof