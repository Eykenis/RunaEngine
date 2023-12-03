cmake_minimum_required(VERSION 3.5.0)

set (CMAKE_C_COMPILER "D:/Program Files (x86)/LLVM/bin/clang.exe")
set (CMAKE_C_FLAGS "-Wall")

set (CMAKE_CXX_COMPILER "D:/Program Files (x86)/LLVM/bin/clang++.exe")
set (CMAKE_CXX_FLAGS "-Wall")

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

project(RunaEngine)

set(TARGET_PLATFORM "Windows")

add_subdirectory(src/Runtime)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(libpng STATIC IMPORTED)
set_target_properties(libpng PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lib/libpng16_static.lib)
add_library(zlib STATIC IMPORTED )
set_target_properties(zlib PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lib/zlibstatic.lib)

add_executable(${PROJECT_NAME} src/Program/helloengine.cpp src/Program/glad.c)

target_link_libraries(${PROJECT_NAME}
  d3d11.lib dxgi.lib kernel32.lib user32.lib d3dcompiler.lib dxguid.lib
  gdi32.lib opengl32.lib
  Forms RHI Core AssetsImport
  libpng zlib
)