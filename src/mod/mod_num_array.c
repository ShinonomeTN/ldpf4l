//
// Created by cattenlinger on 20-2-28.
//

#define LUA_LIB

#include "lauxlib.h"
#include "lualib.h"

#include "mod_num_array.h"
#include "../utils/lua_utils.h"


#define NA_OUT_OF_RANGE "index out of boundary"

typedef struct number_array {
    long size;
    double values[1];
} number_array;
#define _LUA_TYPE_NAME "LuaBook.NumberArray"

/*
 *
 *
 * */

static int _lua_new(lua_State *L) {
    int n = luaL_checkinteger(L, 1);
    size_t size = sizeof(number_array) + (n - 1) * sizeof(double);
    number_array *array = (number_array *) lua_newuserdata(L, size);
    array->size = n;

    // Give this user data a type info
    luaL_setmetatable(L, _LUA_TYPE_NAME);

    return 1;
}

static int _lua_set(lua_State *L) {
    number_array *array = (number_array *) luaU_checkoutSelf(L, _LUA_TYPE_NAME);
    int index = luaL_checkinteger(L, 2);
    double value = luaL_checknumber(L, 3);

    luaL_argcheck(L, 1 <= index && index <= array->size, 2, NA_OUT_OF_RANGE);

    array->values[index - 1] = value;
    return 0;
}

static int _lua_get(lua_State *L) {
    number_array *array = (number_array *) luaU_checkoutSelf(L, _LUA_TYPE_NAME);
    int index = luaL_checkinteger(L, 2);

    luaL_argcheck(L, 1 <= index && index <= array->size, 2, NA_OUT_OF_RANGE);

    lua_pushnumber(L, array->values[index - 1]);

    return 1;
}

static int _lua_get_size(lua_State *L) {
    number_array *array = (number_array *) luaU_checkoutSelf(L, _LUA_TYPE_NAME);

    lua_pushnumber(L, array->size);
    return 1;
}

static int _lua_to_string(lua_State *L) {
    number_array *array = (number_array *) luaU_checkoutSelf(L, _LUA_TYPE_NAME);

    lua_pushfstring(L, _LUA_TYPE_NAME"[%d]", array->size);

    return 1;
}

/*
 *
 * function define
 *
 * */

static const struct luaL_Reg num_array_Function[] = {
        {"new", _lua_new},
        {NULL, NULL}
};

/*
 *
 * method define
 *
 * */

static const struct luaL_Reg num_array_Method[] = {
        {"__newindex", _lua_set},
        {"__index",    _lua_get},
        {"__len",      _lua_get_size},
        {"__tostring", _lua_to_string},
        {NULL, NULL}
};

/*
 *
 * register
 *
 * */
LUAMOD_API int luaopen_number_array(lua_State *L) {
    /* Register type */
    luaL_newmetatable(L, _LUA_TYPE_NAME);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, num_array_Method, 0);

    /* Register function */
    luaL_newlib(L, num_array_Function);
    return 1;
}