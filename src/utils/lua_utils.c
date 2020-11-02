//
// Created by cattenlinger on 19-11-22.
//
#include <lua.h>
#include <stdio.h>

#include "lua_utils.h"
#include "lauxlib.h"

void *luaU_checkUserDataNotNull(lua_State *L, const char *typeName, int pos) {
    void *data = luaL_checkudata(L, pos, typeName);
    luaL_argcheck(L, data != NULL, 1, "error: value should not be null");
    return data;
}