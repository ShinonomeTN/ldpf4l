//
// Created by cattenlinger on 20-3-4.
//

#include "lauxlib.h"
#include "lualib.h"

#include "../utils/lua_utils.h"

#include "mod_buffer.h"

#define t_BUFFER "ldpf4l.type.buffer"

typedef struct Buffer {
    unsigned long size;
    unsigned char buffer[1];
} Buffer;

static int lf_buffer_create(lua_State *L) {
    int size = luaL_checkinteger(L, 1);
    if (size <= 0) luaL_error(L, "error: buffer size must bigger than 0");

    ulong sizeBuff = sizeof(Buffer) + (size - 1);
    Buffer *buffer = (Buffer *) lua_newuserdata(L, sizeBuff);
    buffer->size = size;

    luaL_setmetatable(L, t_BUFFER);
    return 1;
}

static int lf_buffer_size(lua_State *L) {
    Buffer *buffer = (Buffer *) luaU_checkoutType(L, t_BUFFER);
    lua_pushinteger(L, buffer->size);
    return 1;
}

static int lf_buffer_set(lua_State *L) {
    Buffer *buffer = (Buffer *) luaU_checkoutType(L, t_BUFFER);
    unsigned long index = luaL_checkinteger(L, 2) + 1;
    if (!(index > 0 && index <= buffer->size)) luaL_error(L, "error: index out of range");
    buffer->buffer[index] = luaL_checkinteger(L, 2);

    return 0;
}

static int lf_buffer_get(lua_State *L) {
    Buffer *buffer = (Buffer *) luaU_checkoutType(L, t_BUFFER);
    unsigned long index = luaL_checkinteger(L, 2) + 1;
    if (!(index > 0 && index <= buffer->size)) luaL_error(L, "error: index out of range");
    lua_pushinteger(L, buffer->buffer[index]);

    return 1;
}

static int lf_buffer_copy_to(lua_State *L) {
    Buffer *buffer = (Buffer *) luaU_checkoutType(L, t_BUFFER);
    Buffer *target = (Buffer *) luaU_checkUserDataNotNull(L, t_BUFFER, 2);
    uint length = buffer->size < target->size ? target->size : buffer->size;

    int i;
    for (i = 0; i < length; ++i) {
        target->buffer[i] = buffer->buffer[i];
    }

    lua_pushinteger(L, i + 1);
    return 1;
}

static int lf_buffer_copy_range(lua_State *L) {
    Buffer *buffer = (Buffer *) luaU_checkoutType(L, t_BUFFER);

    int from = (int) luaL_checkinteger(L, 2) - 1;
    if (!(from >= 0 && from <= buffer->size)) luaL_error(L, "error: source start position out of range");
    int end = (int) luaL_checkinteger(L, 3) - 1;
    if (!(end >= 0 && end <= buffer->size)) luaL_error(L, "error: source start position out of range");
    if (from > end) luaL_error(L, "error: source start index after end");

    Buffer *target = (Buffer *) luaU_checkUserDataNotNull(L, t_BUFFER, 4);
    int start = (int) luaL_checkinteger(L, 5) - 1;
    if (!(start >= 0 && start <= target->size)) luaL_error(L, "error: target start position out of range");
    if (!(end >= 0 && (start - end) <= target->size)) luaL_error(L, "error: target start position out of range");

    int i;
    for (i = 0; i < from - end; i++) {
        target->buffer[i + start] = buffer->buffer[i + from];
    }

    lua_pushinteger(L, i + 1);
    return 1;
}

LUA_TYPE_DEFINE(t_BUFFER)
    MEMBER("__newindex", lf_buffer_set)
    MEMBER("__index", lf_buffer_get)
    MEMBER("size", lf_buffer_size)
    MEMBER("copyTo", lf_buffer_copy_to)
    MEMBER("copyRange", lf_buffer_copy_range)
LUA_TYPE_END

LUA_LIB_DEFINE(ldpf4l_Buffer)
    FUNCTION("new", lf_buffer_create)
LUA_LIB_END

LUA_LIB_EXPORT(ldpf4l_Buffer)
    EXPORT_TYPE(t_BUFFER)
    EXPORT_LIB(ldpf4l_Buffer)
LUA_LIB_EXPORT_END