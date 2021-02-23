@echo off

setlocal EnableExtensions EnableDelayedExpansion

set ROOT_DIR=%~dp0..\..

set FAST_BUILD_CONFIG=!ROOT_DIR!\AndroidSource\scripts\bff
set FAST_BUILD_COMPILE=!FAST_BUILD_CONFIG!\Compile
set FAST_BUILD_GENERATED=!ROOT_DIR!\generated
set FAST_BUILD_LINK=!FAST_BUILD_CONFIG!\Link
set FAST_BUILD_TOOL=%ROOT_DIR%\..\Tools\FastBuild\bin\FBuild.exe
set SO_LIB_FILE=native-activity

rem uncomment the line below to see the entire command used on this .bat script
REM echo COMMAND: %0 %1 %2 %3
set build_type=release
set architectures=armeabi-v7a
set architecture=ARM
set jniLibs_path=!FAST_BUILD_CONFIG!\..\..\app\libs

set FASTBUILD_BROKERAGE_PATH=\\gameloft.org\sai\FastBuild_Detection
set DO_STATIC_LIBS_COMPARE=FALSE
set DO_CLEAN_LINK=FALSE

	echo.
	echo *********************************************************************************************
	echo                                  CALL_FAST_BUILD !architecture! !build_type!
	echo *********************************************************************************************
	echo.
REM !FAST_BUILD_TOOL! -cachewrite -cacheread -dist -summary All-Compile-!architecture!-!build_type! -clean // Note: Add -dist to enable distributed
	echo 1^) --- COMPILE ---
	pushd !FAST_BUILD_COMPILE!
	"!FAST_BUILD_TOOL!" -cachewrite -cacheread -summary All-Compile-!architecture!-!build_type! -clean
	popd

	rem Check if COMPILE step is OK
	if NOT "!errorlevel!"=="0" (
	   echo COMPILE ERROR code returned by fbuild.exe, target All-Compile-!architecture!-!build_type!: !errorlevel!
	   REM pause
	   goto:eof
	   exit !errorlevel!
	)
	
	rem LINK 1
	rem trigger linking without the -clean flag
	echo.
	echo.
	echo 2^) --- LINK ---
	pushd !FAST_BUILD_LINK!
	"!FAST_BUILD_TOOL!" -clean !SO_LIB_FILE!-SharedLib-!architecture!-!build_type! Strip-!SO_LIB_FILE!-!architecture!-!build_type!
	popd

	REM echo.

	if NOT "!errorlevel!"=="0" (
		echo LINK ERROR first Link Command :: code returned by fbuild.exe, target !SO_LIB_FILE!-SharedLib-!architecture!-!build_type!: !errorlevel!
		REM pause
		goto:eof
		exit !errorlevel!
	)


	call:copy_shared_libs %%I
	call:fbuild_finished
)

:end
goto:eof
rem =================================================================================
rem ==== Functions: =================================================================
rem =================================================================================


:fbuild_finished	
	echo.
	echo *********************************************************************************************
	echo                                FAST_BUILD_FINISHED !architecture! %build_type%
	echo *********************************************************************************************
	echo.	
goto:eof


:copy_shared_libs
	echo.
	echo.
	echo 3^) --- Copy Files ---


	rem -------------- COPY UNSTRIPPED LIB --------------
	if not exist "!jniLibs_path!\!architectures!" (
		rem echo jniLibs_path "!jniLibs_path!\%1"
		mkdir "!jniLibs_path!\!architectures!"
	)
	copy "!FAST_BUILD_GENERATED!\!architecture!\!build_type!\SharedLibs\lib!SO_LIB_FILE!.so" "!jniLibs_path!\!architectures!\lib!SO_LIB_FILE!.so" > nul
	if "!ERRORLEVEL!"=="1" (
		echo ERROR @ copy lid!SO_LIB_FILE!.so file into 'AndroidFramework\libs' directory
		echo ^^! Will EXIT without having the lid!SO_LIB_FILE!.so updated ^^!
		REM pause
		goto:eof
		exit !errorlevel!
	) else (
		echo Copied: "!FAST_BUILD_GENERATED!\!architecture!\!build_type!\SharedLibs\lib!SO_LIB_FILE!.so"
		echo     To: "!jniLibs_path!\!architectures!\lib!SO_LIB_FILE!.so"
	)

goto:eof


:clearErrorLevel
	exit /b 0
goto:eof


rem =================================================================================

:eof

