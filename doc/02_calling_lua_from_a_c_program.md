# Lua C API 教程 - 从 C 程序调用 Lua

从正在运行的 C 程序中，你可以调用 Lua 脚本。C 程序可以将参数传递给 Lua 脚本，而Lua脚本也可以向 C 程序传回一个返回值。Lua脚本可以用来初始化C程序，或者Lua脚本也可以在 C 程序中注册一个回调。

## 错误处理

如果没有正确的信息，从C程序调用Lua脚本可能会令人望而生畏。不同的计算机对库和包含文件有不同的名称和位置，很难找出导致编译/链接问题的原因。
在有错误的情况下，应该程序将错误信息输出到标准错误流中，而 Lua 核心绝不会直接输出任何东西到错误输出流上，它通过返回错误代码和信息来发出错误信号。每个应用程序都可以适当的用最适合它们自己的方式来处理这些错误信息。

下面是一段处理 Lua 错误的方案，它只是输出一条错误信息，关闭 lua_State，并退出整个应用程序：

```c
#ifndef ERRORS_H
#define ERRORS_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "lua.h"

void err_exit(lua_State *L, const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    lua_close(L);
    exit(EXIT_FAILURE);
}

#endif //ERRORS_H
```

## 剖析一个 C 程序对 Lua 的调用

从 C 程序到 Lua 脚本的典型交互流程看起来像这样：

- 创建一个 Lua 状态变量
- 加载 Lua 库
- 加载但不运行 Lua 脚本文件
- 启动脚本运行以创建脚本的全局变量
- 将所有参数传递给堆栈上的 Lua 脚本
- 运行加载的 Lua 脚本
- 从 Lua 脚本中检索返回
- 关闭 Lua 状态变量

## 实现一个调用 Lua 简单脚本 C 程序

我们需要编写一个简单的 C 程序调用 Lua 脚本的实例，它需要实现以下功能：

- 加载的 Lua 脚本启动运行
- 将参数传递给 Lua 脚本
- 从 Lua 脚本中检索返回

首先，编写一个 Lua 脚本文件代码，文件名为 `call_lua_script` ：

```lua
io.write("This is comming from lua\n")
```

然后，编写一个 C 程序代码，加载前面编写好的脚本文件：

```c
#include <stdio.h>
#include <stdlib.h>

#include "errors.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

int main()
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
```

### 相关函数解析

1.`luaL_loadfile(L, "call_lua_script.lua");` 

加载一个 Lua 脚本文件。此函数的返回值和 `lua_load` 相同，不过它还可能产生一个叫做 `LUA_ERRFILE` 的出错码。这种错误发生于无法打开或读入文件时，或是文件的模式错误。和 `lua_load` 一样，这个函数仅加载代码块，并不会直接运行脚本内容。

2.`lua_pcall(L, number_of_args, number_of_returns, errfunc_idx);`

运行加载的 Lua 脚本函数，第二个参数位传入参数数量; 第三个参数为调用函数返回值数量。

### 编译、运行：

``` shell
$ cc -o call_lua_script call_lua_script.c -I /usr/local/include/ -L /usr/local/lib/ -llua
$ ./call_lua_script
In C, calling Lua
This is comming from lua
Back in C again
```

## 调用Lua函数，传递参数，接受返回值

实际上，很多时候，我们需要调用的是指定的 Lua 脚本函数，它的运行步骤看起来是这样的：

- 加载的 Lua 脚本启动运行
- 将参数传递给 Lua 脚本的指定函数
- 从 Lua 脚本中检索返回值

然后，让我们开始在 Lua 脚本中编写一个待调用的特定函数，创建 `call_lua_func.lua` 文件：

```lua
function sayHello()
    io.write("This is comming from lua.sayHello.\n")
end

function add(a, b)
    print("This is comming from lua.add. arg.a =", a, " arg.b =", b)
    return a + b
end
```

创建并编写一个 `call_lua_func.c` 文件，调用上面创建的 Lua 文件：

```c
#include <stdio.h>
#include <stdlib.h>

#include "errors.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

int main()
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    if(luaL_loadfile(L, "call_lua_func.lua"))
        error_exit(L, "luaL_loadfile failed.\n");

    if(lua_pcall(L, 0, 0, 0))
        error_exit(L, "lua_pcall failed.\n");

    printf("In C, calling Lua->sayHello()\n");

    lua_getglobal(L, "sayHello");    //Tell it to run test2.lua -> sayHello()
    if(lua_pcall(L, 0, 0, 0))
        error_exit(L, "lua_pcall failed.\n");

    printf("Back in C again\n");
    printf("\nIn C, calling Lua->add()\n");

    lua_getglobal(L, "add");        //Tell it to run test2.lua -> add()
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
```

### 相关函数解析

在 Lua 脚本中， `sayHello` 函数只做输出操作，而 `add` 函数则需要传入两值，并进行加法操作并返回结果。

从 C 层在调用 Lua 脚本层时，首先要把被调用的函数压入栈中，可以通过 `lua_getglobal` 函数把调用函数入栈。

把需要传递给被调用函数的参数用 `lua_push*` 函数按正序压栈。

最后调用一下 `lua_call`，把要传入的参数个数及返回值个数一起传进去。  

当函数调用完毕后，所有的参数以及函数本身都会出栈。紧接着函数的所有返回值这时则被压栈。Lua 会保证返回值都放入栈空间中。函数返回值将按正序压栈（第一个返回值首先压栈），因此在调用结束后，最后一个返回值将被放在栈顶。

### 编译、运行

``` shell
$ cc -o call_lua_func call_lua_func.c -I /usr/local/include/ -L /usr/local/lib -llua
$ ./call_lua_func
In C, calling Lua->sayHello()

This is comming from lua.sayHello.

Back in C again

In C, calling Lua->add()

This is comming from lua.add. arg.a = 1.0 arg.b = 5.0

Back in C again

Returned number : 6
```

## 将 table 传递给函数

在对 Lua 函数调用参数传值时经常涉及到复杂的数据类型，而 Lua 中，复杂数据类型通常用 table 来表示。通过下面代码我们来讨论如何实现表的传递：

创建文件名为 `call_lua_table.lua` 的脚本文件，定义一个接受一个 table 的函数，并且新建一个 talbe，将传入的 table 键值都插入新建的 table 中，并记录数据长度，最后返回新建立的 table：

```lua
function tablehandler(t)
    local returnedt = {numfields = 1}

    for i, v in pairs(t) do
        returnedt.numfields = returnedt.numfields + 1
        returnedt[tostring(i)] = tostring(v)
    end

    io.write("this is comming from table handler. table num fields : ", returnedt.numfields, "\n")
    return returnedt
end
```

编写 C 程序代码，调用函数，传递一个 table 参数，并解析返回值：

```c
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

    if(luaL_loadfile(L, "call_lua_table.lua"))
        error_exit(L, "luaL_loadfile failed.\n");

    if (lua_pcall(L, 0, 0, 0))
        error_exit(L, "lua_pcall failed");

    printf("In C,  calling Lua->tablehandler()\n");
    lua_getglobal(L, "tablehandler");
    lua_newtable(L);                    // Push empty table onto stack, table now at -1
    lua_pushliteral(L, "firstname");    // Puah a key onto stack, table now at -2
    lua_pushliteral(L, "Veinin");       // Puash a value onto stack, table now at -3
    lua_settable(L, -3);                // Take key and value, put into table at -3, then pop key and value so table again at -1

    lua_pushliteral(L, "lastname");
    lua_pushliteral(L, "Guo");
    lua_settable(L, -3);

    if(lua_pcall(L, 1, 1, 0))            // Run function.
        error_exit(L, "lua_pcall failed");

    printf("============ Back in C again, Iterating thru returned table ============\n");

    lua_pushnil(L);                     // Make sure lua_next starts at beginning
    const char *k, *v;
    while(lua_next(L, -2)) {            // Table location at -2 in stack
        v = lua_tostring(L, -1);
        k = lua_tostring(L, -2);

        lua_pop(L, 1);                    // Remove value, leave in place to guide next lua_next().

        printf("%s = %s\n", k, v);
    }

    lua_close(L);

    return 0;
}
```

### 相关函数解析

1.表参数传递惯用手段

```c
lua_newtable(L);                    // Push empty table onto stack, table now at -1
lua_pushliteral(L, "firstname");    // Puah a key onto stack, table now at -2
lua_pushliteral(L, "Veinin");        // Puash a value onto stack, table now at -3
lua_settable(L, -3);                // Take key and value, put into table at -3, then pop key and value so table again at -1
```

对于 C 层代码，可以通过 `lua_newtable` 函数创建一张空表，并将其压栈，该函数它等价于 `lua_createtable(L, 0, 0)`。
然后调用 `lua_pushliteral` 把键和值分别压入栈中，并通过 `lua_settable` 把 `key` 和 `value` 放入 table 中，做一个等价于 `t[k] = v` 的操作，在操作完成后，这个函数会将键和值都弹出栈。

2.表返回值处理

当 C 调用 Lua 函数返回结束时，它把返回值放在了栈上。当返回值是一个 table 时，索引表格的元素就不那么容易了。对于 table ，在返回 C 时，这个 table 在堆栈的顶部，下面的是访问这个 table 的惯用方法：

```c
lua_pushnil(L);                        // Make sure lua_next starts at beginning
const char *k, *v;
while(lua_next(L, -2)) {            // Table location at -2 in stack
    v = lua_tostring(L, -1);
    k = lua_tostring(L, -2);

    lua_pop(L, 1);                    // Remove value, leave in place to guide next lua_next().

    printf("%s = %s\n", k, v);
}
```

为了迭代获取 table 中的值，需要使用 `lua_next` 函数从栈顶弹出一个键，然后通过索引指定表中的一个键值对，对其压栈，在这一步开始之前我们需要调用 `lua_pushnil(L)` 设置第一个键为缺省值，然后再调用 `lua_next` 。通常，我们会在下一个迭代到来之前，把‘值’出栈，但会保留‘键’做下一次迭代操作。

### 编译、运行

``` shell
$ cc -o 05_call_lua_table 05_call_lua_table.c -I /usr/local/include/ -L /usr/local/lib/ -llua
$ ./05_call_lua_table
In C,  calling Lua->tablehandler()

this is comming from table handler. table num fields : 3

============ Back in C again, Iterating thru returned table ============
lastname = Guo

firstname = Veinin

numfields = 3
```

## 总结

你已经能使用 Lua C API 完成对 Lua 脚本的调用工作了，并且可以对函数调用传递参数，在函数返回后，处理函数的返回值。下一步，将介绍最后一节内容，如何从 Lua 中调用 C 代码，并为 Lua 编写可拓展的 C 程序库。