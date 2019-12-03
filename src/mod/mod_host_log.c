//
// Created by cattenlinger on 19-11-26.
//

#define LUA_LIB

#include <stdlib.h>

#include "mod_host_log.h"

#include "lprefix.h"
#include "lauxlib.h"
#include "lualib.h"

#include "../utils/log.h"

static int luaM_logLevel(lua_State *L) {
    int logLevel = log_get_level();
    lua_pushnumber(L, logLevel);
    return 1;
}

static int luaM_logLevelName(lua_State *L) {
    const char* logLevelName = log_get_level_name();
    lua_pushstring(L, logLevelName);
    return 1;
}

static int luaM_logInfo(lua_State *L) {
    log_info(lua_tostring(L, -1));
    return 0;
}

static int luaM_logTrace(lua_State *L) {
    log_trace(lua_tostring(L, -1));
    return 0;
}

static const struct luaL_Reg log[] = {
        {"level", luaM_logLevel},
        {"info", luaM_logInfo},
        {"trace",luaM_logTrace},
        {"levelName",luaM_logLevelName},
        {NULL, NULL}
};

LUAMOD_API int luaopen_log(lua_State *L) {
    luaL_newlib(L, log);
    return 1;
}