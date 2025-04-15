include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(Forms Win32/FormWin.cpp 
            imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/imgui.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_win32.cpp)