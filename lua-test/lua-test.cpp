// LuaTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>


extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}


int add(lua_State* L)
{
	double a = lua_tonumber(L, 1);
	double b = lua_tonumber(L, 2);

	lua_pushnumber(L, a + b);

	return 1;	// number of return value
}


void call_script(lua_State* L)
{
/*
	function max(a, b)
		if a > b then
			print(a.."가 큽니다")
			return a, b
		else
			print(b.."가 큽니다")
			return b, a
		end
	end
*/

	double a = 10, b = 5;

	// load script
	luaL_dofile(L, "Max.lua");
	lua_getglobal(L, "max");

	// add parameter to stack
	lua_pushnumber(L, a);
	lua_pushnumber(L, b);

	// call (no. param=2, no. return=2)
	lua_call(L, 2, 2);

	auto big = lua_tonumber(L, 1);
	auto small = lua_tonumber(L, 2);

	std::cout << "max(" << a << ", " << b << ")" << std::endl;
	std::cout << "\tbig: " << big << std::endl;
	std::cout << "\tsmall" << small << std::endl;
}

int main()
{
	char buf[256];
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	call_script(L);

	// register C++ function
	lua_register(L, "add", add);

	std::cout << "Lua version: " << LUA_VERSION << std::endl;
	std::cout << "Lua release version: " << LUA_RELEASE << std::endl;
	std::cout << LUA_COPYRIGHT << " " << LUA_AUTHORS << std::endl;

	while (true)
	{
		std::cout << "> ";
		fgets(buf, sizeof(buf), stdin);

		if (0 == strcmp(buf, "quit\n"))
			break;

		luaL_loadbuffer(L, buf, strlen(buf), "console");
		lua_call(L, 0, 0);
	}

	lua_close(L);

	return 0;
}


