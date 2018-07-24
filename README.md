# lua-c-api-tutorials

## Contents

* [Get Started](./doc/01_get_started.md)
  * [Hello World](./src/01.hello.c)
  * [Lua Stack](./src/02_stack.c)

* [Calling Lua From a C Program](./doc/02_calling_lua_from_a_c_program.md)
  * Calling Lua Script.  [call_lua_script.c](./src/03_call_lua_script.c) -> [call_lua_script.lua](./src/call_lua_script.lua)
  * Calling Lua Function.  [call_lua_func.c](./src/04_call_lua_func.c) -> [call_lua_func.lua](./src/call_lua_script.lua)
  * Calling Lua Table.  [call_lua_table.c](./src/05_call_lua_table.c) -> [call_lua_table.lua](./src/call_lua_table.lua)

* [Calling C from Lua](/doc/03_calling_c_from_lua.md)
  * Lua to C Function Call.  [call_c_func.c](./src/06_call_lua_script.c) -> [call_c_func.lua](./src/call_c_func.lua)
  * Example: Split Lib.  [split_lib.c](./src/07_split_lib.c) -> [test_split.lua](./src/test_split.lua)
  * Example: Boolean Array Lib.  [boolarray_lib.c](./src/08_boolarray_lib.c) -> [test_boolarray.lua](./src/test_boolarray.lua)
  * Example: Sleep Lib.  [sleep_lib.c](./src/09_sleep_lib.c) -> [test_sleep.lua](./src/test_sleep.lua)