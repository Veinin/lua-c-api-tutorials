# Lua C API 教程 - 入门

Lua 的 C API 是一个 C 代码与 Lua 进行交互的函数集，也就是宿主程序跟 Lua 通讯用的一组 C 函数。它由以下部分组成：读写Lua全局变量的函数，调用Lua函数的函数，运行 Lua 代码片段的函数，注册 C 函数然后可以在 Lua 中被调用的函数，等等。

## 头文件

在任何调用Lua的 C程序中都必选包含以下3个文件 ：

```c
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
```

如果是 C++ 程序，请包含头文件 `lua.hpp`, 这个文件实际上也是引入了上面 3 头文件，它看起来是这样的：

```c
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
```

在上面的头文件中其中，所有的 API 函数按相关的类型以及常量都声明在头文件 `lua.h` 中。
而另一个相关头文件 `lauxlib.h` 则是API接口的一个辅助库（auxlib），其中定义的函数都以luaL_开头（如lual_loadbuffer），辅助库利用了lua.h中提供的基础函数提供了更高层次的抽象。auxlib 没有存取 Lua 内部的权限，它所完成的所有工作都是通过正式的基本 API。

## 编译和连接

没有比编译和连接你的第一个 C 程序更难的事情了。在编译之前你需要了解一些编译选项，下面是一些典型的编译命令：

- `cc`，你系统里面的 C 代码编译器，它可以是 `cc` 、 `gcc` 或其他的编译器。
- `-Wall`，显示所有警告，直到所有错误和警告已修复之前，你都可以选择不插入这个选项。
- `-o hello`，编程后的执行程序名称，这里会得到一个名为 `hello` 的可执行程序。
- `hello.c`，需要编译的 C 程序名称。
- `-I/path/to/Lua/include`，每个 C 编译器都会在某个地方检察程序包含的头文件信息，但大部分情况下，Lua C API 的头文件并不在你程序所处的目录，所以需要你告诉编译器去哪个地方找到这些头文件。
- `-L/path/to/lua/libraries`，每个 C 编译器都会在特定的地方进行连接，但通常 Lua 的库并不会在你自己编写的 C 程序目录下，所以你需要告诉编译器去哪里找到它们。
- `-llibLuaName`，告诉 C 编译器链接 Lua 库的名称。
- `-lm`，数学库的链接。

## 第一个示例程序

实现一个独立的 Lua 解释器。

```c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

int main(void)
{
     char buff[256];
     int error;
     lua_State *L = luaL_newstate(); //opens lua
     luaL_openlibs(L); //open the standar libraries

     while(fgets(buff, sizeof(buff), stdin) != NULL) {
          error = luaL_loadstring(L, buff) || lua_pcall(L, 0, 0, 0);
          if(error) {
               fprintf(stderr, "%s\n", lua_tostring(L, -1));
               lua_pop(L, 1);
          }
     }

     lua_close(L);
     return 0;
}

```

`luaL_newstate` 函数创建一个新的 Lua 状态机。它以一个基于标准 C 的 `realloc` 函数实现的内存分配器调用 `lua_newstate` 。
Lua 中没用定义任何全局变量，它所有状态都保存在动态结构lua_State中，而指向这个函数的指针做为所有Lua函数的第一个参数，这样的实现方式使得Lua能够重入且在多线程中的使用做好准备。

函数 `luaL_openlibs` 打开指定状态机中的所有 Lua 标准库。

当状态机标准库载入后，对于用户输入的每一行，C 程序首先调用 `luaL_loadstring` 将字符串加载为 Lua 代码块，并通过 `lua_pcall` 以保护模式运行从chunk中调用返回的代码。`lua_pcall` 在没有错误的情况下返回0，如有错误发送，错误信息将被压入栈中，我们可以通过 `lua_tostring` 来得到这条信息，输出它，最后调用 `lua_pop` 将它从栈中删除。

### 编译、运行

``` shell
$ cc -o first first.c -I/usr/local/include -L/usr/local/lib -llua -lm
$ ./first
print("hello")
hello

hehe
[string "hehe..."]:2: syntax error near
```

## 栈

当 Lua 与 C 之间交换数据时我们面临两个问题，一个是动态与静态类型系统的不匹配，另一个则是自动和手动内存管理的不一致。
Lua 使用一个虚拟栈来与 C 互相传值，栈上的的每个元素都是一个 Lua 值。第一次调用 Lua 时，首先值会压入栈中，然后调用 Lua （这个值会被弹出），我们要做的就是把值按类型用不同的函数压入栈中，然后再用相应的函数把值从栈中取出。当然，对于栈来说，它始终遵循 `LIFO` 规则，你可以自由的查询栈上的任何元素，也可以在任何位置插入和删除元素。

### 压入元素

API 有一系列的压入函数，如空值: `lua_pushnil()`、整数 `lua_pushnumber()`、任意字符串 `lua_pushlstring()`、C风格字符串 `lua_pushstring()` ...

``` c
void lua_pushnil(lua_State *L);
void lua_pushboolean(lua_State *L, int bool);
void lua_pushnumber(lua_State *L, lua_Number n);
void lua_pushinteger(lua_State *L, lua_Integer n);
void lua_pushunsigned(lua_State *L, lua_Unsigned n);
void lua_pushlstring(lua_State *L, const char *s, size_t len);
void lua_pushstring(lua_State *L, const char * s);
```

当你使用 Lua API 时， 就有责任保证做恰当的调用。 特别需要注意的是， 你有责任控制不要堆栈溢出。 你可以使用 `lua_checkstack` 这个函数来扩大可用堆栈的尺寸。无论你何时从 C 调用 Lua，都要保证栈空间至少有 `LUA_MINSTACK` 大小的空闲，这个值定义在了 `lua.h` 中，通常位20。因此，只要你不是不断的把数据压栈， 通常你不用关心堆栈大小。

``` c
int lua_checkstack(lua_State *L, int sz);
```

### 查询元素

栈中的元素通过索引值查询，第一个元素索引为 1，最后一个元素位 n。当索引值为负时，从栈顶开始找，如 -1 也可以表示最后一个元素，-2 指栈顶的前一个元素。例如可以调用 `lua_tostring(L, -1)` 来调用栈顶元素。
Lua C API 提供了一套 `lua_is*` 函数来检查指定类型的值，如 `lua_isnumber`，`lua_isstring` 等，这些函数只检查是否能转换成指定类型。

```c
int lua_is...(lua_State *L, int index);
```

最后，还有一个的 lua_type 函数，它返回栈中元素的类型。在 `lua.h` 中，每种类型都定义了一个常量 `LUA_TNIL` 、`LUA_TBOOLEAN` 、`LUA_TNUMBER` 、`LUA_TSTRING` 、`LUA_TTABLE` 、`LUA_TFUNCTION` 、`LUA_TUSERDATA` 。
为了从栈中获取值，还提供了每种类值相对的 `lua_to*` 函数。如给定元素不正确则返回 0 或 NULL。

``` c
int          lua_toboolean(lua_State *L, int index);
lua_Number   lua_tonumber(lua_State *L, int index);
lua_Integer  lua_tointeger(lua_State *L, int index);
lua_Unsigned lua_tounsigned(lua_State *L, int index);
const char * lua_tostring(lua_State *L, int index);
size_t       lua_strlen(lua_State *L, int index);
```

在检查一个类型值时，通常你需要用 `lua_is*` 函数来检查，但在Lua 5.2 中新增了如下 `lua_to*` 函数，在传入值 `isnum`，如果 `isnum` 不是 `NULL` ， `*isnum` 会被设为操作是否成功。

``` c
lua_Number   lua_tonumberx(lua_State *L, int index, int *isnum);
lua_Integer  lua_tointegerx(lua_State *L, int index, int *isnum);
lua_Unsigned lua_tounsignedx(lua_State *L, int index, int *isnum);
```

### 其他栈操作

除了上面所列的栈操作函数，在 C 和 Lua 交换值时，我们还可以通过以下 API 来对栈进行操作：

``` c
int  lua_gettop(lua_State *L);
void lua_settop(lua_State *L, int index);
void lua_pushvalue(lua_State *L, int index);
void lua_remove(lua_State *L, int index);
void lua_insert(lua_State *L, int index);
void lua_replace(lua_State *L, int index);
void lua_copy(la_State *L, int formidx, int toidx);
void lua_pop(lua_State *L, int n);
```

`lua_gettop` 返回栈顶的索引值；
`lua_settop` 它将把堆栈的栈顶设为这个索引。如果新的栈顶比原来的大，超出部分的新元素将被填为nil，如果index为0，把栈上所有元素移除；
`lua_pushvalue` 把栈上给定索引处的元素作一个副本压栈；
`lua_remove` 删除指定索引位置的值；
`lua_insert` 将栈顶值移动到指定位置；
`lua_replace` 把栈顶元素放置到给定位置而不移动其它元素 （因此覆盖了那个位置处的值），然后将栈顶元素弹出。
`lua_copy` 从索引 fromidx 处复制一个值到一个有效索引 toidx 处，覆盖那里的原有值。 不会影响其它位置的值。lua_pop从栈中弹出 n 个元素。
`lua_settop(L,0)` 可以清空栈顶，而 `lua_pop` 真是利用了这一技巧:

``` c
#define lua_pop(L,n) lua_settop(L, -(n)-1)
```

### 栈操作实例

``` c
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

     lua_pushvalue(L, 1);  //将索引为1的元素压入栈顶
     stackDump(L);

     lua_settop(L, 4);     //设置栈顶为4
     stackDump(L);

     lua_insert(L, 3);     //移动栈顶元素到3
     stackDump(L);

     lua_replace(L, 3);     //将栈顶元素移动到3位置，弹出栈顶元素
     stackDump(L);

     lua_copy(L, 2, 3);     //将索引为2的元素复制到3位置
     stackDump(L);

     lua_pop(L, 3);         //剩余3个元素出栈
     stackDump(L);

     lua_close(L);
     return 0;
}
```

编译、运行：

``` shell
$ cc -o stack_test stack_test.c -I /usr/local/include/ -L /usr/local/lib/ -llua
$ ./stack_test
true 100 10.011 hello nil

true 100 10.011 hello nil true

true 100 10.011 hello

true 100 hello 10.011

true 100 10.011

true 100 100
```

## 总结

现在，你可以完成一个独立的 Lua 解释器编写工作，并在解释器程序中，运行一些基础的 Lua 脚本程序。你还需要了解 Lua 的栈是什么，如何使用堆栈操作来实现数据操作。下一步，将介绍如何在 C 程序中调用 Lua 脚本。