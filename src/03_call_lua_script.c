#include "errors.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

int main(void)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	if(luaL_loadfile(L, "call_lua_script.lua"))
		err_exit(L, "luaL_loadfile failed.\n");

	printf("In C, calling Lua\n");

	if(lua_pcall(L, 0, 0, 0))
		err_exit(L, "lua_pcall() failed.", lua_tostring(L, -1));

	printf("Back in C again\n");

	lua_close(L);

	return 0;
}
