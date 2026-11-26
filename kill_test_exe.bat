@echo off
setlocal EnableExtensions

rem ------------------------------------------------------------------
rem  用法: kill_test_exe.bat [进程名(不带.exe), 可省略]
rem  不传参数时, 自动结束所有 STLTest_*.exe 和 test_*.exe 测试进程,
rem  用于解决链接时 "Permission denied: cannot open output file" 的问题
rem  (一般是上次运行的同名 exe 还没退出, 文件被占用导致的)
rem ------------------------------------------------------------------

set "PROC_NAME_BASE=%~1"

rem ------------------------------------------------------------------
rem  检查管理员权限；若不是管理员，自动请求提权后重新运行
rem ------------------------------------------------------------------
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo [INFO] 当前不是管理员权限，正在请求提权...
    powershell -NoProfile -Command "Start-Process -FilePath '%~f0' -ArgumentList '%PROC_NAME_BASE%' -Verb RunAs"
    exit /b
)

echo [INFO] 已以管理员权限运行。

if not "%PROC_NAME_BASE%"=="" (
    echo [INFO] 正在结束所有 %PROC_NAME_BASE%.exe 进程...
    taskkill /F /IM "%PROC_NAME_BASE%.exe" /T 2>nul
    powershell -NoProfile -Command "Get-Process -Name '%PROC_NAME_BASE%' -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue"

    tasklist /FI "IMAGENAME eq %PROC_NAME_BASE%.exe" 2>nul | find /I "%PROC_NAME_BASE%.exe" >nul
    if %errorlevel% equ 0 (
        echo.
        echo [WARN] 仍有实例未能结束，残留进程：
        tasklist /FI "IMAGENAME eq %PROC_NAME_BASE%.exe"
    ) else (
        echo [OK] 所有 %PROC_NAME_BASE%.exe 进程已全部结束。
    )
) else (
    echo [INFO] 未指定进程名，正在结束所有 STLTest_*.exe / test_*.exe 测试进程...
    powershell -NoProfile -Command ^
        "Get-Process | Where-Object { $_.ProcessName -like 'STLTest_*' -or $_.ProcessName -like 'test_*' } | ForEach-Object { Write-Host ('[INFO] 结束 ' + $_.ProcessName + ' (PID ' + $_.Id + ')'); Stop-Process -Id $_.Id -Force -ErrorAction SilentlyContinue }"

    powershell -NoProfile -Command ^
        "if (-not (Get-Process | Where-Object { $_.ProcessName -like 'STLTest_*' -or $_.ProcessName -like 'test_*' })) { Write-Host '[OK] 所有测试进程已全部结束。' }"
)

echo.
pause
