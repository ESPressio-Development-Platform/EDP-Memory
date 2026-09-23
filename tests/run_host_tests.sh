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

SANITIZER_MODE="${EDP_MEMORY_SANITIZER_MODE:-}"

if [[ "${EDP_MEMORY_SANITIZERS:-0}" == "1" && -z "${SANITIZER_MODE}" ]]; then
    SANITIZER_MODE="address,undefined"
fi

compile_host_tests() {
    "${CXX}" \
        "${COMMON_FLAGS[@]}" \
        "$@" \
        "${ROOT_DIR}/tests/host/main.cpp" \
        -o "${BUILD_DIR}/host-tests"
}

case "${SANITIZER_MODE}" in
    "")
        echo "EDP-Memory host runtime tests: compiling"
        compile_host_tests
        ;;
    address)
        echo "EDP-Memory host runtime tests: compiling (ASan)"
        compile_host_tests \
            -g \
            -fno-omit-frame-pointer \
            -fsanitize=address
        ;;
    undefined)
        echo "EDP-Memory host runtime tests: compiling (UBSan)"
        compile_host_tests \
            -g \
            -fno-omit-frame-pointer \
            -fsanitize=undefined
        ;;
    address,undefined)
        echo "EDP-Memory host runtime tests: compiling (ASan+UBSan)"
        compile_host_tests \
            -g \
            -fno-omit-frame-pointer \
            -fsanitize=address,undefined
        ;;
    *)
        echo "Unsupported EDP_MEMORY_SANITIZER_MODE: ${SANITIZER_MODE}" >&2
        echo "Use: address, undefined, or address,undefined" >&2
        exit 2
        ;;
esac

echo "EDP-Memory host runtime tests: executing"

if [[ -n "${SANITIZER_MODE}" ]]; then
    ASAN_OPTIONS="${ASAN_OPTIONS:-detect_leaks=0:abort_on_error=1}" \
    UBSAN_OPTIONS="${UBSAN_OPTIONS:-halt_on_error=1:print_stacktrace=1}" \
        "${BUILD_DIR}/host-tests"
else
    "${BUILD_DIR}/host-tests"
fi

echo "EDP-Memory compile-fail tests: validating"

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
