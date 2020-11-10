//
// Created by cattenlinger on 20-3-7.
//
#include <stdio.h>

#include "lauxlib.h"
#include "lualib.h"

#include "../com/com_image.h"

#include "mod_image.h"
#include "../utils/log.h"
#include "../lib/stb_image.h"

#define t_IMAGE "ldpf4l.type.image"

static int lf_image_load_from_file(lua_State *L) {
    const char *filename = lua_tostring(L, 1);
//    log_trace("Open image file %s ", filename);
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        lua_pushnil(L);
        lua_pushstring(L, "error: could not open file");
        return 2;
    }
    ll_image *image = (ll_image *) lua_newuserdata(L, sizeof(ll_image));

    ll_image_load(image, file, 4);

    if (image->data == NULL) {
        lua_pushnil(L);
        lua_pushstring(L, "error: could not open image");
    }

    luaL_setmetatable(L, t_IMAGE);

    return 1;
}

static int lf_image_get_size(lua_State *L) {
    ll_image *image = (ll_image *) luaU_checkoutSelf(L, t_IMAGE);

    lua_newtable(L);
    luaU_tablePutObjectField(L, image, integer, width);
    luaU_tablePutObjectField(L, image, integer, height);

    return 1;
}

static int lf_image_get_info(lua_State *L) {
    ll_image *image = (ll_image *) luaU_checkoutSelf(L, t_IMAGE);

    lua_newtable(L);
    luaU_tablePutObjectField(L, image, integer, channels);

    lua_pushstring(L, "requiredChannels");
    lua_pushinteger(L, image->requiredChannel);
    lua_settable(L, -3);

    return 1;
}

static int lf_image_gc(lua_State *L) {
    log_trace("[%s gc]", t_IMAGE);

    ll_image *image = (ll_image *) luaU_checkoutSelf(L, t_IMAGE);

    ll_image_free(image);

    return 0;
}

LUA_TYPE_DEFINE(t_IMAGE)
    MEMBER("__gc", lf_image_gc)

    MEMBER("getSize", lf_image_get_size)
    MEMBER("getInfo", lf_image_get_info)
LUA_TYPE_END

LUA_LIB_DEFINE(ldpf4l_Image)
    MEMBER("loadFile", lf_image_load_from_file)
LUA_LIB_END

LUA_LIB_EXPORT(ldpf4l_Image)
    // Fix stbi iphone image problem
    stbi_convert_iphone_png_to_rgb(1);
    stbi_set_unpremultiply_on_load(1);

    EXPORT_TYPE(t_IMAGE)
    EXPORT_LIB(ldpf4l_Image)
LUA_LIB_EXPORT_END