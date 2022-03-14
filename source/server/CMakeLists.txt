add_executable(Server main.cpp server.cpp ProfileManager.cpp Profile.cpp SessionManager.cpp ../messaging/Packet.cpp)

# target_link_libraries(Server    PRIVATE 
#         general  # It is possible to link some libraries for debug or optimized builds only
# )

# include_directories(${CMAKE_SOURCE_DIR}/include)
target_include_directories(Server PUBLIC
                           "../messaging")               

target_compile_features(Server PRIVATE cxx_std_14)
target_link_libraries(Server PRIVATE Threads::Threads)


target_set_warnings(Server DISABLE ALL)

# target_set_warnings(Server ENABLE ALL AS_ERROR ALL DISABLE Annoying) # Helper that can set default warning flags for you
# target_enable_lto(Server optimized) #enable lto if available for non-debug configurations
# copy_dlls_for_debug(Server "" "") # Copy dependencies next to the executable (DLLs for example)

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT Server)