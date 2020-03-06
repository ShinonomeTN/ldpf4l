//
// Created by cattenlinger on 20-3-4.
//

#ifndef LDPF4L_MOD_BUFFER_H
#define LDPF4L_MOD_BUFFER_H

struct Buffer;

static const char *MOD_t_BUFFER = "t_Buffer";

#define LUA_BUFFER_NAME "Buffer"

LUAMOD_API int (luaopen_buffer)(lua_State *L);

#endif //LDPF4L_MOD_BUFFER_H
