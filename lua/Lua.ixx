module;
#include <iostream>
#include "vendor/lua.hpp"

export module Lua;

export class Lua
{
public:

	Lua()
	{
		Init();
	}

	~Lua()
	{
		lua_close(state);
	}

private:

	lua_State* state;

	void Init()
	{
		
		state = luaL_newstate(); // Make a new lua state
		if (state == NULL)
			throw "Lua state could not be created. (Usually due to insuffecient memory)";
		luaL_openlibs(state); // Load std libs
		lua_gc(state, LUA_GCGEN, 20, 100); // Enable generational GC

		//Test
		lua_register(state, "CppFunc", [](lua_State* s)->int { std::cout << "I'm a cpp func that was called from lua!"; return 0; });
	}

public:

	void LoadScript(const char* fileName)
	{
		if (luaL_loadfile(state, fileName))
		{
			std::cerr << "Something went wrong whilst loading the specified lua file.\nErr: "
				<< lua_tostring(state, -1) << '\n';
			lua_pop(state, 1); // Pop error obj off stack
		}
	}

	void Run()
	{
		if (lua_pcall(state, 0, LUA_MULTRET, 0))
		{
			std::cerr << "Something went wrong during execution.\nErr: "
				<< lua_tostring(state, -1) << '\n';
			lua_pop(state, 1); // Pop error obj off stack
		}
	}
};

