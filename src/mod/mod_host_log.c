//
// Created by cattenlinger on 19-11-26.
//

#include "mod_host_log.h"

#include "lauxlib.h"

#include "../utils/log.h"

static int ll_setLogLevel(lua_State *L) {
    int n = luaL_checkinteger(L, 1);
    if(n < 0 || n >= 6) {
        luaL_error(L, "log level must from 0 to 6");
    }

    log_set_level(n);
    return 0;
}

static int ll_logLevel(lua_State *L) {
    int logLevel = log_get_level();
    lua_pushnumber(L, logLevel);
    return 1;
}

static int ll_logLevelName(lua_State *L) {
    const char *logLevelName = log_get_level_name();
    lua_pushstring(L, logLevelName);
    return 1;
}

static int ll_logTrace(lua_State *L) {
    log_trace(lua_tostring(L, -1));
    return 0;
}

static int ll_logInfo(lua_State *L) {
    log_info(lua_tostring(L, -1));
    return 0;
}

static int ll_logWarn(lua_State *L) {
    log_warn(lua_tostring(L, -1));
    return 0;
}

static int ll_logError(lua_State *L) {
    log_error(lua_tostring(L, -1));
    return 0;
}

static int ll_logFatal(lua_State *L) {
    log_fatal(lua_tostring(L, -1));
    return 0;
}

LUA_LIB_DEFINE(ldpf4l_Logging)
        MEMBER("level", ll_logLevel)
        MEMBER("info", ll_logInfo)
        MEMBER("trace", ll_logTrace)
        MEMBER("warn", ll_logWarn)
        MEMBER("error", ll_logError)
        MEMBER("fatal", ll_logFatal)
        MEMBER("levelName", ll_logLevelName)
        MEMBER("setLevel", ll_setLogLevel)
LUA_LIB_END

LUA_LIB_EXPORT(ldpf4l_Logging)
    EXPORT_LIB(ldpf4l_Logging)
LUA_LIB_EXPORT_END