include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(RHI D3D11/RenderD3D11.cpp OpenGL/RenderGL.cpp)