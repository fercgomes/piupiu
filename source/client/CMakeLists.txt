add_executable(Client main.cpp client.cpp gui/gui.cpp)

target_set_warnings(Client DISABLE ALL)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(Client PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
                           $<INSTALL_INTERFACE:include>)               

target_link_libraries(Client PRIVATE Threads::Threads imgui glfw OpenGL::GL ${CMAKE_DL_LIBS})

target_compile_features(Client PRIVATE cxx_std_14)
target_compile_options(Client PRIVATE -O0)