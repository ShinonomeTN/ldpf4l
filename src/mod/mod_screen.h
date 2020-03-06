//
// Created by cattenlinger on 20-3-4.
//

#ifndef LDPF4L_MOD_SCREEN_H
#define LDPF4L_MOD_SCREEN_H

#include "lua.h"

const static char *MOD_t_SCREEN = "t_Screen";

#define LUA_SCREEN_NAME "Screen"

LUAMOD_API int (luaopen_screen)(lua_State *L);

#endif //LDPF4L_MOD_SCREEN_H
