//
// Created by cattenlinger on 20-2-28.
//

#include "lauxlib.h"
#include "lualib.h"

#include "mod_num_array.h"


#define NA_OUT_OF_RANGE "index out of boundary"

typedef struct number_array {
    long size;
    double values[1];
} number_array;

#define t_NumberArray "ldpf4l.type.LuaBookNumberArray"

/*
 *
 *
 * */

static int _lua_new(lua_State *L) {
    int n = luaL_checkinteger(L, 1);
    size_t size = sizeof(number_array) + (n - 1) * sizeof(double);
    number_array *array = lua_newuserdata(L, size);
    array->size = n;

    // Give this user data a type info
    luaL_setmetatable(L, t_NumberArray);

    return 1;
}

static int _lua_set(lua_State *L) {
    number_array *array = luaU_checkoutType(L, t_NumberArray);
    int index = luaL_checkinteger(L, 2);
    double value = luaL_checknumber(L, 3);

    luaL_argcheck(L, 1 <= index && index <= array->size, 2, NA_OUT_OF_RANGE);

    array->values[index - 1] = value;
    return 0;
}

static int _lua_get(lua_State *L) {
    number_array *array = luaU_checkoutType(L, t_NumberArray);
    int index = luaL_checkinteger(L, 2);

    luaL_argcheck(L, 1 <= index && index <= array->size, 2, NA_OUT_OF_RANGE);

    lua_pushnumber(L, array->values[index - 1]);

    return 1;
}

static int _lua_get_size(lua_State *L) {
    number_array *array = luaU_checkoutType(L, t_NumberArray);

    lua_pushnumber(L, array->size);
    return 1;
}

static int _lua_to_string(lua_State *L) {
    number_array *array = luaU_checkoutType(L, t_NumberArray);

    lua_pushfstring(L, t_NumberArray"[%d]", array->size);

    return 1;
}

/*
 *
 * function define
 *
 * */

LUA_LIB_DEFINE(ldpf4l_NumberArray)
        FUNCTION("new", _lua_new)
LUA_LIB_END

/*
 *
 * method define
 *
 * */

LUA_TYPE_DEFINE(t_NumberArray)
    MEMBER("__newindex", _lua_set)
    MEMBER("__index",    _lua_get)
    MEMBER("__len",      _lua_get_size)
    MEMBER("__tostring", _lua_to_string)
LUA_TYPE_END

/*
 *
 * register
 *
 * */
LUA_LIB_EXPORT(ldpf4l_NumberArray)
    EXPORT_LIB(ldpf4l_NumberArray)
    EXPORT_TYPE(t_NumberArray)
LUA_LIB_EXPORT_END