add_executable(hello 01_hello.c)
add_executable(stack 02_stack.c)
add_executable(call_lua_script 03_call_lua_script.c)
add_executable(call_lua_func 04_call_lua_func.c)
add_executable(call_lua_table 05_call_lua_table.c)

add_library(mylib SHARED 06_call_c_func.c)
add_library(split SHARED 08_split_lib.c)