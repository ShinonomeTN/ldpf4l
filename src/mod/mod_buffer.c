//
// Created by cattenlinger on 20-3-4.
//

#include <stdlib.h>

#include "lprefix.h"
#include "lauxlib.h"
#include "lualib.h"

#include "../utils/lua_utils.h"

#include "mod_buffer.h"

#define t_BUFFER "t_Buffer"

typedef struct Buffer {
    unsigned long size;
    unsigned char buffer[1];
} Buffer;

static int _buffer_create(lua_State *L) {
    int size = luaL_checkinteger(L, 1);
    if (size <= 0) luaL_error(L, "error: buffer size must bigger than 0");

    int sizeBuff = sizeof(Buffer) + (size - 1);
    Buffer *buffer = (Buffer *) lua_newuserdata(L, sizeBuff);
    buffer->size = size;

    luaL_setmetatable(L, t_BUFFER);
    return 1;
}

static int _buffer_size(lua_State *L) {
    Buffer *buffer = (Buffer *) luaU_checkoutSelf(L, t_BUFFER);
    lua_pushinteger(L, buffer->size);
    return 1;
}

static int _buffer_set(lua_State *L) {
    Buffer *buffer = (Buffer *) luaU_checkoutSelf(L, t_BUFFER);
    unsigned long index = luaL_checkinteger(L, 2) + 1;
    if (!(index > 0 && index <= buffer->size)) luaL_error(L, "error: index out of range");
    buffer->buffer[index] = luaL_checkinteger(L, 2);

    return 0;
}

static int _buffer_get(lua_State *L) {
    Buffer *buffer = (Buffer *) luaU_checkoutSelf(L, t_BUFFER);
    unsigned long index = luaL_checkinteger(L, 2) + 1;
    if (!(index > 0 && index <= buffer->size)) luaL_error(L, "error: index out of range");
    lua_pushinteger(L, buffer->buffer[index]);

    return 1;
}

static int _buffer_copy_to(lua_State *L) {
    Buffer *buffer = (Buffer *) luaU_checkoutSelf(L, t_BUFFER);
    Buffer *target = (Buffer *) luaU_checkUserDataNotNull(L, t_BUFFER, 2);
    int length = buffer->size < target->size ? target->size : buffer->size;

    int i;
    for (i = 0; i < length; ++i) {
        target->buffer[i] = buffer->buffer[i];
    }

    lua_pushinteger(L, i + 1);
    return 1;
}

static int _buffer_copy_range(lua_State *L) {
    Buffer *buffer = (Buffer *) luaU_checkoutSelf(L, t_BUFFER);

    int from = luaL_checkinteger(L, 2) - 1;
    if (!(from >= 0 && from <= buffer->size)) luaL_error(L, "error: source start position out of range");
    int end = luaL_checkinteger(L, 3) - 1;
    if (!(end >= 0 && end <= buffer->size)) luaL_error(L, "error: source start position out of range");
    if (from > end) luaL_error(L, "error: source start index after end");

    Buffer *target = (Buffer *) luaU_checkUserDataNotNull(L, t_BUFFER, 4);
    int start = luaL_checkinteger(L, 5) - 1;
    if (!(start >= 0 && start <= target->size)) luaL_error(L, "error: target start position out of range");
    if (!(end >= 0 && (start - end) <= target->size)) luaL_error(L, "error: target start position out of range");

    int i;
    for (i = 0; i < from - end; i++) {
        target->buffer[i + start] = buffer->buffer[i + from];
    }

    lua_pushinteger(L, i + 1);
    return 1;
}

LUA_TYPE_MEMBERS(t_BUFFER) {
        {"__newindex", _buffer_set},
        {"__index",    _buffer_get},

        {"size",       _buffer_size},

        {"copyTo",     _buffer_copy_to},
        {"copyRange",  _buffer_copy_range},
        {NULL, NULL}
};

LUA_LIB_FUNCTION(buffer) {
        {"new", _buffer_create},
        {NULL, NULL}
};

LUAMOD_API int luaopen_buffer(lua_State *L) {
    luaU_registerType(L, t_BUFFER);

    luaL_newlib(L, buffer_function);
    return 1;
}