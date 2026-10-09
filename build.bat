@echo off
REM ============================================================================
REM  Build Script for Custom CJ Clothing Plugin
REM  Builds both Server Plugin (.dll) and Client ASI Plugin (.asi)
REM ============================================================================

echo ============================================
echo  Custom CJ Clothing Plugin - Build Script
echo  Version: 1.0.0
echo ============================================
echo.

REM Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake not found! Please install CMake and add it to PATH.
    echo Download: https://cmake.org/download/
    pause
    exit /b 1
)

REM Check for Visual Studio / MSVC
where cl >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [WARNING] MSVC compiler not found in PATH.
    echo Make sure to run this from a Visual Studio Developer Command Prompt.
    echo Or install Visual Studio Build Tools.
    echo.
)

REM Create build directory
set BUILD_DIR=%~dp0build
set OUTPUT_DIR=%~dp0output

echo [1/4] Creating build directory...
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo [2/4] Running CMake configuration...
cd /d "%BUILD_DIR%"
cmake .. -G "Visual Studio 18 2026" -A Win32 -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo.
    echo [INFO] VS 2026 not found, trying VS 2022...
    if exist CMakeCache.txt del CMakeCache.txt
    if exist CMakeFiles rmdir /s /q CMakeFiles
    cmake .. -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=Release
    if %ERRORLEVEL% neq 0 (
        echo.
        echo [INFO] VS 2022 not found, trying VS 2019...
        if exist CMakeCache.txt del CMakeCache.txt
        if exist CMakeFiles rmdir /s /q CMakeFiles
        cmake .. -G "Visual Studio 16 2019" -A Win32 -DCMAKE_BUILD_TYPE=Release
        if %ERRORLEVEL% neq 0 (
            echo.
            echo [INFO] Trying NMake...
            if exist CMakeCache.txt del CMakeCache.txt
            if exist CMakeFiles rmdir /s /q CMakeFiles
            cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
            if %ERRORLEVEL% neq 0 (
                echo [ERROR] CMake configuration failed!
                cd /d "%~dp0"
                pause
                exit /b 1
            )
        )
    )
)

echo.
echo [3/4] Building project...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed!
    cd /d "%~dp0"
    pause
    exit /b 1
)

echo.
echo [4/4] Build completed successfully!
echo.
echo ============================================
echo  OUTPUT FILES:
echo ============================================

if exist "%OUTPUT_DIR%\server\plugins\clothing.dll" (
    echo  [OK] Server Plugin:  output\server\plugins\clothing.dll
) else (
    echo  [--] Server Plugin:  NOT FOUND
)

if exist "%OUTPUT_DIR%\client\clothing.asi" (
    echo  [OK] Client Plugin:  output\client\clothing.asi
) else (
    echo  [--] Client Plugin:  NOT FOUND
)

if exist "%OUTPUT_DIR%\server\include\clothing.inc" (
    echo  [OK] Pawn Include:   output\server\include\clothing.inc
) else (
    echo  [--] Pawn Include:   NOT FOUND
)

echo.
echo ============================================
echo  TEST FOLDERS (auto-copied):
echo ============================================
echo  Server: ..\SA-MP SERVER\plugins\
echo  Client: ..\GTA SAMP\
echo  Include: ..\SA-MP SERVER\pawno\include\
echo ============================================

cd /d "%~dp0"
pause
