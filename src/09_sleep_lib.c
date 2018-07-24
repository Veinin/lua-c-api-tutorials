#include <unistd.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static int s_sleep(lua_State *L)
{
    long seconds = lua_tointeger(L, -1);
    sleep(seconds);
    return 0;
}

static int u_sleep(lua_State *L)
{
    long usec = lua_tointeger(L, -1);
    usleep(usec);
    return 0;
}

int luaopen_msleep(lua_State *L)
{
    lua_register(L, "sleep", s_sleep);
    lua_register(L, "msleep", m_sleep);
    return 0;
}