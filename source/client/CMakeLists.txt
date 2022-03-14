add_executable(Client main.cpp client.cpp $<$<BOOL:${USE_GUI}>:gui/gui.cpp> ../messaging/Packet.cpp)

if(USE_GUI)
    target_compile_definitions(Client PUBLIC USE_GUI)
endif()

target_set_warnings(Client DISABLE ALL)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(Client PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
                           $<INSTALL_INTERFACE:include>
                           "../messaging")               

target_link_libraries(Client PRIVATE Threads::Threads glfw OpenGL::GL ${CMAKE_DL_LIBS} $<$<BOOL:${USE_GUI}>:imgui>)

target_compile_features(Client PRIVATE cxx_std_14)
target_compile_options(Client PRIVATE -O0)