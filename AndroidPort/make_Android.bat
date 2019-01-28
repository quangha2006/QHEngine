@echo OFF
set CUR_MPATH=%~dp0
REM set ANDROID_HOME=F:\Android\android-sdk
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
	echo         3. Build APK FULL Release
	echo.
	echo         4. Build APK FULL Debug
	echo.
	echo         5. Build Native
	echo.
	echo         6. Build Java Release
	echo.
	echo         7. Build Java Debug
	echo.
	echo         8. Install APK Release
	echo.
	echo         9. Install APK Debug
	echo.
	echo         10. Push data
	echo.
	echo.
	set /P DO_JOB=.           Enter your choose: 
REM ...................................
cls
echo Do job: %DO_JOB%
if %DO_JOB%==1 (
  echo  ************************ 1. Clean UP ************************
git clean -d -f -fx
git checkout ./
goto END
) else if %DO_JOB%==2 (
echo.
  echo  ************************ 2. Update ************************
  git pull
goto END
) else if %DO_JOB%==3 (
  pushd AndroidSource\scripts\
  call:CleanOldSoFile
  call build_native_java_install.bat Release
  popd
  goto END
) else if %DO_JOB%==4 (
  pushd AndroidSource\scripts\
  call:CleanOldSoFile
  call build_native_java_install.bat Debug
  popd
  goto END
) else if %DO_JOB%==5 (
  pushd AndroidSource\scripts\
  call:CleanOldSoFile
  call external_native_fastbuild.bat
  popd
  goto END
) else if %DO_JOB%==6 (
  pushd AndroidSource\scripts\
  if exist "..\app\libs\*" (
	call build_java.bat Release
  ) else (
  echo "Missing native lib" 
  )
  popd
  goto END
) else if %DO_JOB%==7 (
  pushd AndroidSource\scripts\
  if exist "..\app\libs\*" (
	call build_java.bat Debug
  ) else (
  echo "Missing native lib" 
  )
  popd
  goto END
) else if %DO_JOB%==8 (
  pushd AndroidSource\scripts\
  call install.bat Release
  popd
  goto END
) else if %DO_JOB%==9 (
  pushd AndroidSource\scripts\
  call install.bat Debug
  popd
  goto END
) else if %DO_JOB%==10 (
	pushd %CUR_MPATH%\..
	adb push Resources /sdcard/Android/data/com.android.learnning3D/files/
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