set (CMAKE_C_COMPILER "D:/Program Files (x86)/LLVM/bin/clang.exe")
set (CMAKE_CXX_COMPILER "D:/Program Files (x86)/LLVM/bin/clang++.exe")

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

project(RunaEngine)

add_executable(${PROJECT_NAME} src/Program/helloengine.c)