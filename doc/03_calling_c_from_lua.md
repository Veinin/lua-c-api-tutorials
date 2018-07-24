# Lua C API 教程 - 从 Lua 调用 C 函数

前一篇文章介绍了怎么从 C 程序中调用 Lua 代码。但内容并没有深入，还有很多东西需要反复去尝试，并且需要通过 Lua 辅助来调用 C 程序。 本章将着重介绍如何继续扩展你的 Lua 程序 - 在Lua中调用C函数。

## 剖析一个 Lua 程序对 C 的调用

从 Lua 的角度来看，调用 C 程序其实就是调用一个模块，在Lua中一个代码块称为chunk，一个chunk里面通常有若干函数，这些函数用table存储。而Lua的C扩展模块也是可以这样实现的，Lua调用C函数时，并不依赖于函数名、包的位置，而只依赖于注册函数时传入的函数地址。

一个简单的 Lua 程序，调用一个包含了 `foo` 和 `bar` 函数的 `mylib` 模块：

```lua
local mylib = require "mylib"
print(mylib.foo())
print(mylib.bar())
```

上面的代码 `require "mylib"` 语句加载了一个名为 `mylib` 的模块，显然模块具有函数 `foo` 和 `bar`。所以 C 程序不仅需要定义 `foo()` 和 `bar()` 函数，还需要将自己注册为一个模块，并将函数注入模块当中。

`require "mylib"` 做了两件事：

- 寻找一个叫做 `mylib.so` (windows 一个是 `mylib.dll`) 的动态库文件。
- 一旦发现 `mylib.so` , 就会去查找一个名为 `luaopen_mylib` 的函数来运行。

通常，创建 C 模块时，函数都是私有的，都声明为static。一个模块函数定义大概是这样子：

```c
static int foo(lua_State *L)
{
    lua_pushstring(L, "foo...")
    return 1;
}

static int bar(lua_State *L)
{
    lua_pushstring(L, "bar...");
    return 1;
}
```

为了让模块函数注册到 Lua 中去，我们需要声明一个 `luaL_Reg` 的数组，来包含模块中的所有函数及名称。任何 `luaL_Reg` 数组都必须以一对为 `NULL` 的 name 与 func 结束：

```c
static const struct luaL_Reg mylib[] = {
    {"foo", foo},
    {"bar", bar},
    {NULL, NULL}
};
```

最后，声明一个打开模块的主函数，该函数名称必须以 `luaopen_` 开头，后面跟着的是你编写的模块名称，它唯一的参数就是 Lua 状态机。并使用 `luaL_newlib` 创建一张新表，并把 `luaL_Reg` 中的函数注册进去：

```c
int luaopen_mylib(lua_State *L) {
    luaL_newlib(L, mylib);
    return 1;
}
```

最棘手的莫过于编译一个写好的新模块，它看起来是这样子：

在 Mac OS 中：

```shell
$ cc -o mylib.so mylib.c -I /usr/local/include -L /usr/local/lib -bundle -undefined dynamic_lookup
```

在 Linux 中：

```shell
$ gcc -o mylib.so mylib.c -I/usr/include/lua5.1 -llua5.1 -Wall -shared -fPIC
```

## 做的更好一些

下面我们模拟编写一个名为 `power` 的模块，并实现两个名为 `square` 和 `cube` 的函数来进行计算任务，并把计算结果返回 Lua。

首先，编写模块 C 程序：

```c
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
```

然后，把c程序编译成动态库 pwer.so。
ps:MAC OSX中编译时需要加入-bundle -undefined dynamic_lookup 选项，不然会引起"multiple lua vms detected" 错误。

```shell
$ cc -o power.so power.c -I /usr/local/include -L /usr/local/lib -bundle -undefined dynamic_lookup
```

别写测试 Lua 脚本：

```shell
local power = require("power")
print(power.square(2.5))
print(power.cube(2.5))
```

运行：

```shell
$ lua test_power.lua
square(), num = 2.500000
6.25
cube(), num = 2.500000
15.625
```

## 更进一步

如果你已经和 Lua 一起工作过一段时间后，你一个会注意到 Lua 没有内置 `sleep()` 延迟函数。一种解决方案是调用 C 的 `sleep()` 函数。我们做的是需要实现两个名为 `sleep()` 和 `msleep` 和函数来让 Lua 代码延迟指定多少秒、多少毫秒。

