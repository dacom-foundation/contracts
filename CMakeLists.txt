cmake_minimum_required(VERSION 3.5)

project(coopenomics)

# Опции
option(SYSTEM_CONFIGURABLE_WASM_LIMITS "Enables use of the host functions activated by the CONFIGURABLE_WASM_LIMITS protocol feature" ON)
option(SYSTEM_BLOCKCHAIN_PARAMETERS "Enables use of the host functions activated by the BLOCKCHAIN_PARAMETERS protocol feature" ON)
option(ENABLE_NATIVE_COMPILER "enable native builds with the CDT toolchain" ON)

set(CDT_DIR "/cdt/build/lib/cmake/cdt") # Укажите путь к CDT
set(LEAP_DIR "/blockchain/build/lib/cmake/leap") # Укажите путь к Leap

include(ExternalProject)


ExternalProject_Add(
   fund_project
   SOURCE_DIR ${CMAKE_SOURCE_DIR}/fund
   BINARY_DIR ${CMAKE_BINARY_DIR}/fund
   CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=/cdt/build/lib/cmake/cdt/CDTWasmToolchain.cmake
   UPDATE_COMMAND ""
   PATCH_COMMAND ""
   TEST_COMMAND ""
   INSTALL_COMMAND ""
   BUILD_ALWAYS 1
)

ExternalProject_Add(
   hello_tests_project
   SOURCE_DIR ${CMAKE_SOURCE_DIR}/fund/tests
   BINARY_DIR ${CMAKE_BINARY_DIR}/fund/tests
   CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=/cdt/build/lib/cmake/cdt/CDTWasmToolchain.cmake
   UPDATE_COMMAND ""
   PATCH_COMMAND ""
   TEST_COMMAND ""
   INSTALL_COMMAND ""
   BUILD_ALWAYS 1
)

### add a new test
include (CTest)
enable_testing()
add_test(hello_test ${CMAKE_BINARY_DIR}/fund/tests/fund_test)