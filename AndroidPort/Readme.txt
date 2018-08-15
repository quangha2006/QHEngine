How to compile for android platform.
1. open AndroidPort\AndroidSource\scripts\bff\Main_Paths.bff
2. change .NDK_Root =
3. open AndroidPort\make_Android.bat
4. change ANDROID_HOME=
5. unzip AndroidPort\precompiledLib\release\armeabi-v7a\libAssimp.zip
6. run make_Android.bat
7. press 3 to build source and press 7 to push data to device.