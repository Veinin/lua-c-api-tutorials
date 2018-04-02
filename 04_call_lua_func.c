#include <stdio.h>
#include <stdlib.h>

#include "errors.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

int main(void)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	if(luaL_loadfile(L, "call_lua_func.lua"))
		error_exit(L, "luaL_loadfile failed.\n");

	if(lua_pcall(L, 0, 0, 0))
		error_exit(L, "lua_pcall failed.\n");

	printf("In C, calling Lua->sayHello()\n");

	lua_getglobal(L, "sayHello");	//Tell it to run test2.lua -> sayHello()
	if(lua_pcall(L, 0, 0, 0))
		error_exit(L, "lua_pcall failed.\n");

	printf("Back in C again\n");
	printf("\nIn C, calling Lua->add()\n");

	lua_getglobal(L, "add");		//Tell it to run test2.lua -> add()
	lua_pushnumber(L, 1);
	lua_pushnumber(L, 5);
	if(lua_pcall(L, 2, 1, 0))
		error_exit(L, "lua_pcall failed.\n");

	printf("Back in C again\n");
	int returnNum = lua_tonumber(L, -1);
	printf("Returned number : %d\n", returnNum);

	lua_close(L);

	return 0;
}