//
// Created by cattenlinger on 20-3-5.
//

#ifndef LDPF4L_MOD_CANVAS_H
#define LDPF4L_MOD_CANVAS_H

#include "lua.h"

static const char *MOD_t_CANVAS = "t_Canvas";

#define LUA_CANVAS_NAME "Canvas"

LUAMOD_API int (luaopen_canvas)(lua_State *L);

#endif //LDPF4L_MOD_CANVAS_H
