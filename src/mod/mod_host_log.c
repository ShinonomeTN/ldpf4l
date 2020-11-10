//
// Created by cattenlinger on 19-11-26.
//

#include "mod_host_log.h"

#include "lauxlib.h"

#include "../utils/log.h"

static int lf_setLogLevel(lua_State *L) {
    int n = luaL_checkinteger(L, 1);
    if(n < 0 || n >= 6) {
        luaL_error(L, "log level must from 0 to 6");
    }

    log_set_level(n);
    return 0;
}

static int lf_logLevel(lua_State *L) {
    int logLevel = log_get_level();
    lua_pushnumber(L, logLevel);
    return 1;
}

static int lf_logLevelName(lua_State *L) {
    const char *logLevelName = log_get_level_name();
    lua_pushstring(L, logLevelName);
    return 1;
}

static int lf_logTrace(lua_State *L) {
    log_trace(lua_tostring(L, -1));
    return 0;
}

static int lf_logInfo(lua_State *L) {
    log_info(lua_tostring(L, -1));
    return 0;
}

static int lf_logWarn(lua_State *L) {
    log_warn(lua_tostring(L, -1));
    return 0;
}

static int lf_logError(lua_State *L) {
    log_error(lua_tostring(L, -1));
    return 0;
}

static int lf_logFatal(lua_State *L) {
    log_fatal(lua_tostring(L, -1));
    return 0;
}

LUA_LIB_DEFINE(ldpf4l_Logging)
    MEMBER("level", lf_logLevel)
    MEMBER("info", lf_logInfo)
    MEMBER("trace", lf_logTrace)
    MEMBER("warn", lf_logWarn)
    MEMBER("error", lf_logError)
    MEMBER("fatal", lf_logFatal)
    MEMBER("levelName", lf_logLevelName)
    MEMBER("setLevel", lf_setLogLevel)
LUA_LIB_END

LUA_LIB_EXPORT(ldpf4l_Logging)
    EXPORT_LIB(ldpf4l_Logging)
LUA_LIB_EXPORT_END