@echo off
setlocal enabledelayedexpansion

rem Parameters
set cstylec_emu=C:\bin\mingw\bin\gcc.exe
set compile_emu=tcc.exe
set counter_emu=wsl.exe
set cstylecopt_emu=-fsyntax-only -Wall -I C:\bin\lib\include
set compileopt_emu=-Wall -IC:\bin\lib\include
set counteropt_emu=wc -l

set compiler_os=nasm.exe
set executer_os=C:\bin\qemu\qemu-system-i386.exe
set compileropt_os=
set executeropt_os=-nographic

rem Mainmenu
echo +------------------+
echo ^|   Project Utils  ^|
echo +------------------+
if /i "%1"=="emu" goto emu
if /i "%1"=="os" goto os
if /i "%1"=="release" goto release

rem Special action full make(release)
:release
    call :checkfiles nasm.exe tcc.exe
    nasm os/main.S -o Latest\syoch_os_disk
    tcc -m32 -Wall -IC:\bin\lib\include -bench EMU/tinyemulator.c -o Latest/x86emu_x86.exe
    tcc -m64 -Wall -IC:\bin\lib\include -bench EMU/tinyemulator.c -o Latest/x86emu_x64.exe
    goto end
rem SubMenu os
:os
    call :Gettimestamp el
    set md=%el:~4,4%
    set yms=%el:~8,8%
    set target=os\bin\%md%\%yms%
    call :checkfiles %compiler_os% %executer_os%
    if %ERRORLEVEL% NEQ 0 goto end

    if /i "%2"=="build" goto build_os
    if /i "%2"=="run" goto run_os
    if /i "%2"=="runs" goto runs_os

    echo type not spectied
    goto help_os

    :build_os
        call :compile_os
        goto end
    :run_os
        call :compile_os
        %executer_os% %executeropt_os% Latest\syoch_os_disk
    :runs_os
        call :compile_os
        call :compile_emu
        Latest\x86emu Latest\syoch_os_disk
        goto end
    :help_os
        echo Compiler: %compiler_os%
        echo executer: %execute_os%
        echo [usage] tool os (type)
        echo type-+- Build : Build os Save bin/Latestos
        echo      ^|
        echo      +- Run   : Build ^& Run os Using Qemu
        echo                     Original Emulator Use =^> tool emu run OR Launch PreCompiled exe `LatestEmu os/output`
        goto end
    :compile_os
        
        if not exist os\bin\%md% mkdir os\bin\%md%
        echo Compiling os
        echo %compiler_os% %compileropt_os% os/main.S -o os\bin\%md%\%yms% 2^>^&1
        for /f "usebackq tokens=1,2,3,4,5 delims=:" %%a in (`%compiler_os% %compileropt_os% os/main.S -o os\bin\%md%\%yms% 2^>^&1`) do (
            echo Error:%%a@%%b:%%c %%d %%e
        )
        copy %target% Latest\syoch_os_disk
        exit /b 0

rem SubMenu Emu
:emu
    call :Gettimestamp el
    set md=%el:~4,4%
    set yms=%el:~8,8%
    set target=EMU\bin\%md%\%yms%
    call :checkfiles %cstylec_emu% %compile_emu% %counter_emu%
    if "%errorlevel%" NEQ "0" goto end

    :menu_emu
    if /i "%2"=="check" goto check_emu
    if /i "%2"=="build" goto build_emu
    if /i "%2"=="count" goto count_emu
    if /i "%2"=="run"   goto run_emu
    if /i "%2"=="exes"  goto show_exes_emu
    if /i "%2"=="help"  goto help_emu

    echo type not spectied
    goto help_emu

    :check_emu
        %stylecheck_emu% %opt_emu%  Emu\tinyemulator.c
        goto end

    :build_emu
        call :compile_emu
        copy %target% ./bin/LatestEmu.exe
        goto end

    :run_emu
        call :compile_emu
        if %ERRORLEVEL% EQU 0 (
            %target%.exe Latest/syoch_os_disk
        )
        goto end

    :show_exes_emu
        echo Compiled Exe List
        for %%f in (bin/*.exe) do (
            echo ^| %%f
        ) 
        echo +--
        echo if you trying to execute to execute command 'bin/NAME.exe'
        goto end

    :count_emu
        %counter_emu% %counteropt_emu% EMU/*.h EMU/*.c EMU/i386/*.h
        goto end

    :help_emu
        echo StyleChecker:%cstylec_emu%
        echo compiler:%compile_emu%
        
        echo [usage] tool emu (type)
        echo type-+- Build : Build File Save to ./bin/LatestEmu.exe
        echo      ^|
        echo      +- Count : Count Programs All length
        echo      ^|
        echo      +- Run   : Build ^& Run File
        echo      ^|
        echo      +- exes  : Show Compiled Exe list
        echo      ^|
        echo      +- check : Check Syntax Using gcc
        goto end
    
    :compile_emu
        set out=%target%
        if not exist EMU\bin\%md%\ mkdir EMU\bin\%md%\
        echo Compiling Emu
        set error=""
        for /f "usebackq tokens=1,2,4,5 eol=I delims=:" %%a in (`%compile_emu% %compileopt_emu% -bench  EMU/tinyemulator.c -o %out%.exe 2^>^&1`) do (
            set tmpa=%%a
            if "!tmpa:~0,1!"=="*" (
                for /f "usebackq tokens=1-6 delims=, " %%b in ('!tmpa:~1!') do (
                    echo Bench: %%b %%c
                    echo Bench: %%d %%e
                    echo Bench: %%f %%g
                )
            ) else (
                echo Error: %%a@%%b: %%c %%d
                set error="1"
            )
        )
        copy %target%.exe Latest\x86emu.exe
        exit /b %error%

rem Global Routines
:end
    echo end
    endlocal
    exit /b 0

rem Functions
:checkfiles
    set /a notfoundcount=0
    :chkf
        set /p a="Checking %1..."<nul
        call :SearchFilePath %1
        set el=!errorlevel!
        if "!el!" EQU "1" (
            echo fail
            set /a notfoundcount+=1
        ) else (
            echo done
        )
        shift
        if "%1"=="" goto chke
        goto chkf
    :chke
    exit /b %notfoundcount%
:SearchFilePath
    set filepath=""
    for %%i in (%1) do set filepath=%%~$PATH:i
    if "%filepath%" EQU "" (
        exit /b 1
    ) else (
        exit /b 0
    )
:Gettimestamp
  setlocal
    set ymd=%date%
    set hms=%time%
    set ymd=%ymd: =0%
    set ymd=%ymd:/=%
    set hms=%hms: =0%
    set hms=%hms::=%
    set hms=%hms:.=%
  endlocal & set %1=%ymd%%hms%
    goto :eof

