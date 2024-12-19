SET(CORE_SRC Memory/Allocator.cpp Memory/MemoryManager.cpp)
set(CMAKE_BUILD_TYPE Debug)
add_library(Core ${CORE_SRC})