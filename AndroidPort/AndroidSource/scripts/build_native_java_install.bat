REM call build_native.bat

call external_native_fastbuild.bat
call build_java.bat
call install.bat
REM echo.
REM echo  ************************ 6. Install APK ************************
REM echo.
REM call install.bat