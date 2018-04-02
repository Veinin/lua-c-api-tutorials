#include <stdio.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static int square(lua_State *L)
{
	float num = lua_tonumber(L, -1);
	printf("square(), num = %f\n", num);
	lua_pushnumber(L, num * num);
	return 1;
}

static int cube(lua_State *L)
{
	float num = lua_tonumber(L, -1);
	printf("cube(), num = %f\n", num);
	lua_pushnumber(L, num * num * num);
	return 1;
}

static const struct luaL_Reg libs [] = {
	{"square", square},
	{"cube", cube},
	{NULL, NULL}
};

int luaopen_power(lua_State *L)
{
	luaL_newlib(L, libs);
	return 1;
}