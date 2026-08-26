@echo off
title Building Travel Management System
echo ========================================================
echo   Bangladesh Tourism & Travel Management System (OOP)
echo ========================================================
echo.

echo [1/2] Compiling Modular Project (C++14)...
C:\MinGW\bin\g++.exe -std=c++14 -Wall -Wextra -O2 -I. main.cpp src/core/Graph.cpp src/models/User.cpp src/models/Trip.cpp src/models/Booking.cpp src/services/FileManager.cpp src/services/AuthService.cpp src/services/BookingService.cpp src/ui/ConsoleUI.cpp -o TravelManagementSystem_modular.exe

if %ERRORLEVEL% EQU 0 (
    echo [✓] Modular Build Successful: TravelManagementSystem_modular.exe
) else (
    echo [X] Modular Build Failed!
)

echo.
echo [2/2] Compiling Standalone Single-File Edition...
C:\MinGW\bin\g++.exe -std=c++14 -Wall -Wextra -O2 TravelManagementSystem_standalone.cpp -o TravelManagementSystem.exe

if %ERRORLEVEL% EQU 0 (
    echo [✓] Standalone Build Successful: TravelManagementSystem.exe
) else (
    echo [X] Standalone Build Failed!
)

echo.
echo ========================================================
echo Build Complete! Run TravelManagementSystem.exe to start.
echo ========================================================
pause
