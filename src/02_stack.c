#include <stdio.h>
#include "lua.h"
#include "lauxlib.h"

static void stackDump(lua_State *L)
{
	int i;
	int top = lua_gettop(L);

	for(i = 1; i <= top; i++) {
		int t = lua_type(L, i);
		switch(t) {
			case LUA_TNIL:
				printf("nil");
				break;
			case LUA_TBOOLEAN:
				printf(lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				printf("%g", lua_tonumber(L, i));
				break;
			case LUA_TSTRING:
				printf("%s", lua_tostring(L, i));
				break;

			default:
				printf("%s", lua_typename(L, t));
		}
		printf(" ");
	}
	printf("\n\n");
}

int main(void)
{
	lua_State *L = luaL_newstate();
	lua_pushboolean(L, 1);
	lua_pushinteger(L, 100);
	lua_pushnumber(L, 10.011);
	lua_pushstring(L, "hello");
	lua_pushnil(L);

	stackDump(L);

	lua_pushvalue(L, 1);	//将索引为1的元素压入栈顶
	stackDump(L);

	lua_settop(L, 4);	//设置栈顶为4
	stackDump(L);

	lua_insert(L, 3);	//移动栈顶元素到3
	stackDump(L);

	lua_replace(L, 3);	//将栈顶元素移动到3位置，弹出栈顶元素
	stackDump(L);

	lua_copy(L, 2, 3);	//将索引为2的元素复制到3位置
	stackDump(L);

	lua_pop(L, 3);	//剩余3个元素出栈
	stackDump(L);

	lua_close(L);
	return 0;
}