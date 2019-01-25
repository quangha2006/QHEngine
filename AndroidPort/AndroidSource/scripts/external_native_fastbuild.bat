@echo off

setlocal EnableExtensions EnableDelayedExpansion

set ROOT_DIR=%~dp0..\..

set FAST_BUILD_CONFIG=!ROOT_DIR!\AndroidSource\scripts\bff
set FAST_BUILD_COMPILE=!FAST_BUILD_CONFIG!\Compile
set FAST_BUILD_GENERATED=!ROOT_DIR!\generated
set FAST_BUILD_LINK=!FAST_BUILD_CONFIG!\Link
set FAST_BUILD_TOOL=%ROOT_DIR%\..\Tools\FastBuild\FBuild.exe
set SO_LIB_FILE=native-activity

rem uncomment the line below to see the entire command used on this .bat script
REM echo COMMAND: %0 %1 %2 %3
set build_type=release
set architectures=armeabi-v7a
set architecture=ARM
set jniLibs_path=!FAST_BUILD_CONFIG!\..\..\app\libs

set FASTBUILD_BROKERAGE_PATH=\\SA2BLD0010\FastBuild_Detection\
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
	!FAST_BUILD_TOOL! -cachewrite -cacheread -summary All-Compile-!architecture!-!build_type! -clean
	popd

	rem Check if COMPILE step is OK
	if NOT "!errorlevel!"=="0" (
	   echo COMPILE ERROR code returned by fbuild.exe, target All-Compile-!architecture!-!build_type!: !errorlevel!
	   pause
		exit !errorlevel!
	)
	rem the COMPILE step was OK
	rem Check if re-linking is required.
	rem 1) Check with the link command, not using -clean flag first
	rem this covers the case when the .a files are still the same but there are changes in the link build_typeuration
	rem the Check will be made with TimeStamps


	rem LINK 1
	rem trigger linking without the -clean flag
	echo.
	echo.
	echo 2^) --- LINK ---
	rem Hoa keep strip-so
	pushd !FAST_BUILD_LINK!
	!FAST_BUILD_TOOL! -clean !SO_LIB_FILE!-SharedLib-!architecture!-!build_type! Strip-!SO_LIB_FILE!-!architecture!-!build_type!
	popd

	echo.
	echo.
	echo 3^) --- CHECK SHARED LIB ---
	echo Check if lib!SO_LIB_FILE!.so has been updated by the Link command

	if NOT "!errorlevel!"=="0" (
		echo LINK ERROR first Link Command :: code returned by fbuild.exe, target !SO_LIB_FILE!-SharedLib-!architecture!-!build_type!: !errorlevel!
		pause
		exit !errorlevel!
	)
	rem Link was either succesfull or it was not triggered (because FBuild.exe considered wronlgy that all the libs required for the shared lib target are already up-to-date)
	rem if .so does not exist the link will alwyas be triggered
	rem also, the link is triggered if the .bff files change (any change that influences the fbuild.bff from Link folder will be considerd, even it is from an #included file)

	if exist "!FAST_BUILD_GENERATED!\!architecture!\!build_type!\SharedLibs\Unstripped_Current_TimeStamps.txt" (

		call:compareTimeStamps !FAST_BUILD_GENERATED!\!architecture!\!build_type!\SharedLibs Unstripped
		set SHARED_LIBS_RESULT=!ERRORLEVEL!
		call:clearErrorLevel

		echo Shared Lib TimeStamps compare result: !SHARED_LIBS_RESULT!
		if "!SHARED_LIBS_RESULT!"=="-1" (
			echo 	Invalid syntax e.g. only one file passed. !SHARED_LIBS_RESULT!
			pause
			exit !errorlevel!
		) else if "!SHARED_LIBS_RESULT!"=="0" (
			echo 	The files are identical
			echo 	Link command was not triggered
			echo 	Will check if there are changes in the Static Libs
			set DO_STATIC_LIBS_COMPARE=TRUE
		) else if "!SHARED_LIBS_RESULT!"=="1" (
			echo 	The files are different.
			echo 	Check for static libs is not needed, the link was already triggered
			
			rem generate new StaticLibs Current
			pushd !FAST_BUILD_GENERATED!\!architecture!\!build_type!\StaticLibs
			forfiles /m *.* /c "cmd /c ECHO @file @ftime" > ..\StaticLibs_Current_TimeStamps.txt
			popd
			
		) else if "!SHARED_LIBS_RESULT!"=="2" (
			echo 	Cannot find at least one of the files. 
			echo 	THIS SHOULD NOT HAPPEN!
			pause
			exit !errorlevel!
		)

	) else (

		rem Unstripped_Current_TimeStamps.txt DOES NOT EXIST
		rem no previous link was made, the link above was the first link

		echo create Unstripped_Current_TimeStamps.txt
		pushd !FAST_BUILD_GENERATED!\!architecture!\!build_type!\SharedLibs\Unstripped
		forfiles /m *.* /c "cmd /c ECHO @file @ftime" > ..\Unstripped_Current_TimeStamps.txt
		popd

		rem generate new StaticLibs Current
		pushd !FAST_BUILD_GENERATED!\!architecture!\!build_type!\StaticLibs
		forfiles /m *.* /c "cmd /c ECHO @file @ftime" > ..\StaticLibs_Current_TimeStamps.txt
		popd


	)

	if "!DO_STATIC_LIBS_COMPARE!"=="TRUE" (
		echo.
		echo.
		echo 4^) --- CHECK STATIC LIBS ---
		echo If any .a file has been re-saved/modified then the Forced-Linking will be triggered
		if exist "!FAST_BUILD_GENERATED!\!architecture!\!build_type!\StaticLibs_Current_TimeStamps.txt" (
			
			call:compareTimeStamps !FAST_BUILD_GENERATED!\!architecture!\!build_type! StaticLibs
			set STATIC_LIB_RESULT=!ERRORLEVEL!
			call:clearErrorLevel

			echo Static Libs TimeStamps compare result: !STATIC_LIB_RESULT!
			if "!STATIC_LIB_RESULT!"=="-1" (
				echo 	Invalid syntax e.g. only one file passed.
				pause
				exit !errorlevel!
			) else if "!STATIC_LIB_RESULT!"=="0" (		
				echo 	The files are identical.
				echo 	Link with -clean flag command will NOT be triggered		
			) else if "!STATIC_LIB_RESULT!"=="1" (
				echo 	The files are different.
				echo 	Forced-Link command ^(with -clean flag^) will BE triggered
				set DO_CLEAN_LINK=TRUE
			) else if "!STATIC_LIB_RESULT!"=="2" (
				echo 	Cannot find at least one of the files.
				echo 	THIS SHOULD NOT HAPPEN!
				pause
				exit !errorlevel!
			)
			
		) else (

			rem StaticLibs_Current_TimeStamps.txt DOES NOT EXIST
			rem no previous link was made
			
			echo 	Need to create StaticLibs_Current_TimeStamps.txt
			echo 	Forced Link Will be triggered
			
			rem create Current
			pushd !FAST_BUILD_GENERATED!\!architecture!\!build_type!\StaticLibs
			forfiles /m *.* /c "cmd /c echo @file @ftime" > ..\StaticLibs_Current_TimeStamps.txt
			popd

			rem trigger linking
			set DO_CLEAN_LINK=TRUE
			
		)
	)


	if "!DO_CLEAN_LINK!"=="TRUE" (
		echo.
		echo.
		echo 5^) --- FORCED LINK ---
		
		rem call linking with -clean flag only if the static libs are confiremed to have changed (this will not get called again if the first link alredy triggered)
		rem Hoa keep strip-so
		pushd !FAST_BUILD_LINK!
		!FAST_BUILD_TOOL! -clean !SO_LIB_FILE!-SharedLib-!architecture!-!build_type! Strip-!SO_LIB_FILE!-!architecture!-!build_type!
		popd

		if NOT "!errorlevel!"=="0" (
			echo LINK ERROR !!! code returned by fbuild.exe, target !SO_LIB_FILE!-SharedLib-!architecture!-!build_type!: !errorlevel!
			pause
			exit !errorlevel!
		)

		rem generate new SharedLib Current
		pushd !FAST_BUILD_GENERATED!\!architecture!\!build_type!\SharedLibs
		forfiles /m *.* /c "cmd /c ECHO @file @ftime" > ..\SharedLibs_Current_TimeStamps.txt
		popd

		
	) else (
		rem echo Linking is not required, command will not be triggered
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


:compareTimeStamps
    set WORK_FOLDER=%1
		
    set COMPARE_CONTENT_FOLDER=%2

    cd !WORK_FOLDER!
    copy !COMPARE_CONTENT_FOLDER!_Current_TimeStamps.txt !COMPARE_CONTENT_FOLDER!_Previous_TimeStamps.txt > nul

    cd !WORK_FOLDER!\!COMPARE_CONTENT_FOLDER!
    forfiles /m *.* /c "cmd /c ECHO @file @ftime" > ..\!COMPARE_CONTENT_FOLDER!_Current_TimeStamps.txt

    cd !WORK_FOLDER!
    fc !COMPARE_CONTENT_FOLDER!_Previous_TimeStamps.txt !COMPARE_CONTENT_FOLDER!_Current_TimeStamps.txt > nul
	set EXIT_CODE=!ERRORLEVEL!
	rem FC will set an ErrorLevel as follows:
	rem 	-1 Invalid syntax (e.g. only one file passed)
	rem 	 0 The files are identical.
	rem 	 1 The files are different.
	rem 	 2 Cannot find at least one of the files.

	cd !WORK_FOLDER!
	exit /b !EXIT_CODE!
goto:eof


:copy_shared_libs
	echo.
	echo.
	echo --- Copy Files ---


	rem -------------- COPY UNSTRIPPED LIB --------------
	if not exist "!jniLibs_path!\!architectures!" (
		rem echo jniLibs_path !jniLibs_path!\%1
		mkdir "!jniLibs_path!\!architectures!"
	)
	copy "!FAST_BUILD_GENERATED!\!architecture!\!build_type!\SharedLibs\lib!SO_LIB_FILE!.so" "!jniLibs_path!\!architectures!\lib!SO_LIB_FILE!.so" > nul
	if "!ERRORLEVEL!"=="1" (
		echo ERROR @ copy lid!SO_LIB_FILE!.so file into 'AndroidFramework\libs' directory
		echo ^^! Will EXIT without having the lid!SO_LIB_FILE!.so updated ^^!
		pause
		exit !errorlevel!
	) else (
		echo    Copied: !FAST_BUILD_GENERATED!\!architecture!\!build_type!\SharedLibs\lib!SO_LIB_FILE!.so
		echo        To: !jniLibs_path!\!architectures!\lib!SO_LIB_FILE!.so
	)

goto:eof


:clearErrorLevel
	exit /b 0
goto:eof


rem =================================================================================

:eof

