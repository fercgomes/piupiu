add_executable(Client main.cpp client.cpp)

target_set_warnings(Client DISABLE ALL)

target_link_libraries(Client PRIVATE Threads::Threads)

target_compile_features(Client PRIVATE cxx_std_14)
target_compile_options(Client PRIVATE -O0)