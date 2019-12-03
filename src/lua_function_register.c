//
// Created by cattenlinger on 19-11-30.
//

#include "lua_function_register.h"

#include "lprefix.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "mod/mod_host_log.h"

static const luaL_Reg loadedExternalLibs[] = {
        {LUA_HOST_LOGGER_NAME, luaopen_log},
        {NULL, NULL}
};

LUALIB_API void luaL_openExtraLibs(lua_State *L) {
    const luaL_Reg *lib;
    /* "require" functions from 'loadedlibs' and set results to global table */
    for (lib = loadedExternalLibs; lib->func; lib++) {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);  /* remove lib */
    }
}