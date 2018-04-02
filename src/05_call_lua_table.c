#include "errors.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

int main(void)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	if(luaL_loadfile(L, "call_lua_table.lua"))
		err_exit(L, "luaL_loadfile failed.\n");

	if (lua_pcall(L, 0, 0, 0))
        err_exit(L, "lua_pcall failed");

    printf("In C,  calling Lua->tablehandler()\n");
    lua_getglobal(L, "tablehandler");
    lua_newtable(L);					// Push empty table onto stack, table now at -1
    lua_pushliteral(L, "firstname");	// Puah a key onto stack, table now at -2
    lua_pushliteral(L, "Veinin");		// Puash a value onto stack, table now at -3
    lua_settable(L, -3);				// Take key and value, put into table at -3, then pop key and value so table again at -1

    lua_pushliteral(L, "lastname");
    lua_pushliteral(L, "Guo");
    lua_settable(L, -3);

    if(lua_pcall(L, 1, 1, 0))			// Run function.
    	err_exit(L, "lua_pcall failed");

    printf("============ Back in C again, Iterating thru returned table ============\n");

    lua_pushnil(L);						// Make sure lua_next starts at beginning
    const char *k, *v;
    while(lua_next(L, -2)) {			// Table location at -2 in stack
    	v = lua_tostring(L, -1);
    	k = lua_tostring(L, -2);

    	lua_pop(L, 1);					// Remove value, leave in place to guide next lua_next().

    	printf("%s = %s\n", k, v);
    }

    lua_close(L);

    return 0;
}
