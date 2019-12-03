//
// Created by cattenlinger on 19-11-30.
//

#ifndef LUA_TEST_MOD_HOST_LOG_H
#define LUA_TEST_MOD_HOST_LOG_H

#include "lua.h"

#define LUA_HOST_LOGGER_NAME "log"
LUAMOD_API int (luaopen_log)(lua_State *L);

#endif //LUA_TEST_MOD_HOST_LOG_H
