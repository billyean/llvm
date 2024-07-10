cmake_minimum_required(VERSION 3.26.4s)
project(lding)

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

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
add_executable(lding Lexer.h Lexer.cpp
        Lexer.h
        Parser.cpp
        Parser.h
        AST.cpp
        AST.h
        Sema.cpp
        Sema.h
        CodeGen.cpp
        CodeGen.h
        LDing.cpp)
