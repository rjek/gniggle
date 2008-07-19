/*
 * lua.c
 * This file is part of Gniggle
 *
 * Copyright (C) 2008 - Rob Kendrick <rjek@rjek.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to
 * do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>

#include "dictionary.h"

/* dictionary functions */

static int l_gniggle_dict_trim_qu(lua_State *L)
{
	const unsigned char *in = luaL_checkstring(L, 1);
	unsigned char *out = gniggle_dictionary_trim_qu(in);
	
	lua_pushstring(L, out);
	free(out);

	return 1;
}

static int l_gniggle_dict_restore_qu(lua_State *L)
{
	const unsigned char *in = luaL_checkstring(L, 1);
	unsigned char *out = gniggle_dictionary_restore_qu(in);
	
	lua_pushstring(L, out);
	free(out);

	return 1;
}

static int l_gniggle_dict_word_qualifies(lua_State *L)
{
	const unsigned char *word = luaL_checkstring(L, 1);
	const int maxlen = luaL_checknumber(L, 2);
	bool r = gniggle_dictionary_word_qualifies(word, maxlen);
	
	lua_pushboolean(L, r);
	return 1;
}

static int l_gniggle_dict_new(lua_State *L)
{
	return 0;
}

static int l_gniggle_dict_load_file(lua_State *L)
{
	return 0;
}

static int l_gniggle_dict_new_from_file(lua_State *L)
{
	return 0;
}

static int l_gniggle_dict_delete(lua_State *L)
{
	return 0;
}

static int l_gniggle_dict_add(lua_State *L)
{
	return 0;
}

static int l_gniggle_dict_lookup(lua_State *L)
{
	return 0;
}

static int l_gniggle_dict_size(lua_State *L)
{
	return 0;
}

static int l_gniggle_dict_iter(lua_State *L)
{
	return 0;
}

static int l_gniggle_dict_next(lua_State *L)
{
	return 0;
}

static int l_gniggle_dict_iter_delete(lua_State *L)
{
	return 0;
}

static int l_gniggle_dict_dump(lua_State *L)
{
	return 0;
}

static int l_gniggle_dict_undump(lua_State *L)
{
	return 0;
}

/* Lua interface entry points */

static const struct luaL_Reg gniggle[] = {
	{ "dict_trim_qu", 	l_gniggle_dict_trim_qu },
	{ "dict_restore_qu", 	l_gniggle_dict_restore_qu },
	{ "dict_word_qualifies", l_gniggle_dict_word_qualifies },
	{ "dict_new", 		l_gniggle_dict_new },
	{ "dict_load_file", 	l_gniggle_dict_load_file },
	{ "dict_new_from_file", l_gniggle_dict_new_from_file },
	{ "dict_delete", 	l_gniggle_dict_delete },
	{ "dict_add", 		l_gniggle_dict_add },
	{ "dict_lookup", 	l_gniggle_dict_lookup },
	{ "dict_size", 		l_gniggle_dict_size },
	{ "dict_iter", 		l_gniggle_dict_iter },
	{ "dict_next", 		l_gniggle_dict_next },
	{ "dict_iter_delete", 	l_gniggle_dict_delete },
	{ "dict_dump", 		l_gniggle_dict_dump },
	{ "dict_undump", 	l_gniggle_dict_undump },
	
	{ NULL, NULL }
};

int luaopen_luagniggle(lua_State *L)
{
	luaL_register(L, "gniggle", gniggle);
	return 1;
}
