REM Copy assets.
xcopy /y ..\..\VKTS_Binaries\shader\SPIR\V\font.vert.spv .\assets\shader\SPIR\V\
xcopy /y ..\..\VKTS_Binaries\shader\SPIR\V\font.frag.spv .\assets\shader\SPIR\V\
xcopy /y ..\..\VKTS_Binaries\font\*.fnt .\assets\font\
xcopy /y ..\..\VKTS_Binaries\font\*.tga .\assets\font\
xcopy /y ..\..\VKTS_Binaries\test\cycles\*.spv .\assets\test\cycles\
xcopy /y ..\..\VKTS_Binaries\test\cycles\*.vkts .\assets\test\cycles\
xcopy /y ..\..\VKTS_Binaries\test\cycles\*.tga .\assets\test\cycles\

cd jni
call ndk-build
cd ..
call ant debug
call adb install -r bin/NativeActivity-debug.apk
