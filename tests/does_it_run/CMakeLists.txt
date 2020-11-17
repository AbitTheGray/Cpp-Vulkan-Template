add_executable(T_Does_it_run main.cpp)

target_link_libraries(T_Does_it_run Vulkan_Template)

configure_file(example.txt example.txt COPYONLY)

add_test(NAME Does_it_run COMMAND T_Does_it_run)
