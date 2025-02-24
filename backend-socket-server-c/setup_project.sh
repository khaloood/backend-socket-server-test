#!/usr/bin/env bash
set -e          # Exit on error
set -o pipefail # Catch errors in pipelines

################################################################################
# CONFIGURATION
################################################################################
VCPKG_ROOT="${PWD}/vcpkg"          # vcpkg installation directory
VCPKG_REPO="https://github.com/microsoft/vcpkg.git"
VCPKG_BRANCH="master"              # Change to a specific tag/commit if needed
BUILD_DIR="build"                  # CMake build directory
TOOLCHAIN_FILE="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"

################################################################################
# 1. Install vcpkg (if not already installed)
################################################################################
if [ ! -d "${VCPKG_ROOT}" ]; then
  echo "[INFO] Cloning vcpkg into ${VCPKG_ROOT} ..."
  git clone --depth=1 --branch "${VCPKG_BRANCH}" "${VCPKG_REPO}" "${VCPKG_ROOT}"
fi

echo "[INFO] Bootstrapping vcpkg..."
pushd "${VCPKG_ROOT}" >/dev/null
./bootstrap-vcpkg.sh -disableMetrics
popd >/dev/null

################################################################################
# 2. Install dependencies using vcpkg manifest mode
################################################################################
echo "[INFO] Installing dependencies using vcpkg.json..."
pushd "${VCPKG_ROOT}" >/dev/null
./vcpkg install --triplet x64-linux
popd >/dev/null

################################################################################
# 3. Configure project with CMake (using the vcpkg toolchain)
################################################################################
echo "[INFO] Configuring project with CMake..."
cmake -B "${BUILD_DIR}" -S . \
  -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DVCPKG_FEATURE_FLAGS=manifests

################################################################################
# 4. Build the project
################################################################################
echo "[INFO] Building project..."
cmake --build "${BUILD_DIR}" -- -j$(nproc)

################################################################################
# 5. Run the server (Optional: Uncomment to start automatically)
################################################################################
# echo "[INFO] Starting the WebSocket server..."
# "${BUILD_DIR}/backend-socket-server"

echo
echo "========================================================"
echo "[INFO] SUCCESS! The project has been built in '${BUILD_DIR}'."
echo "========================================================"
