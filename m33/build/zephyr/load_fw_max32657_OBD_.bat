@echo off

set MAXIM_PATH=C:\MaximSDK
set TOOLCHAIN_PATH=%MAXIM_PATH%\Tools

set PATH=%TOOLCHAIN_PATH%\OpenOCD; %PATH%

set TARGETPATH=c:\analog\cfs\1.1.0\Tools\openocd\share\openocd\scripts\target
set INTERFACEPATH=c:\analog\cfs\1.1.0\Tools\openocd\share\openocd\scripts\interface
set SCRIPTSPATH=c:\analog\cfs\1.1.0\Tools\openocd\share\openocd\scripts

echo ----------------------------------------
echo Erasing flash...
echo.  
c:\analog\cfs\1.1.0\Tools\openocd\bin\openocd.exe  -s %SCRIPTSPATH%  -f interface/cmsis-dap.cfg -f target/max32657_setup.cfg -c "init;halt;max32xxx mass_erase 0;exit"
timeout /t 2

echo ----------------------------------------
echo Programming image...
echo.
c:\analog\cfs\1.1.0\Tools\openocd\bin\openocd.exe  -s %SCRIPTSPATH%  -f interface/cmsis-dap.cfg -f target/max32657_setup.cfg -c "program zephyr.elf verify reset exit"
timeout /t 2