@echo off

set MAXIM_PATH=C:\MaximSDK
set TOOLCHAIN_PATH=%MAXIM_PATH%\Tools

set PATH=%TOOLCHAIN_PATH%\OpenOCD; %PATH%

echo ----------------------------------------
echo Erasing flash...
echo.  
openocd -s %TOOLCHAIN_PATH%/OpenOCD/scripts -f interface/cmsis-dap.cfg -f target/max32657_setup.cfg -c "init;halt;max32xxx mass_erase 0;exit"
timeout /t 2

echo ----------------------------------------
echo Programming image...
echo.
openocd -s %TOOLCHAIN_PATH%/OpenOCD/scripts -f interface/cmsis-dap.cfg -f target/max32657_setup.cfg -c "program zephyr.elf verify reset exit"
timeout /t 2