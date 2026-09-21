#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/tests/.test-build"
SYSTEM_DIR="${EDP_SYSTEM_SOURCE_DIR:?EDP_SYSTEM_SOURCE_DIR must point at an EDP-System checkout}"
PLATFORM_DIR="${EDP_PLATFORM_SOURCE_DIR:?EDP_PLATFORM_SOURCE_DIR must point at an EDP-Platform checkout}"

rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"

CXX="${CXX:-g++}"
COMMON_FLAGS=(
    -std=c++20
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -pthread
    -I"${ROOT_DIR}/src"
    -I"${SYSTEM_DIR}/src"
    -I"${PLATFORM_DIR}/src"
)

"${CXX}"     "${COMMON_FLAGS[@]}"     -fsanitize=address,undefined     -fno-omit-frame-pointer     "${ROOT_DIR}/tests/host/main.cpp"     -o "${BUILD_DIR}/host-tests"

"${BUILD_DIR}/host-tests"

for source in "${ROOT_DIR}"/tests/compile_fail/*.cpp
do
    name="$(basename "${source}" .cpp)"

    if "${CXX}" "${COMMON_FLAGS[@]}" "${source}" -o "${BUILD_DIR}/${name}" >"${BUILD_DIR}/${name}.log" 2>&1
    then
        echo "ERROR: compile-fail test unexpectedly succeeded: ${name}" >&2
        exit 1
    fi

    echo "Expected compile failure confirmed: ${name}"
done

echo "All EDP-Memory host tests passed."
