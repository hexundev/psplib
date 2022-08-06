@echo off

rem This defines the installation directory, please change it:
set PSPSDKDIR=C:\pspsdk15\psp

echo Files will be copied to the following directory:
echo %PSPSDKDIR%
echo Please verify that it's correct, or edit this batch file.
pause

echo *****************
echo * Archive files *
echo *****************
echo %PSPSDKDIR%\lib
pause
copy libpsplib.a %PSPSDKDIR%\lib
IF ERRORLEVEL 1 GOTO ERROR

echo *****************
echo * Include files *
echo *****************

set INC=%PSPSDKDIR%\include\psplib
rem echo %INC%
rem pause

mkdir %INC%

cd source
copy pspl_graphics.h %INC%
copy pspl_input.h %INC%
copy psplib.h %INC%

IF ERRORLEVEL 1 GOTO ERROR
echo Installation completed successfully.
pause
exit

:ERROR
color c
echo Installation failed. Please verify the installation path!
pause
