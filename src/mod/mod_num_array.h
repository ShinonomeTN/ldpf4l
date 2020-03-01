//
// Created by cattenlinger on 20-2-28.
//

#ifndef LDPF4L_MOD_NUM_ARRAY_H
#define LDPF4L_MOD_NUM_ARRAY_H

#include "lua.h"

typedef struct number_array number_array ;

#define LUA_NUMBER_ARRAY_NAME "NumberArray"
LUAMOD_API int (luaopen_number_array)(lua_State *L);
#endif //LDPF4L_MOD_NUM_ARRAY_H
