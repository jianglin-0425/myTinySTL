@echo off
setlocal EnableExtensions

set "PROC_NAME=STLTest_03_list.exe"
set "PROC_NAME_BASE=STLTest_03_list"

rem ------------------------------------------------------------------
rem  检查管理员权限；若不是管理员，自动请求提权后重新运行
rem ------------------------------------------------------------------
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo [INFO] 当前不是管理员权限，正在请求提权...
    powershell -NoProfile -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    exit /b
)

echo [INFO] 已以管理员权限运行。
echo [INFO] 正在结束所有 %PROC_NAME% 进程...

rem ---- 方式一：taskkill 强制结束（含子进程树）----
taskkill /F /IM "%PROC_NAME%" /T 2>nul

rem ---- 方式二：PowerShell Stop-Process -Force 兜底 ----
powershell -NoProfile -Command "Get-Process -Name '%PROC_NAME_BASE%' -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue"

rem ---- 检查是否还有残留进程 ----
tasklist /FI "IMAGENAME eq %PROC_NAME%" 2>nul | find /I "%PROC_NAME%" >nul
if %errorlevel% equ 0 (
    echo.
    echo [WARN] 仍有实例未能结束，可能原因：
    echo        1. 进程正被调试器（gdb/lldb/vs debugger）附加；
    echo        2. 被安全软件保护；
    echo        3. 由其他用户会话启动。
    echo        残留进程：
    tasklist /FI "IMAGENAME eq %PROC_NAME%"
) else (
    echo [OK] 所有 %PROC_NAME% 进程已全部结束。
)

echo.
pause
