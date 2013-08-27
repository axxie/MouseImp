@echo off
set datetime=%date%__%time%
set datetime=%datetime: =%
set datetime=%datetime::=_%
set datetime=%datetime:,=_%
set OutName=Mimp_Release_Win32_%datetime%.log
set OutName64=Mimp_Release_x64_%datetime%.log

@echo on
"%VS100COMNTOOLS%\..\IDE\devenv" MImp.sln /rebuild "Release|Win32" /Out %OutName% >nul 2>nul
@if errorlevel 1 goto prep_build_error
"%VS100COMNTOOLS%\..\IDE\devenv" MImp.sln /rebuild "Release|x64" /Out %OutName64% >nul 2>nul
@if errorlevel 1 goto prep_build_error64

@echo B^> Incrementing build number
for /f "tokens=*" %%i in ('cscript //nologo IncrementBuildNumber.vbs') do set Version=%%i

"%VS100COMNTOOLS%\..\IDE\devenv" MImp.sln /rebuild "Release|Win32" /Out %OutName% >nul 2>nul
@if errorlevel 1 goto error
"%VS100COMNTOOLS%\..\IDE\devenv" MImp.sln /rebuild "Release|x64" /Out %OutName64% >nul 2>nul
@if errorlevel 1 goto error64

@echo B^> Creating backup
rar a -r -x*.user -x*.ncb -x*.aps -x*.suo -xIntermediate -x.hg -xbin\Win32\Debug -xbin\x64\Debug ..\..\bkp\MouseImp_%Version%_src.rar ..\*.*

@echo B^> Copying .msi files
copy ..\bin\win32\release\en-US\MouseImp.msi .\MouseImp%Version%.msi
copy ..\bin\x64\release\en-US\MouseImp.msi .\MouseImp%Version%_x64.msi

@echo B^> Packing .msi files
rar a -hppassword MouseImp_%Version%.rar MouseImp%Version%.msi MouseImp%Version%_x64.msi

@echo B^> BUILD SUCCEEDED

@goto exit

:prep_build_error64
@echo B^> x64 preparation build failed, skiping version increment
:error64
@echo B^> X64 BUILD FAILED, SEE %OutName64%
@goto exit

:prep_build_error
@echo B^> Preparation build failed, skiping version increment
:error
@echo B^> BUILD FAILED, SEE %OutName%

:exit
@echo B^> End of execution