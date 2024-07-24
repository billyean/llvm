cmake_minimum_required(VERSION 3.26.4s)
project(ltiger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(vcpkg "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
if(NOT CMAKE_TOOLCHAIN_FILE AND EXISTS "${vcpkg}")
    set(CMAKE_TOOLCHAIN_FILE "${vcpkg}"
            CACHE FILEPATH "CMake toolchain file")
    message(STATUS "vcpkg toolchain found: ${CMAKE_TOOLCHAIN_FILE}")
endif()

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include(AddLLVM)
include(HandleLLVMOptions)

include_directories("${LLVM_BINARY_DIR}/include" "${LLVM_INCLUDE_DIR}")
link_directories("${LLVM_LIBRARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(LTIGER_VERSION_STRING "1.0")
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Basic/Ver.inc.in
        ${CMAKE_CURRENT_BINARY_DIR}/include/Basic/Ver.inc)

include(AddLTiger)

include_directories(BEFORE
        ${CMAKE_CURRENT_BINARY_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_subdirectory(lib)
#add_subdirectory(main)


