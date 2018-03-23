@echo off
pushd ..
set ispushdata=%1
REM if ispushdata=="pushdata"
REM (
	REM pushd ..\NewTrainingFramework
	REM adb push Resources /sdcard/
	REM popd
REM )
if exist bin\app-release.apk (
echo.
echo ****************************************
echo ********     Install apk      **********
echo ****************************************
echo.
    call adb install -r bin\app-release.apk
) else (
  echo.    
  echo  *********** Install APK FAILED ************
  echo.
  pause
)  
popd