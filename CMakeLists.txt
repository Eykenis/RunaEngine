cmake_minimum_required(VERSION 3.5.0)

set (CMAKE_C_COMPILER "D:/Program Files (x86)/LLVM/bin/clang.exe")
set (CMAKE_CXX_COMPILER "D:/Program Files (x86)/LLVM/bin/clang++.exe")

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

project(RunaEngine)

set(TARGET_PLATFORM "Windows")

add_subdirectory(src/Runtime)

add_executable(${PROJECT_NAME} src/Program/memory.cpp)
target_link_libraries(${PROJECT_NAME}
  d3d11.lib dxgi.lib kernel32.lib user32.lib d3dcompiler.lib dxguid.lib
  Forms RHI Core
)