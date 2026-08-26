@echo off
title Bangladesh Tourism & Travel Management System - SQLite3 Server
echo =========================================================================
echo   🇧🇩 Bangladesh Tourism & Travel Management System
echo   [✓] SQLite3 Database Backend & Web Application Launcher
echo =========================================================================
echo.

where python >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [✓] Python detected. Starting SQLite3 Backend Server...
    echo [✓] Live sync with C++ database files active.
    echo.
    python "%~dp0web\server.py"
) else (
    echo [!] Python not found in PATH. Opening web interface directly in browser...
    start "" "%~dp0web\index.html"
    echo [✓] Web application launched!
    timeout /t 3 >nul
)
