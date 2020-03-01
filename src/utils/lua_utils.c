//
// Created by cattenlinger on 19-11-22.
//
#include <lua.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lua_utils.h"
#include "lauxlib.h"
#include "log.h"

void luaU_printStack(lua_State *L) {
    String *s = luaU_dumpStack(L);
    log_debug("Stack trace \n%s", string_value(s));
    string_free(s);
}

String *luaU_dumpStack(lua_State *L) {
    int top = lua_gettop(L);
    String *string = string_emptyString();

    char buffer[1024];

    for (int i = 1; i < top; ++i) {
        int type = lua_type(L, i);
        const char *typeName = lua_typename(L, type);
        switch (type) {
            case LUA_TSTRING:
                snprintf(buffer, 1024, "% 4d: %s (%s)", i, lua_tostring(L, i), typeName);
                string_appendChars(string, buffer);
                break;

            case LUA_TBOOLEAN:
                snprintf(buffer, 1024, "% 4d: %s (%s)", i, lua_toboolean(L, i) ? "true" : "false", typeName);
                string_appendChars(string, buffer);
                break;


            case LUA_TNUMBER:
                snprintf(buffer, 1024, "% 4d: %f (%s)", i, lua_tonumber(L, i), typeName);
                string_appendChars(string, buffer);
                break;

            default:
                snprintf(buffer, 1024, "% 4d: %s (%s)", i, lua_tostring(L, i), typeName);
                string_appendChars(string, buffer);
                break;
        }
        if (i < (top - 1)) {
            snprintf(buffer, 1024, "%s", "\n");
            string_appendChars(string, buffer);
        }
    }

    return string;
}

void *luaU_checkUserDataNotNull(lua_State *L, const char *typeName) {
    void *data = luaL_checkudata(L, 1, typeName);
    luaL_argcheck(L, data != NULL, 1, "error: value should not be null");
    return data;
}