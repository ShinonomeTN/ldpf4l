//
// Created by cattenlinger on 19-11-22.
//
#include <lua.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lua_utils.h"

String *luaUtils_dumpStack(lua_State *L) {
    int top = lua_gettop(L);
    String *string = string_emptyString();

    char buffer[1024];

    for (int i = 0; i < top; ++i) {
        int type = lua_type(L, i);
        switch (type) {
            case LUA_TSTRING: {
                snprintf(buffer, 1024, "% 4d: %s\n", i, lua_tostring(L, i));
                string_appendChars(string, buffer);
                break;
            }

            case LUA_TBOOLEAN: {
                snprintf(buffer, 1024, "% 4d: %s\n", i, lua_toboolean(L, i) ? "true" : "false");
                string_appendChars(string, buffer);
                break;
            }

            case LUA_TNUMBER: {
                snprintf(buffer, 1024, "% 4d: %f\n", i, lua_tonumber(L, i));
                string_appendChars(string, buffer);
                break;
            }

            default:
                snprintf(buffer, 1024, "% 4d: %s\n", i, lua_typename(L, i));
                string_appendChars(string, buffer);
                break;
        }
    }

    return string;
}
