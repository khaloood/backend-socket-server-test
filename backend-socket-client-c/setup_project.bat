@echo off
setlocal enabledelayedexpansion

:: Define directories
set VCPKG_ROOT=%CD%\vcpkg
set BUILD_DIR=%CD%\build
set TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake

echo [INFO] Checking for vcpkg...
if not exist "%VCPKG_ROOT%" (
    echo [INFO] Cloning vcpkg...
    git clone --depth=1 --branch master https://github.com/microsoft/vcpkg.git "%VCPKG_ROOT%"
    if %errorlevel% neq 0 (
        echo [ERROR] Failed to clone vcpkg.
        exit /b 1
    )
)

echo [INFO] Bootstrapping vcpkg...
cd /D "%VCPKG_ROOT%"
call bootstrap-vcpkg.bat -disableMetrics
if %errorlevel% neq 0 (
    echo [ERROR] Failed to bootstrap vcpkg.
    exit /b 1
)

echo [INFO] Installing dependencies from vcpkg.json...
call vcpkg install --triplet x64-windows
if %errorlevel% neq 0 (
    echo [ERROR] Failed to install dependencies.
    exit /b 1
)

cd /D "%CD%"

echo [INFO] Configuring project with CMake...
cmake -B "%BUILD_DIR%" -S . ^
  -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN_FILE%" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DVCPKG_FEATURE_FLAGS=manifests
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed.
    exit /b 1
)

echo [INFO] Building project...
cmake --build "%BUILD_DIR%" -- /maxcpucount
if %errorlevel% neq 0 (
    echo [ERROR] Build failed.
    exit /b 1
)

echo ========================================================
echo SUCCESS! The project has been built in '%BUILD_DIR%'.
echo ========================================================
exit /b 0
