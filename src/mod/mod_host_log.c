//
// Created by cattenlinger on 19-11-26.
//

#define LUA_LIB

#include "mod_host_log.h"

#include "lauxlib.h"

#include "../utils/log.h"

static int luaM_setLogLevel(lua_State *L) {
    int n = luaL_checkinteger(L, 1);
    if(n < 0 || n >= 6) {

    }

    log_set_level(n);
    return 0;
}

static int luaM_logLevel(lua_State *L) {
    int logLevel = log_get_level();
    lua_pushnumber(L, logLevel);
    return 1;
}

static int luaM_logLevelName(lua_State *L) {
    const char *logLevelName = log_get_level_name();
    lua_pushstring(L, logLevelName);
    return 1;
}

static int luaM_logTrace(lua_State *L) {
    log_trace(lua_tostring(L, -1));
    return 0;
}

static int luaM_logInfo(lua_State *L) {
    log_info(lua_tostring(L, -1));
    return 0;
}

static int luaM_logWarn(lua_State *L) {
    log_warn(lua_tostring(L, -1));
    return 0;
}

static int luaM_logError(lua_State *L) {
    log_error(lua_tostring(L, -1));
    return 0;
}

static int luaM_logFatal(lua_State *L) {
    log_fatal(lua_tostring(L, -1));
    return 0;
}

static const struct luaL_Reg log[] = {
        {"level",     luaM_logLevel},
        {"info",      luaM_logInfo},
        {"trace",     luaM_logTrace},
        {"warn",      luaM_logWarn},
        {"error",     luaM_logError},
        {"fatal",     luaM_logFatal},
        {"levelName", luaM_logLevelName},
        {NULL, NULL}
};

LUAMOD_API int luaopen_log(lua_State *L) {
    luaL_newlib(L, log);
    return 1;
}