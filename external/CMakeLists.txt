project(external C CXX)

if(USE_GUI)
    file(GLOB Imgui_SRC "imgui/*.h" "imgui/*.cpp" "imgui/backends/imgui_impl_glfw.cpp" "imgui/backends/imgui_impl_glfw.h" "imgui/backends/imgui_impl_opengl3.cpp" "imgui/backends/imgui_impl_opengl3.h")
    add_library(imgui ${Imgui_SRC})
    target_include_directories(imgui PUBLIC imgui imgui/backends)
endif()