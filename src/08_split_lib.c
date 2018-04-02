#include <string.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static int l_split(lua_State *L)
{
	const char *s = luaL_checkstring(L, 1);		//目标字符串
	const char *sep = luaL_checkstring(L, 2);	//分割字符
	const char *e;
	int i = 1;

	lua_newtable(L);	//返回结果

	while((e = strchr(s, *sep)) != NULL) {
		lua_pushlstring(L, s, e - s);
		lua_rawseti(L, -2, i++);
		s = e + 1;
	}

	lua_pushstring(L, s);
	lua_rawseti(L, -2, i);

	return 1;
}

static const struct luaL_Reg libs [] = {
	{"split", l_split},
	{NULL, NULL}
};

int luaopen_splitlib(lua_State *L) 
{
	luaL_newlib(L, libs);
	return 1;
}
